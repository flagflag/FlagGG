//
// 降采样normal（为了更好的性能）
//

#include "Shader/Platform.hlsl"
#include "Shader/Define.hlsl"
#include "Shader/Common.hlsl"
#include "Shader/SSAO/ScreenSpaceAmbientOcclusionCommon.hlsl"

cbuffer HiZParam : register(b2)
{
    float2 invSize;           // 当前视口大小的倒数
    float thresholdInverse;
}

void PS(
    in float4 position : SV_POSITION,
    in float2 texcoord : TEXCOORD,
    out float4 outColor : SV_TARGET)
{

    float4 samples[4];
    float2 uv[4] = 
    {
        texcoord + float2(-0.5f, -0.5f) * invSize,
        texcoord + float2(-0.5f,  0.5f) * invSize,
        texcoord + float2(0.5f,  -0.5f) * invSize,
        texcoord + float2(0.5f,   0.5f) * invSize
    };

    UNROLL
    for (uint i = 0; i < 4; ++i)
    {
        samples[i].rgb = screenNormalTexture.SampleLevel(screenNormalSampler, uv[i], 0).rgb;
        samples[i].a = ConvertFromDeviceZ(screenDepthTexture.SampleLevel(screenDepthSampler, uv[i], 0).r);
    }

    float maxZ = max(max(samples[0].a, samples[1].a), max(samples[2].a, samples[3].a));

#if USE_NORMALS
    float4 avgerColor = 0.0001f;
    UNROLL
    for (uint i = 0; i < 4; ++i)
    {
        avgerColor += float4(samples[i].rgb, 1) * ComputeDepthSimilarity(samples[i].a, maxZ, thresholdInverse);
    }
    avgerColor.rgb /= avgerColor.a;
#else
    float4 avgerColor = 0.0f;
#endif

    outColor = float4(avgerColor.rgb, maxZ / Constant_Float16F_Scale);
}
