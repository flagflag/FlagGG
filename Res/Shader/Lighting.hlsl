
float3 DirectionalLight_GetLightColor()
{
    return lightColor.rgb;
}

float DirectionalLight_GetAttenAndLightDir(float3 worldPosition, out float3 lightDirection)
{
    lightDirection = lightDir;
    return 1.0;
}

// 均匀发散的物理光源
float GetLightRangeMask(float radius, float distSqr)
{
    float w = 1.0 / radius;
    float lightRadiusMask = Square(saturate(1.0 - Square(distSqr * w * w)));
    return lightRadiusMask;
}

// 均匀发散的物理光源
float GetAttenAndLightDir(float3 worldPosition, float3 lightPosition, float radius, out float3 lightDirection)
{
    float3 toLight = lightPosition - worldPosition;
    float distSqr = dot(toLight, toLight);
    // <==> normalize(toLight)
    lightDirection = toLight / (sqrt(distSqr) + 1e-7);
    float falloff = 1.0 / (distSqr + 1.0);
    return falloff * GetLightRangeMask(radius, distSqr);
}

// Spot形状的物理光源衰减额外系数
float GetLightDirectionFalloff(float3 L, float3 direction, float cosOuterCone, float invCosDiff)
{
    return Square(clamp((dot(direction, -L) - cosOuterCone) * invCosDiff, 0.0, 1.0));
}
