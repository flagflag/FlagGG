// for each light:
//   float4 position (w is padding)
//   float4 intensity + radius (xyz is intensity, w is radius)
#if PLATFORM_WINDOWS_DIRECTX
    StructuredBuffer<float4> clusterLightsBuffer : REGISTER(u, SAMPLER_LIGHTS_POINTLIGHTS);
#else
    cbuffer float4 clusterLightsBuffer[600u] : REGISTER(b, SAMPLER_LIGHTS_POINTLIGHTS);
#endif

struct PointLight
{
    float3 position;
    float range;
    float3 intensity;
    float radius;
};

struct AmbientLight
{
    float3 irradiance;
};

// primary source:
// https://seblagarde.files.wordpress.com/2015/07/course_notes_moving_frostbite_to_pbr_v32.pdf
// also really good:
// https://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf

float distanceAttenuation(float distance)
{
    // only for point lights

    // physics: inverse square falloff
    // to keep irradiance from reaching infinity at really close distances, stop at 1cm
    return 1.0 / max(distance * distance, 0.01 * 0.01);
}

float smoothAttenuation(float distance, float radius)
{
    // window function with smooth transition to 0
    // radius is arbitrary (and usually artist controlled)
    float nom = saturate(1.0 - pow(distance / radius, 4.0));
    return nom * nom * distanceAttenuation(distance);
}

uint GetPointLightCount()
{
    return pointLightCount;
}

PointLight GetPointLight(uint i)
{
    PointLight light;
    float4 positionRangeVec = clusterLightsBuffer[2 * int(i) + 0];
    light.position = positionRangeVec.xyz;
    light.range = positionRangeVec.w;
    float4 intensityRadiusVec = clusterLightsBuffer[2 * int(i) + 1];
    light.intensity = intensityRadiusVec.xyz;
    light.radius = intensityRadiusVec.w;
    return light;
}

#ifdef CLUSTER_SPOTLIGHT
struct SpotLight
{    
    float3 position;
    float range;
    float3 intensity;
    float cosOuterCone;
    float3 direction;
    float invCosConeDiff;
};

SpotLight GetSpotLight(uint i)
{
    SpotLight light;
    float4 v1 = clusterLightsBuffer[3 * int(i) + 0];
    float4 v2 = clusterLightsBuffer[3 * int(i) + 1];
    float4 v3 = clusterLightsBuffer[3 * int(i) + 2];
    light.position = v1.xyz;
    light.range = v1.w;
    light.intensity = v2.xyz;
    light.cosOuterCone = v2.w;
    light.direction = v3.xyz;
    light.invCosConeDiff = v3.w;
    return light;
}
#endif

#ifdef NONPUNCTUAL_LIGHTING

struct NonPunctualPointLight
{
    float3 position;
    float range;
    float3 intensity;
    float packRadius;
    float3 direction;
    float length;
};

NonPunctualPointLight GetNonPunctualPointLight(uint i)
{
    NonPunctualPointLight light;
    float4 positionRangeVec = clusterLightsBuffer[nonPunctualPointLightOffset + 3 * int(i) + 0];
    light.position = positionRangeVec.xyz;
    light.range = positionRangeVec.w;
    float4 intensityRadiusVec = clusterLightsBuffer[nonPunctualPointLightOffset + 3 * int(i) + 1];
    light.intensity = intensityRadiusVec.xyz;
    light.packRadius = intensityRadiusVec.w;
    float4 directionLengthVec = clusterLightsBuffer[nonPunctualPointLightOffset + 3 * int(i) + 2];
    light.direction = directionLengthVec.xyz;
    light.length = directionLengthVec.w;
    return light;
}

#endif
