#include "Shader/Cluster/ClusterUtil.hlsl"

// taken from Doom
// http://advances.realtimerendering.com/s2016/Siggraph2016_idTech6.pdf

#define CLUSTERS_X 16
#define CLUSTERS_Y 8
#define CLUSTERS_Z 24

// workgroup size of the culling compute shader
// D3D compute shaders only allow up to 1024 threads per workgroup
// GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS also only guarantees 1024
#define CLUSTERS_X_THREADS 16
#define CLUSTERS_Y_THREADS 8
#define CLUSTERS_Z_THREADS 1

#define MAX_LIGHTS_PER_CLUSTER 50

#ifdef WRITE_CLUSTERS
    #define CLUSTER_BUFFER(name, type, reg) RWStructuredBuffer<type> name : REGISTER(u, reg)
#else
    #define CLUSTER_BUFFER(name, type, reg) StructuredBuffer<type> name : REGISTER(t, reg)
#endif

// light indices belonging to clusters
CLUSTER_BUFFER(clusterLightIndicesBuffer, uint, SAMPLER_CLUSTERS_LIGHTINDICES);
// for each cluster: (start index in clusterLightIndicesBuffer, number of point lights, empty, empty)
CLUSTER_BUFFER(clusterLightGridBuffer, uint, SAMPLER_CLUSTERS_LIGHTGRID);

// these are only needed for building clusters and light culling, not in the fragment shader
#ifdef WRITE_CLUSTERS
// list of clusters (2 float4's each, min + max pos for AABB)
    CLUSTER_BUFFER(clustersBuffer, float4, SAMPLER_CLUSTERS_CLUSTERS);
    // atomic counter for building the light grid
    // must be reset to 0 every frame
    CLUSTER_BUFFER(globalIndex, uint, SAMPLER_CLUSTERS_ATOMICINDEX);
#endif

struct Cluster
{
    float3 minBounds;
    float3 maxBounds;
};

struct LightGrid
{
    uint offset;
    uint pointLights;
#ifdef NONPUNCTUAL_LIGHTING
    uint nonPunctualPointLightsOffset;
    uint nonPunctualPointLights;
#endif
    
};

#ifdef WRITE_CLUSTERS
Cluster GetCluster(uint index)
{
    Cluster cluster;
    cluster.minBounds = clustersBuffer[2 * int(index) + 0].xyz;
    cluster.maxBounds = clustersBuffer[2 * int(index) + 1].xyz;
    return cluster;
}
#endif

LightGrid GetLightGrid(uint cluster)
{
    uint4 gridvec = uint4(clusterLightGridBuffer[4 * cluster], clusterLightGridBuffer[4 * cluster + 1], clusterLightGridBuffer[4 * cluster + 2], clusterLightGridBuffer[4 * cluster + 3]);
    LightGrid grid;
    grid.offset = gridvec.x;
    grid.pointLights = gridvec.y;
#ifdef NONPUNCTUAL_LIGHTING
    grid.nonPunctualPointLightsOffset = gridvec.x + gridvec.y + gridvec.z;
    grid.nonPunctualPointLights = gridvec.w;
#endif
    return grid;
}

uint GetGridLightIndex(uint start, uint offset)
{
    return clusterLightIndicesBuffer[start + offset];
}

// cluster depth index from depth in screen coordinates (gl_FragCoord.z)
uint GetClusterZIndex(float screenDepth)
{
    // this can be calculated on the CPU and passed as a uniform
    // only leaving it here to keep most of the relevant code in the shaders for learning purposes
    // 总的数量（可以理解为far-near，比如far在10，near在1，scale就是9）
    float scale = float(CLUSTERS_Z) / log(farClip / nearClip);
    // near的位置
    float bias = -(float(CLUSTERS_Z) * log(nearClip) / log(farClip / nearClip));

    float eyeDepth = Screen2EyeDepth(screenDepth, nearClip, farClip);
    //float eyeDepth = Screen2Eye(float4(0, 0, screenDepth, 1)).z;
    // (如当前是5.5，near在1，far在10 ，则一共9个，index就是4)
    uint zIndex = uint(max(log(eyeDepth) * scale + bias, 0.0));
    return zIndex;
}

// cluster index from fragment position in window coordinates (gl_FragCoord)
uint GetClusterIndex(float4 fragCoord)
{
    uint zIndex = GetClusterZIndex(min(0.999999, fragCoord.z));
    uint3 indices = uint3(uint2(fragCoord.xy / clusterSizes), zIndex);
    uint cluster = (uint(CLUSTERS_X) * uint(CLUSTERS_Y)) * indices.z +
                   uint(CLUSTERS_X) * indices.y +
                   indices.x;
    return cluster;
}

float4 GetRealCoord(float4 fragCoord)
{
    float4 realCoord = float4(fragCoord.xy - clusterOrigin, fragCoord.zw);
    return realCoord;
}

// eyeDepth <=> sceneDepth ( LinearDepth )
uint GetDeferredClusterZIndex(float eyeDepth)
{
    float scale = float(CLUSTERS_Z) / log(farClip / nearClip);
    float bias = -(float(CLUSTERS_Z) * log(nearClip) / log(farClip / nearClip));
    uint zIndex = uint(max(log(eyeDepth) * scale + bias, 0.0));
    return zIndex;
}

uint GetDeferredClusterIndex(float2 fragCoordXY, float sceneDepth)
{
    uint zIndex = GetDeferredClusterZIndex(sceneDepth);
    uint3 indices = uint3(uint2((fragCoordXY - clusterOrigin) / clusterSizes), zIndex);
    uint cluster = (uint(CLUSTERS_X) * uint(CLUSTERS_Y)) * indices.z +
                   uint(CLUSTERS_X) * indices.y +
                   indices.x;
    return cluster;
}
