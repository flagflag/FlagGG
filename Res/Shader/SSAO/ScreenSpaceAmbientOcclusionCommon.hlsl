#include "Shader/SSAO/ScreenSpaceAmbientOcclusionDefines.hlsl"

float GetHZBDepth(float2 screenPos, float mipLevel)
{
	float2 HiZUV = screenPos * HiZRemapping.xy + HiZRemapping.zw;
	float HiZDepth = HiZTexture.SampleLevel(HiZSampler, HiZUV, mipLevel).r;
	return ConvertFromDeviceZ(HiZDepth);
}

// could be moved to a more central spot
// @param screenPos -1 .. 1
float3 ReconstructCSPos(float sceneDepth, float2 screenPos)
{
	return float3(screenPos * sceneDepth, sceneDepth);
}

// 0: not similar .. 1:very similar
float ComputeDepthSimilarity(float depthA, float depthB, float tweakScale)
{
	return saturate(1 - abs(depthA - depthB) * tweakScale);
}

// @return can be 0,0,0 if we don't have a good input normal
float3 GetWorldSpaceNormalFromAOInput(float2 uv, float4 svPosition)
{
	float3 worldNormal = 0;

	if (USE_NORMALS)
	{
	#if USE_AO_SETUP_AS_INPUT
		// Low resolution normal computed in the setup (downscaled) pass.
		worldNormal = DecodeGBufferNormal(SSAO_SetupTexture.SampleLevel(SSAO_SetupTexutreSampler, frac(uv), 0).xyz);
	#elif COMPUTE_SHADER || FORWARD_SHADING || SHADING_PATH_MOBILE
		// Async compute and forward shading don't have access to the gbuffer.
		worldNormal = ReconstructNormalFromDepthBuffer(svPosition);
	#else
		// Otherwise sample Gbuffer normals if the shader model has normals.
		worldNormal = DecodeGBufferNormal(screenNormalTexture.SampleLevel(screenNormalSampler, uv, 0).xyz);
	#endif
	}

	return worldNormal;
}

float4 ComputeUpsampleContribution(float sceneDepth, float2 inUV, float3 centerWorldNormal)
{
	// can be optimized
#if AO_UPSAMPLE_QUALITY == 0
	const int sampleCount = 4;
	float2 UV[sampleCount];

	UV[0] = inUV + float2(-0.5f,  0.5f) * SSAO_DownsampledAOInverseSize;
	UV[1] = inUV + float2( 0.5f,  0.5f) * SSAO_DownsampledAOInverseSize;
	UV[2] = inUV + float2(-0.5f, -0.5f) * SSAO_DownsampledAOInverseSize;
	UV[3] = inUV + float2( 0.5f, -0.5f) * SSAO_DownsampledAOInverseSize;
#else // AO_UPSAMPLE_QUALITY == 0
	const int sampleCount = 9;
	float2 UV[sampleCount];

	UV[0] = inUV + float2( -1, -1) * SSAO_DownsampledAOInverseSize;
	UV[1] = inUV + float2(  0, -1) * SSAO_DownsampledAOInverseSize;
	UV[2] = inUV + float2(  1, -1) * SSAO_DownsampledAOInverseSize;
	UV[3] = inUV + float2( -1,  0) * SSAO_DownsampledAOInverseSize;
	UV[4] = inUV + float2(  0,  0) * SSAO_DownsampledAOInverseSize;
	UV[5] = inUV + float2(  1,  0) * SSAO_DownsampledAOInverseSize;
	UV[6] = inUV + float2( -1,  1) * SSAO_DownsampledAOInverseSize;
	UV[7] = inUV + float2(  0,  1) * SSAO_DownsampledAOInverseSize;
	UV[8] = inUV + float2(  1,  1) * SSAO_DownsampledAOInverseSize;
#endif // AO_UPSAMPLE_QUALITY == 0

	// to avoid division by 0
	float smallValue = 0.0001f;

	// we could weight the samples better but tests didn't show much difference
	float weightSum = smallValue;
	float4 ret = float4(smallValue,0,0,0);

	float invThreshold = ambientOcclusionMipThreshold / scaleFactor;
	float minIteration = 1.0f;

	UNROLL
    for(int i = 0; i < sampleCount; ++i)
	{
		float4 sampleValue = SSAO_DownsampledAO.SampleLevel(SSAO_DownsampledAOSampler, UV[i], 0);

		minIteration = min(minIteration, sampleValue.g);

		float4 normalAndSampleDepth = SSAO_NormalsTexture.SampleLevel(SSAO_NormalsTextureSampler, UV[i], 0);
		float sampleDepth = normalAndSampleDepth.a * Constant_Float16F_Scale;

		// when tweaking this constant look for crawling pattern at edges
		float weight = ComputeDepthSimilarity(sampleDepth, sceneDepth, 0.003f);

		if (USE_NORMALS)
		{
			float3 localWorldNormal = normalAndSampleDepth.xyz * 2 - 1;
			weight *= saturate(dot(localWorldNormal, centerWorldNormal));
		}

		// todo: 1 can be put into the input to save an instruction
		ret += float4(sampleValue.rgb, 1) * weight;
		weightSum += weight;
	}

	ret /= weightSum;
	ret.g = minIteration;

	return ret;
}

// to blend between upsampled and current pass data
float ComputeLerpFactor()
{
	// set up on C++ side
	float mipBlend = ambientOcclusionMipBlend;

	float AOLerpFactor = mipBlend;

#if AO_SAMPLE_QUALITY == 0
	// we have no AO, we only use the upsampled data
	AOLerpFactor = 1.0f;
#endif

#if USE_UPSAMPLE == 0
	// if there is no former pass we cannot use the data
	AOLerpFactor = 0.0f;
#endif
	
	return AOLerpFactor;
}

// @return NormAngle means 0..1 is actually 0..PI
float acosApproxNormAngle(float x)
{
	// todo: expose
	// 1: is a good linear approximation, 0.9f seems to look good
	float contrastTweak = 0.9f;

	// correct: acos(x) / PI
	// linear approximation: saturate((1 - x) * 0.5f);
	// pretty good approximation with contrast tweak
	return saturate((1 - x) * 0.5f * contrastTweak);
}

// @return float3(invNormAngleL, invNormAngleR, weight)\
// SSAO计算与HBAO非常类似、同样会跑正反两个方向，但是在AO计算方程上做了些trick：
// 1.在可见性计算V上，不使用Sin(S)-Sin(H)、而是直接用的向量ω与P点法线N之间的cos值；
// 2.在衰减方程W上，直接用向量ω的模长代替
float3 WedgeWithNormal(float2 screenSpacePosCenter, float2 inLocalRandom, float3 invFovFix, float3 viewSpacePosition, float3 scaledViewSpaceNormal, float invHaloSize, float mipLevel)
{
    // 计算正反两个方向的Step终点位置
	float2 screenSpacePosL = screenSpacePosCenter + inLocalRandom;
	float2 screenSpacePosR = screenSpacePosCenter - inLocalRandom;

    // 采样深度
	float absL = GetHZBDepth(screenSpacePosL, mipLevel);
	float absR = GetHZBDepth(screenSpacePosR, mipLevel);

    // 反构造出点S
	float3 samplePositionL = ReconstructCSPos(absL, screenSpacePosL);
	float3 samplePositionR = ReconstructCSPos(absR, screenSpacePosR);

    // 得到向量ω
	float3 deltaL = (samplePositionL - viewSpacePosition) * invFovFix;
	float3 deltaR = (samplePositionR - viewSpacePosition) * invFovFix;
		
#if OPTIMIZATION_O1
    // 等价于 cos<N, ω> * |ω| / |N|
	float invNormAngleL = saturate(dot(deltaL, scaledViewSpaceNormal) / dot(deltaL, deltaL));
	float invNormAngleR = saturate(dot(deltaR, scaledViewSpaceNormal) / dot(deltaR, deltaR));
	float weight = 1;
#else
	float invNormAngleL = saturate(dot(deltaL, scaledViewSpaceNormal) * rsqrt(dot(deltaL, deltaL)));
	float invNormAngleR = saturate(dot(deltaR, scaledViewSpaceNormal) * rsqrt(dot(deltaR, deltaR)));

	float weight = 
		  saturate(1.0f - length(deltaL) * invHaloSize)
		* saturate(1.0f - length(deltaR) * invHaloSize);
#endif

	return float3(invNormAngleL, invNormAngleR, weight);
}

// @return float2(InvNormAngle, weight)
float2 WedgeNoNormal(float2 screenSpacePosCenter, float2 inLocalRandom, float3 invFovFix, float3 viewSpacePosition, float invHaloSize, float mipLevel)
{
	float2 screenSpacePosL = screenSpacePosCenter + inLocalRandom;
	float2 screenSpacePosR = screenSpacePosCenter - inLocalRandom;

	float absL = GetHZBDepth(screenSpacePosL, mipLevel);
	float absR = GetHZBDepth(screenSpacePosR, mipLevel);
	
	float3 samplePositionL = ReconstructCSPos(absL, screenSpacePosL);
	float3 samplePositionR = ReconstructCSPos(absR, screenSpacePosR);
	
	float3 deltaL = (samplePositionL - viewSpacePosition) * invFovFix;
	float3 deltaR = (samplePositionR - viewSpacePosition) * invFovFix;

	float weightLeft;
	float3 samplePositionLeft;
	{
		weightLeft = 1;

#if !OPTIMIZATION_O1
		weightLeft = saturate(1.0f - length(deltaL) * invHaloSize);
#endif
	}

	float weightRight;
	float3 samplePositionRight;
	{	
		weightRight = 1;

#if !OPTIMIZATION_O1
		weightRight = saturate(1.0f - length(deltaR) * invHaloSize);
#endif
	}


	float flatSurfaceBias = 5.0f;

	float left = viewSpacePosition.z - absL;
	float right = viewSpacePosition.z - absR;

	// OptionA: accurate angle computation
	float normAngle = acosApproxNormAngle(dot(deltaL, deltaR) / sqrt(length2(deltaL) * length2(deltaR)));
	// OptionB(fade out in near distance): float normAngle = acosApproxNormAngle( (- left - right) * 20);
	// OptionC(look consistent but more noisy, should be much faster): float normAngle = 0;


	// not 100% correct but simple
	// bias is needed to avoid flickering on almost perfectly flat surfaces
	//	    if((leftAbs  + rightAbs) * 0.5f > sceneDepth - 0.0001f)
	if (left + right < flatSurfaceBias)
	{
		// fix concave case
		normAngle = 1;
	}

	// to avoid halos around objects
	float weight = 1;
				
	float viewDepthAdd = 1.0f - viewSpacePosition.z * invAmbientOcclusionDistance;

	weight *= saturate(samplePositionL.z * invAmbientOcclusionDistance + viewDepthAdd);
	weight *= saturate(samplePositionR.z * invAmbientOcclusionDistance + viewDepthAdd);

//	return float2(1 - normAngle, (weightLeft + weightRight) * 0.5f);
	return float2((1 - normAngle) / (weight + 0.001f), weight);
}

float3 ReconstructNormal(float2 normalXY)
{
	return float3(normalXY, sqrt(1 - dot(normalXY, normalXY)));
}

// @param screenSpacePos -1..1
// @return 1 if inside the center, 0 if outside
float ComputeSampleDebugMask(float2 screenSpacePos, float mipLevel)
{
	screenSpacePos.x -= 0.5f;

	screenSpacePos.y = frac(screenSpacePos.y) - 0.5f;

	float2 viewPortSize = AOViewport_ViewportSize;
	int2 pixelOffsetToCenter = int2(screenSpacePos * viewPortSize * 0.5f);

	float d = length(pixelOffsetToCenter);

	// revisit this
	float radius = 12.0f;

	// hard
	return d < radius * exp2(mipLevel);
	// soft
    //	return saturate(1 - d / (radius * exp2(mipLevel)));
}

float ComputeMipLevel(int sampleid, int step)
{
	float samplePos = (sampleid + 0.5f) / SAMPLESET_ARRAY_SIZE;

	// use a constant to get better performance
	// float HiZStepMipLevelFactor = 0.5f;
    // float HiZStepMipLevelFactor = 1;

	float scale = (step + 1) / (float)SAMPLE_STEPS;

    // return log2(1.0f + HiZStepMipLevelFactor * scale * samplePos);
	return log2(HiZStepMipLevelFactor * scale * samplePos);
}

float GetDepthFromAOInput(float2 uv)
{	
#if USE_AO_SETUP_AS_INPUT
	// low resolution
	return SSAO_SetupTexture.SampleLevel(SSAO_SetupTexutreSampler, uv, 0).a * Constant_Float16F_Scale;
#else
	// full resolution 
	return ConvertFromDeviceZ(screenDepthTexture.SampleLevel(screenDepthSampler, uv, 0).r);
#endif
}
