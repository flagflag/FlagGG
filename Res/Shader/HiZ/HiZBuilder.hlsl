#ifdef VERTEX
void VS(
    in float3 position : POSITION,
    out float4 outPosition : SV_POSITION,
    out float2 outTexcoord : TEXCOORD)
{
    outPosition = float4(position, 1.0);
    outTexcoord = outPosition.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);
}
#endif

#if PIXEL
// Hi-Z map 最远离相机的深度
Texture2D parentMipTexture : register(t0);
SamplerState parentMipTextureSampler : register(s0);

#if CLOSEST_HIZ
// Hi-Z map 最靠近相机的深度
Texture2D parentMipTexture2 : register(t1);
SamplerState parentMipTextureSampler2 : register(s1);
#endif

cbuffer HiZParam : register(b0)
{
    float2 invSize;
}

#if REVERSE_Z
    #define farthest min
    #define closest max
#else
    #define farthest max
    #define closest min
#endif

void PS(
    in float4 position : SV_POSITION,
    in float2 texcoord : TEXCOORD,
#if CLOSEST_HIZ
    out float4 outColor  : SV_Target0,
    out float4 outColor2 : SV_Target1
#else
    out float4 outColor : SV_TARGET
#endif
    )
{
#if COPY_DEPTH
    outColor = parentMipTexture.SampleLevel(parentMipTextureSampler, texcoord, 0).r;
    #if CLOSEST_HIZ
        outColor2 = outColor;
    #endif
#else
    // 偏移0.25，转换到上一级mip的位置
    float2 uv = texcoord + float2(-0.25f, -0.25f) * invSize;
    // 采样加速：Gather方式采样，采样四个像素
    float4 deviceZ = parentMipTexture.GatherRed(parentMipTextureSampler, uv, 0);
    // 取最远离相机的z作为当前mip的z
    float furthestDeviceZ = farthest(farthest(deviceZ.x, deviceZ.y), farthest(deviceZ.z, deviceZ.w));
    outColor = furthestDeviceZ;
    #if CLOSEST_HIZ
        // 采样加速：Gather方式采样，采样四个像素
        deviceZ = parentMipTexture2.GatherRed(parentMipTextureSampler2, uv, 0);
        // 取最靠近相机的z作为当前mip的z
        float closestDeviceZ = closest(closest(deviceZ.x, deviceZ.y), closest(deviceZ.z, deviceZ.w));
        outColor2 = closestDeviceZ;
    #endif
#endif
}
#endif
