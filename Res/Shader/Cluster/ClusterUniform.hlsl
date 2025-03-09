
#define REGISTER(type, reg) register(type[reg])

#if !defined(PIXEL)
cbuffer ClusterParam : register(b0)
#else
cbuffer ClusterParam : register(b1)
#endif
{
    // cluster size in screen coordinates (pixels)
    float2 clusterSizes;
    float2 clusterOrigin;
    float4 viewRect;
#if !defined(PIXEL)
    float nearClip;
    float farClip;
    float4x3 viewMatrix;
#endif
    float4x4 invProjMatrix;
    uint pointLightCount;
    uint spotLightCount;
    uint nonPunctualPointLightCount;
    uint pointLightOffset;
    uint spotLightOffset;
    uint nonPunctualPointLightOffset;
}
