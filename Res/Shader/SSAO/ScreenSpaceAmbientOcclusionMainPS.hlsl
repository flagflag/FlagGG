//
// 生成ao map
//

#include "Shader/Platform.hlsl"
#include "Shader/Define.hlsl"
#include "Shader/Common.hlsl"
#include "Shader/SSAO/ScreenSpaceAmbientOcclusionCommon.hlsl"

void AmbientOcclusionPass(in float2 uv, in float2 screenPos, float4 svPosition, out float4 outColor)
{
    outColor = 0;
    
    float3 fovFix = float3(invTanHalfFov, ratio * invTanHalfFov, 1);
	float3 invFovFix = 1.0f / fovFix;

    float sceneDepth = GetDepthFromAOInput(uv);
    float3 worldNormal = GetWorldSpaceNormalFromAOInput(uv, svPosition);

    float3 viewSpaceNormal = normalize(mul(worldNormal, (float3x3)viewMatrix));
    float3 viewSpacePosition = ReconstructCSPos(sceneDepth, screenPos);
    float actualAORadius = AORadiusInShader * lerp(sceneDepth, 1, scaleRadiusInWorldSpace);

#if USE_NORMALS
    // Add bias after fixup (causes minor banding - not needed with larger radius)
    viewSpacePosition += ambientOcclusionBias * sceneDepth * scaleFactor * (viewSpaceNormal * fovFix);
#endif

    float2 weightAccumulator = 0.0001f;

#if AO_SAMPLE_QUALITY != 0
    // no SSAO
#if AO_SAMPLE_QUALITY == 1
    // no 4x4 randomization
    float2 randomVec = float2(0, 1) * actualAORadius;
#elif AO_SAMPLE_QUALITY == 2
    float2 randomVec = (randomNormalTexture.SampleLevel(randomNormalSampler, uv * viewportUVToRandomUV, 0).xy * 2 - 1) * actualAORadius;
#else // AO_SAMPLE_QUALITY == 3
	// extract one of 16 base vectors (rotation and scale) from a texture that repeats 4x4, changing over time if TemporalAA is enabled

	// if the feature is enabled and right side of screen
	const bool bDebugLookups = DEBUG_LOOKUPS && viewSpacePosition.x > 0;

    // jitter each frame a bit to get higher quality over multiple frames (only if TemporalAA is enabled), can cause ghosting effects
    float2 randomVec = (randomNormalTexture.SampleLevel(randomNormalSampler, temporalOffset + uv * viewportUVToRandomUV, 0).xy * 2 - 1) * actualAORadius;
#endif
    {
        if (bDebugLookups && viewSpacePosition.y > 0)
        {
            randomVec = float2(0, 1) * actualAORadius;
        }

        float2 fovFixXY = fovFix.xy * (1.0f / viewSpacePosition.z);
        float4 randomBase = float4(randomVec, -randomVec.y, randomVec.x) * float4(fovFixXY, fovFixXY);

        float2 screenSpacePos = viewSpacePosition.xy / viewSpacePosition.z;

		// .x means for very anisotropic viewports we scale by x
		float invHaloSize = 1.0f / (actualAORadius * fovFixXY.x * 2);

        float3 scaledViewSpaceNormal = viewSpaceNormal;

#if OPTIMIZATION_O1
        scaledViewSpaceNormal *= 0.08f * lerp(sceneDepth, 1000, scaleRadiusInWorldSpace);
#endif

        UNROLL
        for (int i = 0; i < SAMPLESET_ARRAY_SIZE; ++i)
        {
            // -1..1
            float2 unrotatedRandom = occlusionSamplesOffsets[i].xy;

            float2 localRandom = (unrotatedRandom.x * randomBase.xy + unrotatedRandom.y * randomBase.zw);

            if (bDebugLookups)
            {
                UNROLL
                for (uint step = 0; step < SAMPLE_STEPS; ++step)
                {
                    float scale = (step + 1) / (float)SAMPLE_STEPS;
                    float mipLevel = ComputeMipLevel(i, step);
                    float2 scaledLocalRandom = scale * localRandom;

                    weightAccumulator += float2(ComputeSampleDebugMask(screenSpacePos + scaledLocalRandom, mipLevel), 1.0f);
                    weightAccumulator += float2(ComputeSampleDebugMask(screenSpacePos - scaledLocalRandom, mipLevel), 1.0f);
                }
            }
            else if (USE_NORMALS)
            {
                float3 localAccumulator = 0;

                UNROLL
                for (uint step = 0; step < SAMPLE_STEPS; ++step)
				{
					// constant at run time
					float scale = (step + 1) / (float)SAMPLE_STEPS;
					// constant at run time (higher is better for texture cache / performance, lower is better quality
					float mipLevel = ComputeMipLevel(i, step);

					float3 stepSample = WedgeWithNormal(screenSpacePos, scale * localRandom, invFovFix, viewSpacePosition, scaledViewSpaceNormal, invHaloSize, mipLevel);

					// combine horizon samples
					localAccumulator = lerp(localAccumulator, float3(max(localAccumulator.xy, stepSample.xy), 1), stepSample.z);
				}

				// Square(): the area scales quadratic with the angle - it gets a bit darker
				weightAccumulator += float2(Square(1 - localAccumulator.x) * localAccumulator.z, localAccumulator.z);
				weightAccumulator += float2(Square(1 - localAccumulator.y) * localAccumulator.z, localAccumulator.z);
				// cheaper? Could move 1 - out
				// weightAccumulator += float2(1 - localAccumulator.x, localAccumulator.y);
            }
            else // No normals
            {
                float2 localAccumulator = 0;

                UNROLL
                for (uint step = 0; step < SAMPLE_STEPS; ++step)
				{
					// constant at run time
					float scale = (step + 1) / (float)SAMPLE_STEPS;
					// constant at run time (higher is better for texture cache / performance, lower is better quality
					float mipLevel = ComputeMipLevel(i, step);

					float2 stepSample = WedgeNoNormal(screenSpacePos, scale * localRandom, invFovFix, viewSpacePosition, invHaloSize, mipLevel);

					// combine horizon samples
					localAccumulator = lerp(localAccumulator, float2(max(localAccumulator.x, stepSample.x), 1), stepSample.y);
				}

				// Square(): the area scales quadratic with the angle - it gets a bit darker
				weightAccumulator += float2(Square(1 - localAccumulator.x) * localAccumulator.y, localAccumulator.y);
            }
        }
    }
#endif

    outColor.r = weightAccumulator.x / weightAccumulator.y;
    outColor.gb = float2(0, 0);

    if (!bDebugLookups)
    {
        // 做法类型MSSAO，采样DownsampledAO、DownsampledNormal，和当前计算的ao做一次合并
        // 这样做是考虑到远处物体的AO
        float4 filtered = ComputeUpsampleContribution(sceneDepth, uv, worldNormal);

        outColor.r = lerp(outColor.r, filtered.r, ComputeLerpFactor());
    }

#if !USE_AO_SETUP_AS_INPUT
    if (!bDebugLookups)
    {
		// full res

		// soft fade out AO in the distance
		{
            float fadeRadius = max(1.0f, ambientOcclusionFadeRadius);
            float invFadeRadius = 1.0f / fadeRadius;
			float fadeMul = invFadeRadius;
			float fadeAdd = -(ambientOcclusionFadeDistance - fadeRadius) * invFadeRadius;
			outColor.r = lerp(outColor.r, 1, saturate(sceneDepth * fadeMul + fadeAdd));
		}

		// user adjust AO
		// abs() to prevent shader warning
		outColor.r = 1 - (1 - pow(abs(outColor.r), ambientOcclusionPower)) * ambientOcclusionIntensity;

		// we output in a single alpha channel
		outColor = outColor.r;
    }
    else
    {
		outColor.r = pow(1 - outColor.r, 16);	// constnt is tweaked with radius and sample count
    }
#endif

	// we don't support ddx_fine() for SM4
    // Blur操作，通过ddx/ddy得到x和y方向上的两个相邻像素的AO值，比较本像素与相邻两个像素的Normal和Depth的关系计算出权重，最终进行Blur并输出
#if !COMPUTE_SHADER && QUAD_MESSAGE_PASSING_BLUR > 0
	{
		// .x: AO output, .y:sceneDepth .zw:view space normal
		float4 centerPixel = float4(outColor.r, sceneDepth, normalize(viewSpaceNormal).xy);

		float4 dX = ddx_fine(centerPixel);
		float4 dY = ddy_fine(centerPixel);

		int2 mod = (uint2)(svPosition.xy) % 2;

		float4 pixA = centerPixel;
		float4 pixB = centerPixel - dX * (mod.x * 2 - 1);
		float4 pixC = centerPixel - dY * (mod.y * 2 - 1);

		float weightA = 1.0f;
		float weightB = 1.0f;
		float weightC = 1.0f;

#if QUAD_MESSAGE_PASSING_NORMAL
		const float normalTweak = 4.0f;
		float3 normalA = ReconstructNormal(pixA.zw);
		float3 normalB = ReconstructNormal(pixB.zw);
		float3 normalC = ReconstructNormal(pixC.zw);
		weightB *= saturate(pow(saturate(dot(normalA, normalB)), normalTweak));
		weightC *= saturate(pow(saturate(dot(normalA, normalC)), normalTweak));
#endif

#if QUAD_MESSAGE_PASSING_DEPTH
		const float depthTweak = 1;
		float invDepth = 1.0f / pixA.y;
		weightB *= 1 - saturate(abs(1 - pixB.y * invDepth) * depthTweak);
		weightC *= 1 - saturate(abs(1 - pixC.y * invDepth) * depthTweak);
#endif

		// + 1.0f to avoid div by 0
		float invWeightABC = 1.0f / (weightA + weightB + weightC);

		weightA *= invWeightABC;
		weightB *= invWeightABC;
		weightC *= invWeightABC;

		outColor = weightA * pixA.x + weightB * pixB.x + weightC * pixC.x;
		// visualize where we don't want to fade
        // outColor = (weightA - 0.333f) / 0.666f;
	}
#endif
}

#if PIXEL
void PS(
    in float4 position  : SV_POSITION,
    in float2 texcoord  : TEXCOORD,
    in float2 screenPos : TEXCOORD1,
    out float4 outColor : SV_TARGET)
{
    AmbientOcclusionPass(texcoord, screenPos, position, outColor);
}
#elif COMPUTE
/** Output target. In compute, this is a single value buffer. */
RWTexture2D<float> outTexture;

[numthreads(THREADGROUP_SIZEX, THREADGROUP_SIZEY, 1)]
void CS(
    uint2 groupId : SV_GroupID,
    uint2 dispatchThreadId : SV_DispatchThreadID,
    uint2 groupTheadId : SV_GroupThreadID)
{
    int2 pixelPos = dispatchThreadId + AOViewport_ViewportMin;
    float2 pixelCenter = (float2)pixelPos + float2(0.5, 0.5);

    float4 svPosition = float4(pixelCenter, 0, 0) * scaleFactor;
    float2 bufferUV = SvPositionToBufferUV(svPosition);
    // svPosition.w = ConvertFromDeviceZ(svPosition.z);
    svPosition.w = 1;

    float4 outColor = 1;

	// Test for early exit with out of depth bound.
	float sceneDepth = ConvertFromDeviceZ(svPosition.z);
    float fadeRadius = max(1.0f, ambientOcclusionFadeRadius);
    float invFadeRadius = 1.0f / fadeRadius;
    float fadeMul = invFadeRadius;
    float fadeAdd = -(ambientOcclusionFadeDistance - fadeRadius) * invFadeRadius;
    BRANCH
    if (sceneDepth * fadeMul + fadeAdd < 1)
    {
        AmbientOcclusionPass(float4(bufferUV, SvPositionToScreenPosition(svPosition).xy), svPosition, outColor);
    }

    outTexture[pixelPos] = outColor.r;
}
#endif
