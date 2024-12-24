#include "Shader/Cluster/ClusterUniform.hlsl"
#include "Shader/Cluster/Clusters.hlsl"

[numthreads(1, 1, 1)]
void CS(uint3 DTid : SV_DispatchThreadID)
{
    if(DTid.x == 0u)
    {
        // reset the atomic counter for the light grid generation
        // writable compute buffers can't be updated by CPU so do it here
        globalIndex[0] = 0u;
    }
}
