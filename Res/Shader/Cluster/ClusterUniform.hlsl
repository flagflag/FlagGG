
#define REGISTER(type, reg) type ## reg

// cluster size in screen coordinates (pixels)
cbuffer ParamBuffer : register(b0)
{
    float2 clusterSizes;
    float2 clusterOrigin;
    float nearClip;
    float farClip;
    float4x4 viewMatrix;
    float4x4 invProjMatrix;
    uint pointLightCount;
    uint spotLightCount;
    uint nonPunctualPointLightCount;
    uint pointLightOffset;
    uint spotLightOffset;
    uint nonPunctualPointLightOffset;
}

#define viewRect float4(clusterOrigin, clusterSizes)
