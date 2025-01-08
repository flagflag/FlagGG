
#define REGISTER(type, reg) register(type[reg])

// cluster size in screen coordinates (pixels)
#if !defined(PIXEL)
cbuffer ParamBuffer : register(b0)
#else
cbuffer ParamBuffer : register(b1)
#endif
{
    float2 clusterSizes;
    float2 clusterOrigin;
    float4 viewRect;
#if !defined(PIXEL)
    float nearClip;
    float farClip;
#endif
    float4x4 viewMatrix;
    float4x4 invProjMatrix;
    uint pointLightCount;
    uint spotLightCount;
    uint nonPunctualPointLightCount;
    uint pointLightOffset;
    uint spotLightOffset;
    uint nonPunctualPointLightOffset;
}
