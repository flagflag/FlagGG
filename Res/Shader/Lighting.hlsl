
float3 GetLightColor()
{
    return lightColor.rgb;
}

float GetAttenAndLightDir(float3 worldPosition, out float3 lightDirection)
{
#if defined(DIRLIGHT)
    lightDirection = lightDir;
    return 1.0;
#else
    // TODO
#endif
}
