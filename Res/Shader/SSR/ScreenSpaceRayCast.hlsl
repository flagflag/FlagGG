//
// 屏幕空间射线检测
// 本质上是基于HiZ做RayTracing
//

// Number of sample batched at same time.
#define SSRT_SAMPLE_BATCH_SIZE 4

#define STOP_WHEN_UNCERTAINM false

#ifndef DEBUG_SSR
	#define DEBUG_SSR 0
#endif

/** Return float multiplier to scale rayStepScreen such that it clip it right at the edge of the screen. */
float GetStepScreenFactorToClipAtScreenEdge(float2 rayStartScreen, float2 rayStepScreen)
{
	// Computes the scale down factor for rayStepScreen required to fit on the X and Y axis in order to clip it in the viewport
	const float rayStepScreenInvFactor = 0.5 * length(rayStepScreen);
	const float2 S = 1 - max(abs(rayStepScreen + rayStartScreen * rayStepScreenInvFactor) - rayStepScreenInvFactor, 0.0f) / abs(rayStepScreen);

	// Rescales rayStepScreen accordingly
	const float rayStepFactor = min(S.x, S.y) / rayStepScreenInvFactor;

	return rayStepFactor;
}

struct SSRRay
{
	float3 rayStartScreen;           // 射线发射点（屏幕空间坐标）
	float3 rayStepScreen;            // 射线步进向量，包含长度信息，可以理解rayStartScreen + rayStepScreen就是射线的末端点（屏幕空间坐标）
	float compareTolerance;          // 精度
};

/** Compile a ray for screen space ray casting. */
SSRRay InitScreenSpaceRayFromWorldSpace(
	float3 rayOrigin,
	float3 rayDirection,
	float worldTMax,
	float sceneDepth,
	float slopeCompareToleranceScale,
	const bool bExtendRayToScreenBorder,
	out bool bRayWasClipped)
{
	worldTMax = min(worldTMax, 1000000);

    // 射线方向（相机空间）
	float3 viewRayDirection = mul(float4(rayDirection, 0.0), viewMatrix).xyz;

    // 射线最远边界的距离（相机空间）
	float rayEndWorldDistance = viewRayDirection.z < 0.0 ? min(-0.95 * sceneDepth / viewRayDirection.z, worldTMax) : worldTMax;

    // 射线最远边界的位置
	float3 rayEndWorld = rayOrigin + rayDirection * rayEndWorldDistance;

    // 投影空间
	float4 rayStartClip = mul(float4(rayOrigin, 1.0), projviewMatrix);
	float4 rayEndClip = mul(float4(rayEndWorld, 1.0), projviewMatrix);

    // 屏幕空间
	float3 rayStartScreen = rayStartClip.xyz * rcp(rayStartClip.w);
	float3 rayEndScreen = rayEndClip.xyz * rcp(rayEndClip.w);

    // 射线最远边界的深度
	float4 rayDepthClip = rayStartClip + mul(float4(0, 0, rayEndWorldDistance, 0), projMatrix);
	float3 rayDepthScreen = rayDepthClip.xyz * rcp(rayDepthClip.w);

	SSRRay ray;
	ray.rayStartScreen = rayStartScreen;
	ray.rayStepScreen = rayEndScreen - rayStartScreen;

	float clipToScreenFactor = GetStepScreenFactorToClipAtScreenEdge(rayStartScreen.xy, ray.rayStepScreen.xy);
	if (!bExtendRayToScreenBorder)
	{
		bRayWasClipped = clipToScreenFactor < 1.0 || rayEndWorldDistance != worldTMax;
		clipToScreenFactor = min(clipToScreenFactor, 1.0);
	}
	else
	{
		bRayWasClipped = true;
	}

	ray.rayStepScreen *= clipToScreenFactor;
	ray.compareTolerance = max(abs(ray.rayStepScreen.z), (rayStartScreen.z - rayDepthScreen.z) * slopeCompareToleranceScale);

	return ray;
}

struct RayCastParam
{
    Texture2D HiZTexture;             // HiZ 纹理
    SamplerState HiZTextureSampler;   // HiZ 纹理采样器
    float3 rayOrigin;                 // 射线发射点
    float3 rayDirection;              // 射线防线
    float roughness;                  // 粗糙度
    float sceneDepth;                 // 场景深度
    uint numSteps;                    // 射线步进次数
    float stepOffset;                 // 射线初始位置朝步进方向的偏移
    float4 HiZUvFactorAndInvFactor;   // UV缩放
};

/** Cast a screen space ray. */
void CastScreenSpaceRay(
    RayCastParam rcParam,
    float startMipLevel,
    SSRRay ray,
	out float3 outHitUVz,
	out float level,
	out bool bFoundHit,
	out bool bUncertain)
{
	const float3 rayStartScreen = ray.rayStartScreen;
	const float3 rayStepScreen = ray.rayStepScreen;

	// 从屏幕空间转换到UV空间
	float3 rayStartUVz = float3((rayStartScreen.xy * float2(0.5, -0.5) + 0.5) * rcParam.HiZUvFactorAndInvFactor.xy, rayStartScreen.z);
	float3 rayStepUVz  = float3(rayStepScreen.xy   * float2(0.5, -0.5)		  * rcParam.HiZUvFactorAndInvFactor.xy, rayStepScreen.z);
	
	// 射线单次步进占总长度的比例
	const float step = 1.0 / rcParam.numSteps;
	float compareTolerance = ray.compareTolerance * step;
	
	float lastDiff = 0;
	level = startMipLevel;

	// * step 算出单次步进的向量
	rayStepUVz *= step;
	float3 rayUVz = rayStartUVz + rayStepUVz * rcParam.stepOffset;
	
	#if DEBUG_SSR
		PrintSample(rcParam.HiZUvFactorAndInvFactor, rayStartUVz.xy, float4(1, 0, 0, 1));
	#endif
	
	float4 multipleSampleDepthDiff;
	bool4 bMultipleSampleHit; // TODO: Might consumes VGPRS if bug in compiler.

	// Compute the maximum of iteration.
	uint maxIteration = rcParam.numSteps;

	uint i;

	bFoundHit = false;
	bUncertain = false;

	// GPU指令的特性：float4一次计算和float一次计算耗时等价
	// 因此这里使用float4，每次计算4个像素的Hit
    LOOP
	for (i = 0; i < maxIteration; i += SSRT_SAMPLE_BATCH_SIZE)
	{
		float2 samplesUV[SSRT_SAMPLE_BATCH_SIZE];
		float4 samplesZ;
		float4 samplesMip;

		// Compute the sample coordinates.
		{
			UNROLL_N(SSRT_SAMPLE_BATCH_SIZE)
			for (uint j = 0; j < SSRT_SAMPLE_BATCH_SIZE; j++)
			{
				samplesUV[j] = rayUVz.xy + (float(i) + float(j + 1)) * rayStepUVz.xy;
				samplesZ[j] = rayUVz.z + (float(i) + float(j + 1)) * rayStepUVz.z;
			}
		
			samplesMip.xy = level;
			level += (8.0 / rcParam.numSteps) * rcParam.roughness;
		
			samplesMip.zw = level;
			level += (8.0 / rcParam.numSteps) * rcParam.roughness;
		}

		// Sample the scene depth.
		float4 sampleDepth;
		{
			UNROLL_N(SSRT_SAMPLE_BATCH_SIZE)
			for (uint j = 0; j < SSRT_SAMPLE_BATCH_SIZE; j++)
			{
				#if DEBUG_SSR
					PrintSample(rcParam.HiZUvFactorAndInvFactor, samplesUV[j], float4(0, 1, 0, 1));
				#endif
				sampleDepth[j] = rcParam.HiZTexture.SampleLevel(rcParam.HiZTextureSampler, samplesUV[j], samplesMip[j]).r;
			}
		}

		// Evaluates the intersections.
		multipleSampleDepthDiff = samplesZ - sampleDepth;
		bMultipleSampleHit = abs(multipleSampleDepthDiff + compareTolerance) < compareTolerance;

		// Determines whether the ray is uncertain because might be going beind geometry.
		{
			bool4 bMultipleSampleUncertain = (multipleSampleDepthDiff + compareTolerance) < -compareTolerance;

			UNROLL_N(SSRT_SAMPLE_BATCH_SIZE)
			for (uint j = 0; j < SSRT_SAMPLE_BATCH_SIZE; j++)
			{
				bFoundHit = bFoundHit || bMultipleSampleHit[j];
				bool bLocalMultisampleUncertain = bMultipleSampleUncertain[i];
				bUncertain = bUncertain || (bLocalMultisampleUncertain && !bFoundHit);
			}
		}

        BRANCH
		if (bFoundHit || (STOP_WHEN_UNCERTAINM && bUncertain))
		{
			break;
		}

		lastDiff = multipleSampleDepthDiff.w;
	} // for( uint i = 0; i < rcParam.numSteps; i += 4 )
	
	// Compute the output coordinates.
    BRANCH
	if (bFoundHit)
    {
    // SSR
        float depthDiff0 = multipleSampleDepthDiff[2];
        float depthDiff1 = multipleSampleDepthDiff[3];
        float time0 = 3;

        FLATTEN
        if (bMultipleSampleHit[2])
        {
            depthDiff0 = multipleSampleDepthDiff[1];
            depthDiff1 = multipleSampleDepthDiff[2];
            time0 = 2;
        }
        FLATTEN
        if (bMultipleSampleHit[1])
        {
            depthDiff0 = multipleSampleDepthDiff[0];
            depthDiff1 = multipleSampleDepthDiff[1];
            time0 = 1;
        }
        FLATTEN
        if (bMultipleSampleHit[0])
        {
            depthDiff0 = lastDiff;
            depthDiff1 = multipleSampleDepthDiff[0];
            time0 = 0;
        }

        time0 += float(i);

        float time1 = time0 + 1;

        // Find more accurate hit using line segment intersection
        float timeLerp = saturate(depthDiff0 / (depthDiff0 - depthDiff1));
        float intersectTime = time0 + timeLerp;
        //float intersectTime = lerp( time0, time1, timeLerp );
            
        outHitUVz = rayUVz + rayStepUVz * intersectTime;
		
		#if DEBUG_SSR
			PrintSample(rcParam.HiZUvFactorAndInvFactor, outHitUVz.xy, float4(0, 0, 1, 1));}
		#endif
    }
	else
    {
		// Uncertain intersection - conservatively pull back to an iteration which didn't intersect
        outHitUVz = rayUVz + rayStepUVz * i;
    }

	outHitUVz.xy *= rcParam.HiZUvFactorAndInvFactor.zw;
	outHitUVz.xy = outHitUVz.xy * float2(2, -2) + float2(-1, 1);
	outHitUVz.xy = outHitUVz.xy * screenPositionScaleBias.xy + screenPositionScaleBias.wz;
}

bool RayCastHiZ(RayCastParam rcParam, out float3 outHitUVs, out float level)
{
	float slopeCompareToleranceScale = 4.0f;

	bool bRayWasClipped;
	SSRRay ray = InitScreenSpaceRayFromWorldSpace(
		rcParam.rayOrigin, rcParam.rayDirection,
		rcParam.sceneDepth,
		rcParam.sceneDepth,
		slopeCompareToleranceScale,
		true,
		bRayWasClipped);

	bool bHit;
	bool bUncertain;
	CastScreenSpaceRay(
		rcParam,
		1.0,
		ray,
		outHitUVs,
		level,
		bHit,
		bUncertain);

    return bHit;
}

float ConvertFromDeviceZ(float deviceZ)
{
    return ReconstructDepth(deviceZ);
}

// float ComputeRayHitSqrDistance(float3 worldPosition, float3 hitUVz)
// {
// 	// ALU get factored out with ReprojectHit.
// 	float2 hitScreenPos = (hitUVz.xy - screenPositionScaleBias.wz) / screenPositionScaleBias.xy;
// 	float hitSceneDepth = ConvertFromDeviceZ(hitUVz.z);

// 	float3 hitPosition = mul(float4(hitScreenPos * hitSceneDepth, hitSceneDepth, 1), View.ScreenToTranslatedWorld).xyz;

// 	return length2(worldPosition - hitSceneDepth);
// }
