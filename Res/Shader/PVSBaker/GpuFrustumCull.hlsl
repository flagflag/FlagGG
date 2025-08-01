//
// 视锥体剔除
//

#include "Shader/Platform.hlsl"
#include "Shader/PVSBaker/BoundingBox.hlsl"

#define NUM_FRUSTUM_PLANES 6

// Cpu做过Uniform优化，所以uniform的顺序布局不能随便乱改
cbuffer Params : register(b0)
{
    uint geometryCount;
    // 这个采样点个数包含6个面的情况
    uint sampleCount;
    // 采样开始的ID
    uint startSampleID;
};

struct PVSGpuCullUniform
{
    float4 normal[NUM_FRUSTUM_PLANES];
};
StructuredBuffer<PVSGpuCullUniform> gpuCullUniform : register(t0);
// AABB，把AABB单独拿出来增加缓存命中率
StructuredBuffer<BoundingBox> worldBoundingBoxBuffer : register(t1);
// 每个模型数据信息
struct GpuSceneGeometry
{
    uint startIndexLocation;
    uint indexCount;
    uint baseVertexLocation;
    uint startInstanceLocation;
};
StructuredBuffer<GpuSceneGeometry> sceneGeometryBuffer : register(t2);
// Draw indirect
// https://learn.microsoft.com/en-us/windows/win32/api/d3d11/ns-d3d11-d3d11_draw_indexed_instanced_indirect_args
struct DrawIndexedIndirectArgs
{
    uint indexCountPerInstance;
    uint instanceCount;
    uint startIndexLocation;
    uint baseVertexLocation;
    uint startInstanceLocation;
};
RWStructuredBuffer<DrawIndexedIndirectArgs> drawIndirectArgsBuffer : register(u3);
// Draw count
RWStructuredBuffer<uint> globalIndex : register(u4);

// 检测AABB是否在视锥体内
bool IsInside(PVSGpuCullUniform params, BoundingBox aabb)
{
    float3 center = GetBoundingBoxCenter(aabb);
    float3 edge = center - aabb.minPos.xyz;

    // UNROLL_N(NUM_FRUSTUM_PLANES)
    for (int i = 0; i < NUM_FRUSTUM_PLANES; ++i)
    {
        // dot(normal, center) + d
        float dist = dot(params.normal[i].xyz, center) + params.normal[i].w;
        float3 absNormal = abs(params.normal[i].xyz);
        float absDist = dot(absNormal, edge);

        // BRANCH
        if (dist < -absDist)
            return false;
    }

    return true;
}

#if INIT_VAR
[numthreads(THREAD_COUNT, 1, 1)]
void CS(uint3 DTid : SV_DispatchThreadID)
{
    uint modelID = DTid.x;
    if (modelID >= geometryCount)
        return;

    // init draw indirect data
    DrawIndexedIndirectArgs args;
    args.indexCountPerInstance = 0;
    args.instanceCount = 0;
    args.startIndexLocation = 0;
    args.baseVertexLocation = 0;
    args.startInstanceLocation = 0;
    for (uint i = 0; i < sampleCount; ++i)
    {
        // init count
        globalIndex[i] = i * geometryCount;

        uint drawIndex = geometryCount * i + modelID;
        drawIndirectArgsBuffer[drawIndex] = args;
    }
}
#else
[numthreads(THREAD_COUNT, 1, 1)]
void CS(uint3 DTid : SV_DispatchThreadID)
{
    uint modelID = DTid.x;
    if (modelID >= geometryCount)
        return;
    
    // read aabb
    BoundingBox aabb = worldBoundingBoxBuffer[modelID];
    // read geom info
    GpuSceneGeometry geom = sceneGeometryBuffer[modelID];

    for (uint i = 0; i < sampleCount; ++i)
    {
        uint sampleID = startSampleID + i;
        PVSGpuCullUniform params = gpuCullUniform[sampleID];

        if (IsInside(params, aabb))
        {
            // atomic add
            uint drawIndex = 0;
            InterlockedAdd(globalIndex[i], 1, drawIndex);

            // write draw indirect data
            DrawIndexedIndirectArgs args;
            args.indexCountPerInstance = geom.indexCount;
            args.instanceCount         = 1;
            args.startIndexLocation    = geom.startIndexLocation;
            args.baseVertexLocation    = geom.baseVertexLocation;
            args.startInstanceLocation = geom.startInstanceLocation;
            drawIndirectArgsBuffer[drawIndex] = args;
        }
    }
}
#endif
