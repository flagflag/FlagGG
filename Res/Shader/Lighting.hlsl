
float3 GetLightColor()
{
    return float3(1.0, 1.0, 1.0);
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
