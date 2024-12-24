#include "Shader/Cluster/ClusterUniform.hlsl"
#include "Shader/Cluster/ClusterLights.hlsl"
#include "Shader/Cluster/Clusters.hlsl"

// compute shader to cull lights against cluster bounds
// builds a light grid that holds indices of lights for each cluster
// largely inspired by http://www.aortiz.me/2018/12/21/CG.html

// point lights only for now
bool LightIntersectsCluster(PointLight light, Cluster cluster);
#ifdef CLUSTER_SPOTLIGHT
bool LightIntersectsCluster(SpotLight light, Cluster cluster);
#endif

#define WorkGroupSize uint3(CLUSTERS_X_THREADS, CLUSTERS_Y_THREADS, CLUSTERS_Z_THREADS)
#define GROUP_SIZE uint(CLUSTERS_X_THREADS * CLUSTERS_Y_THREADS * CLUSTERS_Z_THREADS)

// light cache for the current workgroup
// group shared memory has lower latency than global memory

// there's no guarantee on the available shared memory
// as a guideline the minimum value of GL_MAX_COMPUTE_SHARED_MEMORY_SIZE is 32KB
// with a workgroup size of 16*8*4 this is 64 bytes per light
// however, using all available memory would limit the compute shader invocation to only 1 workgroup

groupshared float4 sharedLights[GROUP_SIZE * 3];

void EncodeSharedPointLight(int idx, PointLight light)
{
    sharedLights[idx * 2] = float4(light.position, light.range);
    sharedLights[idx * 2 + 1] = float4(light.intensity, light.radius);
}

PointLight GetSharedPointLight(int idx)
{
    PointLight light;
    float4 positionRangeVec = sharedLights[2 * idx + 0];
    light.position = positionRangeVec.xyz;
    light.range = positionRangeVec.w;
    float4 intensityRadiusVec = sharedLights[2 * idx + 1];
    light.intensity = intensityRadiusVec.xyz;
    light.radius = intensityRadiusVec.w;
    return light;
}

#ifdef CLUSTER_SPOTLIGHT
#define FalloffPrecision 0.1
float ApproximateFalloffRange(float range, float3 intensity)
{
    float invIntensity = 1.0 / (max(max(intensity.r, intensity.g), intensity.b) + 1e-4);
    float x = FalloffPrecision * range * range * invIntensity;
    float y = 9.316587627e-1 + x * (-6.0524587854e-1 + x *
        (2.3808071668e-1 + x * 
        (-5.1486463156778e-2 + x * 
        (6.26786382348559e-3 + x * 
        (-4.40050876275e-4 + x * 
        (1.759784386156e-5 + x * 
        (3.173039838e-9 * x - 3.70544966259e-7)))))));
    return clamp(sqrt(y), 0.0, 1.0) * range;
}

void EncodeSharedSpotLight(int idx, SpotLight light)
{
    //直接把shared内存中的range就编码好，这样每条thread只算一次，cluster里我们不需要算衰减，所以直接替换掉range就可以了
    sharedLights[idx * 3] = float4(light.position, ApproximateFalloffRange(light.range, light.intensity));
    sharedLights[idx * 3 + 1] = float4(light.intensity, light.cosOuterCone);
    sharedLights[idx * 3 + 2] = float4(light.direction, light.invCosConeDiff);
} 

SpotLight GetSharedSpotLight(int idx)
{
    SpotLight light;
    float4 v1 = sharedLights[3 * idx + 0];
    float4 v2 = sharedLights[3 * idx + 1];
    float4 v3 = sharedLights[3 * idx + 2];
    light.position = v1.xyz;
    light.intensity = v2.xyz;
    light.range = v1.w;
    light.cosOuterCone = v2.w;
    light.direction = v3.xyz;
    light.invCosConeDiff = v3.w;
    return light;
}
#endif


#ifdef NONPUNCTUAL_LIGHTING
bool NonPunctualPointLightIntersectsCluster(NonPunctualPointLight light, Cluster cluster)
{ 
    float3 closest = max(cluster.minBounds, min(light.position, cluster.maxBounds));
    float3 dist = closest - light.position;
    return dot(dist, dist) <= (light.range * light.range);
}

void EncodeSharedNonPunctualPointLight(int idx, NonPunctualPointLight light)
{
    sharedLights[idx * 3] = float4(light.position, light.range);
    sharedLights[idx * 3 + 1] = float4(light.intensity, light.packRadius);
    sharedLights[idx * 3 + 2] = float4(light.direction, light.length);
}

NonPunctualPointLight GetSharedNonPunctualPointLight(int idx)
{
    NonPunctualPointLight light;
    float4 positionRangeVec = sharedLights[3 * idx + 0];
    light.position = positionRangeVec.xyz;
    light.range = positionRangeVec.w;
    float4 intensityRadiusVec = sharedLights[3 * idx + 1];
    light.intensity = intensityRadiusVec.xyz;
    light.packRadius = intensityRadiusVec.w;
    float4 directionLengthVec = sharedLights[3 * idx + 2];
    light.direction = directionLengthVec.xyz;
    light.length = directionLengthVec.w;
    return light;
}
#endif

// each thread handles one cluster
[numthreads(CLUSTERS_X_THREADS, CLUSTERS_Y_THREADS, CLUSTERS_Z_THREADS)]
void CS(uint3 DTid : SV_DispatchThreadID, int LocalInvocationIndex : SV_GroupIndex)
{
    // local thread variables
    // hold the result of light culling for this cluster
    uint visibleLights[MAX_LIGHTS_PER_CLUSTER];
    uint visibleCount = 0u;

    // the way we calculate the index doesn't really matter here since we write to the same index in the light grid as we read from the cluster buffer
    uint clusterIndex = DTid.z * WorkGroupSize.x * WorkGroupSize.y +
                        DTid.y * WorkGroupSize.x +
                        DTid.x;

    Cluster cluster = GetCluster(clusterIndex);
    
    // we have a cache of GROUP_SIZE lights
    // have to run this loop several times if we have more than GROUP_SIZE lights
    uint lightCount = GetPointLightCount();
    uint lightOffset = 0u;
    while(lightOffset < lightCount)
    {
        // read GROUP_SIZE lights into shared memory
        // each thread copies one light
        uint batchSize = min(GROUP_SIZE, lightCount - lightOffset);

        // 每次多线程取batchSize个灯，如果线程数量比batchsize多，只有前面的线程工作
        if(uint(LocalInvocationIndex) < batchSize)
        {
            uint lightIndex = lightOffset + LocalInvocationIndex;
#ifdef CLUSTER_SPOTLIGHT
            SpotLight light = GetSpotLight(lightIndex);
            light.position = mul(float4(light.position, 1.0), viewMatrix).xyz;
            light.direction = normalize(mul(float4(light.direction, 0.0), viewMatrix).xyz);
            EncodeSharedSpotLight(LocalInvocationIndex, light);
#else
            PointLight light = GetPointLight(lightIndex);
            // transform to view space (expected by pointLightAffectsCluster)
            // do it here once rather than for each cluster later
            // 注意行列矩阵，sample和我们应该是反的
            light.position = mul(float4(light.position, 1.0), viewMatrix).xyz;
            EncodeSharedPointLight(LocalInvocationIndex, light);
#endif
        }

        // wait for all threads to finish copying
        GroupMemoryBarrierWithGroupSync();

        // each thread is one cluster and checks against all lights in the cache
        // 每个cluster算下取到的batchsize个灯里，有没有和该cluster有相交关系的，这里每个线程都在工作
        for(uint i = 0u; i < batchSize; i++)
        {
            Cluster cluster = GetCluster(clusterIndex);
#ifdef CLUSTER_SPOTLIGHT
            SpotLight light = GetSharedSpotLight(i);
#else
            PointLight light = GetSharedPointLight(i);
#endif
            if(visibleCount < uint(MAX_LIGHTS_PER_CLUSTER) && LightIntersectsCluster(light, cluster))
            {
                visibleLights[visibleCount] = lightOffset + i;
                visibleCount++;
            }
        }

        lightOffset += batchSize;
    }

    // wait for all threads to finish checking lights
    GroupMemoryBarrierWithGroupSync();

    // get a unique index into the light index list where we can write this cluster's lights
    uint offset = 0u;
#ifdef NONPUNCTUAL_LIGHTING
    //处理编辑器的非精确点光
    uint visibleNonPunctualLights[MAX_LIGHTS_PER_CLUSTER];
    uint visibleNonPunctualLightCount = 0u;
    lightCount = nonPunctualPointLightCount;
    lightOffset = 0u;
    while(lightOffset < lightCount)
    {
        uint batchSize = min(GROUP_SIZE, lightCount - lightOffset);

        // 每次多线程取batchSize个灯，如果线程数量比batchsize多，只有前面的线程工作
        if(uint(LocalInvocationIndex) < batchSize)
        {
            uint lightIndex = lightOffset + LocalInvocationIndex;
            NonPunctualPointLight light = GetNonPunctualPointLight(lightIndex);
            // transform to view space (expected by pointLightAffectsCluster)
            // do it here once rather than for each cluster later
            // 注意行列矩阵，sample和我们应该是反的
            light.position = mul(float4(light.position, 1.0), viewMatrix).xyz;
            EncodeSharedNonPunctualPointLight(LocalInvocationIndex, light);
        }

        // wait for all threads to finish copying
        GroupMemoryBarrierWithGroupSync();

        // each thread is one cluster and checks against all lights in the cache
        // 每个cluster算下取到的batchsize个灯里，有没有和该cluster有相交关系的，这里每个线程都在工作
        for(uint i = 0u; i < batchSize; i++)
        {
            Cluster cluster = GetCluster(clusterIndex);
            NonPunctualPointLight light = GetSharedNonPunctualPointLight(i);
            if(visibleNonPunctualLightCount < uint(MAX_LIGHTS_PER_CLUSTER) && NonPunctualPointLightIntersectsCluster(light, cluster))
            {
                visibleNonPunctualLights[visibleNonPunctualLightCount] = lightOffset + i;
                visibleNonPunctualLightCount++;
            }
        }

        lightOffset += batchSize;
    }

    // wait for all threads to finish checking lights
    GroupMemoryBarrierWithGroupSync();
    InterlockedAdd(globalIndex[0], visibleCount + visibleNonPunctualLightCount, offset);
#else//NONPUNCTUAL_LIGHTING
    // 其实就是：globalIndex[0] = visibleCount + offset; offset = globalIndex[0]
    // offset是局部变量，相当于每个线程有一个基于当前globalIndex的offset
    InterlockedAdd(globalIndex[0], visibleCount, offset);
#endif//NONPUNCTUAL_LIGHTING
    // copy indices of lights
    [allow_uav_condition]
    for(uint i = 0u; i < visibleCount; i++)
    {
        clusterLightIndicesBuffer[offset + i] = visibleLights[i];
    }

#ifdef NONPUNCTUAL_LIGHTING
    for (uint i = 0u; i < visibleNonPunctualLightCount; i++)
    {
        clusterLightIndicesBuffer[offset + visibleCount + i] = visibleNonPunctualLights[i];
    }
    //z通道预留给SpotLight
    clusterLightGridBuffer[4 * clusterIndex] = offset;
    clusterLightGridBuffer[4 * clusterIndex + 1] = visibleCount;
    clusterLightGridBuffer[4 * clusterIndex + 2] = 0;
    clusterLightGridBuffer[4 * clusterIndex + 3] = visibleNonPunctualLightCount;
#else//NONPUNCTUAL_LIGHTING
    // write light grid for this cluster
    clusterLightGridBuffer[4 * clusterIndex] = offset;
    clusterLightGridBuffer[4 * clusterIndex + 1] = visibleCount;
    clusterLightGridBuffer[4 * clusterIndex + 2] = 0;
    clusterLightGridBuffer[4 * clusterIndex + 3] = 0;
#endif//NONPUNCTUAL_LIGHTING
}

// check if light radius extends into the cluster
bool LightIntersectsCluster(PointLight light, Cluster cluster)
{
    // NOTE: expects light.position to be in view space like the cluster bounds
    // global light list has world space coordinates, but we transform the
    // coordinates in the shared array of lights after copying

    // get closest point to sphere center
 
    float3 closest = max(cluster.minBounds, min(light.position, cluster.maxBounds));
    //float3 closest;
    //closest.x= max(cluster.minBounds.x, min(light.position.x, cluster.maxBounds.x));
    //closest.y= max(cluster.minBounds.y, min(light.position.y, cluster.maxBounds.y));
    //closest.z= max(cluster.minBounds.z, min(light.position.z, cluster.maxBounds.z));
    // check if point is inside the sphere
    float3 dist = closest - light.position;
    return dot(dist, dist) <= (light.radius * light.radius);
}

#ifdef CLUSTER_SPOTLIGHT
bool LightIntersectsCluster(SpotLight light, Cluster cluster)
{
    float3 closest = max(cluster.minBounds, min(light.position, cluster.maxBounds));
    // check if point is inside the sphere
    float3 dist = closest - light.position;
    if (dot(dist, dist) > (light.range * light.range))
        return false;
    //点光的这个数值为-1，只有锥光需要继续算下去
    if (light.cosOuterCone < 0.0)
        return true;
    float3 center = 0.5 * (cluster.minBounds + cluster.maxBounds);
    dist = center - light.position;
    float3 M = normalize(cross(cross(light.direction, dist), light.direction));
    //这里可以不normalize,省个操作，因为后面是两个点乘比较等于是等比放缩了而已
    float3 N = M - light.direction * sqrt(1.0 - light.cosOuterCone * light.cosOuterCone) / light.cosOuterCone;
    float3 extents = 0.5 * (cluster.maxBounds - cluster.minBounds);
    return dot(dist, N) <= dot(extents, abs(N));
}
#endif //CLUSTER_SPOTLIGHT
