#include "Shader/Cluster/ClusterUniform.hlsl"
#include "Shader/Cluster/Clusters.hlsl"

#define WorkGroupSize uint3(CLUSTERS_X_THREADS, CLUSTERS_Y_THREADS, CLUSTERS_Z_THREADS)

[numthreads(CLUSTERS_X_THREADS, CLUSTERS_Y_THREADS, CLUSTERS_Z_THREADS)]
void CS(uint3 DTid : SV_DispatchThreadID)
{
    uint clusterIndex = DTid.z * WorkGroupSize.x * WorkGroupSize.y +
                        DTid.y * WorkGroupSize.x +
                        DTid.x;

    float4 minScreen = float4(float2(DTid.xy)                  * clusterSizes.xy, 1.0, 1.0);
    float4 maxScreen = float4((float2(DTid.xy) + float2(1, 1)) * clusterSizes.xy, 1.0, 1.0);

    float3 minEye = Screen2Eye(minScreen).xyz;
    float3 maxEye = Screen2Eye(maxScreen).xyz;

#ifdef ORTHOGRAPHIC
    float clusterNear = lerp(nearClip, farClip, float(DTid.z) / float(CLUSTERS_Z));
    float clusterFar  = lerp(nearClip, farClip, (float(DTid.z) + 1.0) / float(CLUSTERS_Z));

    float3 minNear = float3(minEye.xy, clusterNear);
    float3 minFar  = float3(minEye.xy, clusterFar);
    float3 maxNear = float3(maxEye.xy, clusterNear);
    float3 maxFar  = float3(maxEye.xy, clusterFar);
#else    
    float clusterNear = nearClip * pow(farClip / nearClip,  float(DTid.z)      / float(CLUSTERS_Z));
    float clusterFar  = nearClip * pow(farClip / nearClip, (float(DTid.z) + 1.0) / float(CLUSTERS_Z));

    float3 minNear = minEye * clusterNear / minEye.z;
    float3 minFar  = minEye * clusterFar  / minEye.z;
    float3 maxNear = maxEye * clusterNear / maxEye.z;
    float3 maxFar  = maxEye * clusterFar  / maxEye.z;
#endif

    float3 minBounds = min(min(minNear, minFar), min(maxNear, maxFar));
    float3 maxBounds = max(max(minNear, minFar), max(maxNear, maxFar));

    clustersBuffer[2 * int(clusterIndex) + 0] = float4(minBounds, 1.0);
    clustersBuffer[2 * int(clusterIndex) + 1] = float4(maxBounds, 1.0);
}
