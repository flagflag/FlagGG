//
// 屏幕空间反射
//

#include "Shader/Platform.hlsl"
#include "Shader/Define.hlsl"
#include "Shader/Common.hlsl"
#include "Shader/Sampler.hlsl"
#include "Shader/Random.hlsl"
#if PIXEL
#include "Shader/SSR/ScreenSpaceRayCast.hlsl"
#endif

#ifdef VERTEX
void VS(
    in float3 position : POSITION,
    out float4 outPosition : SV_POSITION,
    out float2 outTexcoord : TEXCOORD,
    out float3 outFarRay : FAR_RAY)
{
    float4x3 iWorldMatrix = worldMatrix;
    float3 worldPos = mul(float4(position, 1.0), iWorldMatrix);
    float4 clipPos  = mul(float4(worldPos, 1.0), projviewMatrix);

    // outPosition = clipPos;
    // outTexcoord = GetQuadTexCoord(clipPos);
    outPosition = float4(position, 1.0);
    outTexcoord = position.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);
    outFarRay = GetFarRay(clipPos);
}
#endif

#if PIXEL
// rgb: 屏幕空间法线
Texture2D GBuffer0Map : register(t0);
SamplerState GBuffer0Sampler: register(s0);

// b: 粗糙度
Texture2D GBuffer1Map : register(t1);
SamplerState GBuffer1Sampler : register(s1);

// rgb: base color
Texture2D GBuffer2Map : register(t2);
SamplerState GBuffer2Sampler : register(s2);

// 屏幕空间深度
Texture2D depthBuffer : register(t4);         
SamplerState depthBufferSampler : register(s4);

// Hi-Z map
Texture2D HiZTexture : register(t5);
SamplerState HiZTextureSampler : register(s5);

cbuffer HiZParam : register(b1)
{
    float roughnessMaskScale;
    float4 HiZUvFactorAndInvFactor;
}

float GetRoughnessFade(float roughness)
{
    return min(roughness * roughnessMaskScale + 2, 1);
}

void PS(
    in float4 position : SV_POSITION,
    in float2 texcoord : TEXCOORD,
    in float3 farRay : FAR_RAY,
    out float4 outColor : SV_TARGET)
{
    float roughness = GBuffer1Map.SampleLevel(GBuffer1Sampler, texcoord, 0).b;
    float roughnessFade = GetRoughnessFade(roughness);

	// Early out. Useless if using the stencil prepass.
	BRANCH
    if (roughnessFade <= 0.0)
	{
		return;
	}

    float sourceDepth = depthBuffer.SampleLevel(depthBufferSampler, texcoord, 0).r;
    float depth = ReconstructDepth(sourceDepth);

    float3 worldPosition = farRay * depth;
    float3 viewDirection = normalize(-worldPosition);
    worldPosition += cameraPos;

    uint numSteps = 16;
    uint numRays = 1;

    float normalDirection = DecodeGBufferNormal(GBuffer0Map.SampleLevel(GBuffer0Sampler, texcoord, 0).rgb);
    // float NdotV = clamp(dot(normalDirection, viewDirection), 0.0, 1.0);
    // float3 viewReflection = 2.0 * NdotV * normalDirection - viewDirection;
    float3 viewReflection = reflect(-viewDirection, normalDirection);

    outColor = 0;
    // float closestHitDistanceSqr = INFINITE_FLOAT;

    RayCastParam rcParam;
    rcParam.HiZTexture = HiZTexture;
    rcParam.HiZTextureSampler = HiZTextureSampler;
    rcParam.rayOrigin = worldPosition;
    rcParam.rayDirection = viewReflection;
    rcParam.roughness = roughness;
    rcParam.sceneDepth = depth;
    rcParam.numSteps = numSteps;
    // 类似TAA，时序抖动（减少发射的射线个数）
    rcParam.stepOffset = InterleavedGradientNoise(texcoord, frameNumberMod8) - 0.5;
    rcParam.HiZUvFactorAndInvFactor = HiZUvFactorAndInvFactor;

    float3 hitUVz;
    float level;

    BRANCH
    if (RayCastHiZ(rcParam, hitUVz, level))
    {
        // closestHitDistanceSqr = ComputeRayHitSqrDistance(worldPosition, hitUVz);

        outColor.rgb = GBuffer2Map.SampleLevel(GBuffer2Sampler, hitUVz.xy * (0.5, -0.5) + 0.5, 0).rgb;
    }
}

#endif
