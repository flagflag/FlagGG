#ifdef VERTEX
cbuffer HiZParam : register(b0)
{
    float4 screenToUV;
}

void VS(
    in float3 position : POSITION,
    out float4 outPosition : SV_POSITION,
    out float2 outTexcoord : TEXCOORD)
{
    outPosition = float4(position, 1.0);
    outTexcoord = outPosition.xy * screenToUV.xy + screenToUV.zw;
}
#endif

#if PIXEL
Texture2D<float> sourceTexture : register(t0);
SamplerState sourceTextureSampler : register(s0);

cbuffer HiZParam : register(b0)
{
    float2 invSize;
}

#if REVERSE_Z
    #define farthest min
#else
    #define farthest max
#endif

void PS(
    in float4 position : SV_POSITION,
    in float2 texcoord : TEXCOORD,
    out float4 outColor : SV_TARGET)
{
#if COPY_DEPTH
    outColor = sourceTexture.SampleLevel(sourceTextureSampler, texcoord, 0).r;
#else
    // 偏移0.25，转换到上一级mip的位置
    float2 uv = texcoord + float2(-0.25f, -0.25f) * invSize;
    // 采样加速：Gather方式采样，采样四个像素
    float4 deviceZ = sourceTexture.GatherRed(sourceTextureSampler, uv, 0);
    // 保守估计，取最远离相机的z作为当前mip的z
    float furthestDeviceZ = farthest(farthest(deviceZ.x, deviceZ.y), farthest(deviceZ.z, deviceZ.w));
    outColor = furthestDeviceZ;
#endif
}
#endif
