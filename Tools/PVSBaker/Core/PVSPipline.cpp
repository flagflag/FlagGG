#include "PVSPipline.h"

#include <Graphics/ShaderHelper.h>
#include <Graphics/VertexBuffer.h>
#include <Graphics/IndexBuffer.h>
#include <Graphics/Texture2D.h>
#include <Graphics/Texture2DArray.h>
#include <Graphics/ShaderParameter.h>
#include <Scene/Camera.h>
#include <GfxDevice/GfxDevice.h>
#include <GfxDevice/GfxRenderSurface.h>
#include <Container/HashMap.h>
#include <Core/Profiler.h>
#include <FileSystem/FileManager.h>
#include <FileSystem/FileSystemArchive/DefaultFileSystemArchive.h>
#include <Utility/SystemHelper.h>
#include <Memory/Memory.h>

#if _DEBUG
#define PROFILE_DETAIL PROFILE_AUTO
#else
#define PROFILE_DETAIL(...)
#endif

static constexpr UInt32 RT_SIZE = 1024;
static constexpr UInt32 READBACK_CELL_COUNT_EACH_TIME = 10000;
static constexpr UInt32 MAX_VISIBILITY_ID_COUNT_PER_CELL = 1000;

static constexpr UInt32 THREAD_COUNT = 1024;

static constexpr UInt32 THREAD_X_COUNT = 32;
static constexpr UInt32 THREAD_Y_COUNT = 32;

static constexpr UInt32 MAX_FACE_COUNT = 6;

// 用Gpu预处理，拷贝uniform也在Gpu侧做
// RTX 4080Ti
// 原先1w个格子，耗时6500ms
// 现在1w个格子，耗时2500ms
#define PrepareCellParams_GPU 1
// RT使用Texture2DArray
// 测试下来，发现图集RT和使用Array效率没有变化，说明切RT开销和切Viewport开销差不多？
// 按照上诉结论，用Array肯定更优。因为合并可见id时，图集RT有冗余空间需要load
#define RT_TEXTURE2D_ARRAY 1

// https://learn.microsoft.com/en-us/windows/win32/api/d3d11/ns-d3d11-d3d11_draw_indexed_instanced_indirect_args
struct DrawIndexedIndirectArgs
{
    UInt32 indexCountPerInstance;
    UInt32 instanceCount;
    UInt32 startIndexLocation;
    UInt32 baseVertexLocation;
    UInt32 startInstanceLocation;
};

PVSBakerPipline::PVSBakerPipline(const IntVector3& sampleCount3D, GfxRenderSurface* renderSurfaceDebugView, UInt32 face)
    : sampleCount3D_(sampleCount3D)
    , gpuSceneGeometryCount_(0)
    , gpuSceneModelIDRange_(0)
    , debugFace_(face)
    , renderSurfaceDebugView_(renderSurfaceDebugView)
    , cullMode_(CULL_BACK)
    , depthWrite_(true)
    , cellGpuCullTime_(0)
    , cellDrawIndirectTime_(0)
    , cellBuildVisibilityBufferTime_(0)
    , mergeVisibleObjectsTime_(0)
    , saveVisibleObjectsTime_(0)
{
    sampleCount_ = sampleCount3D.x_ * sampleCount3D.y_ * sampleCount3D.z_;
    sampleFaceCount_ = MAX_FACE_COUNT;
    rtSize_ = IntVector2(RT_SIZE, RT_SIZE);
    if (sampleCount_ == 0)
    {
        sampleCount_ = 1;
        sampleFaceCount_ = 1;
    }

    if (GetSubsystem<AssetFileManager>()->GetNumArchive() == 0)
    {
        GetSubsystem<AssetFileManager>()->AddArchive(new DefaultFileSystemArchive(GetLocalFileSystem(), GetSubsystem<Context>()->GetResourceDir()));
    }

    INIT_SHADER_VARIATION(gpuPrepareCellParamsCS_, "Shader/PVSBaker/PrepareCellParams.hlsl", CS, 
        Vector<String>({ "THREAD_X_COUNT=16", "THREAD_Y_COUNT=4", "THREAD_Z_COUNT=16" }));
    INIT_SHADER_VARIATION(gpuPrepareCellParams2CS_, "Shader/PVSBaker/PrepareCellParams.hlsl", CS, 
        Vector<String>({ "CELL_BUFFER=1", ToString("THREAD_X_COUNT=%u", THREAD_COUNT), "THREAD_Y_COUNT=1", "THREAD_Z_COUNT=1" }));

    INIT_SHADER_VARIATION(gpuFrustumCullInitCS_, "Shader/PVSBaker/GpuFrustumCull.hlsl", CS, 
        Vector<String>({ "INIT_VAR=1", ToString("THREAD_COUNT=%u", THREAD_COUNT) }));
    INIT_SHADER_VARIATION(gpuFrustumCullCS_, "Shader/PVSBaker/GpuFrustumCull.hlsl", CS, 
        Vector<String>({ ToString("THREAD_COUNT=%u", THREAD_COUNT) }));

    if (renderSurfaceDebugView_)
    {
        INIT_SHADER_VARIATION(gpuDrawIndirectVS_, "Shader/PVSBaker/Model.hlsl", VS, { "DEBUG_VIEW=1" });
        INIT_SHADER_VARIATION(gpuDrawIndirectPS_, "Shader/PVSBaker/Model.hlsl", PS, { "DEBUG_VIEW=1" });
    }
    else
    {
        INIT_SHADER_VARIATION(gpuDrawIndirectVS_, "Shader/PVSBaker/Model.hlsl", VS, {});
        INIT_SHADER_VARIATION(gpuDrawIndirectPS_, "Shader/PVSBaker/Model.hlsl", PS, {});
    }

    INIT_SHADER_VARIATION(gpuMergeVisibleObjectsInitCS_, "Shader/PVSBaker/MergeVisibleObjectsPerCell.hlsl", CS, 
        Vector<String>({ "INIT_VAR=1", ToString("THREAD_COUNT=%u", THREAD_COUNT) }));
    INIT_SHADER_VARIATION(gpuMergeVisibleObjectsCS_, "Shader/PVSBaker/MergeVisibleObjectsPerCell.hlsl", CS, 
        Vector<String>({ ToString("THREAD_X_COUNT=%u", THREAD_X_COUNT), ToString("THREAD_Y_COUNT=%u", THREAD_Y_COUNT) }));

    INIT_SHADER_VARIATION(gpuSaveVisibleObjectsInitCS_, "Shader/PVSBaker/SaveVisibleObjectsPerCell.hlsl", CS, { "INIT_VAR=1" });
    INIT_SHADER_VARIATION(gpuSaveVisibleObjectsCS_, "Shader/PVSBaker/SaveVisibleObjectsPerCell.hlsl", CS, {});
}

PVSBakerPipline::~PVSBakerPipline()
{

}

void PVSBakerPipline::BuildGpuScene(const Vector<SceneInstance>& instances, Int32 invalidInstanceCount)
{
    gpuSceneGeometryCount_ = invalidInstanceCount == -1 ? instances.Size() : Min<UInt32>(invalidInstanceCount, instances.Size());
    gpuSceneModelIDRange_ = 0;
    for (auto& inst : instances)
    {
        gpuSceneModelIDRange_ = Max(gpuSceneModelIDRange_, inst.modelID_ + 1);
    }
    BuildGpuScene_AABB(instances);
    BuildGpuScene_Geometry(instances);
    BuildGpuScene_RenderTarget(instances);
    BuildGpuScene_DrawIndirectBuffer(instances);
    BuildGpuScene_CellDataBuffer(instances);
    BuildGpuScene_Uniform(instances);
}

void PVSBakerPipline::BuildGpuScene_AABB(const Vector<SceneInstance>& instances)
{
    PODVector<BoundingBox> aabbBuffer;
    aabbBuffer.Resize(instances.Size());
    for (int i = 0; i < instances.Size(); ++i)
    {
        aabbBuffer[i] = instances[i].geometry_->aabb_.Transformed(instances[i].worldTransform_);
    }
    gpuBoundingBoxBuffer_ = GfxDevice::GetDevice()->CreateBuffer();
    gpuBoundingBoxBuffer_->SetStride(sizeof(BoundingBox));
    gpuBoundingBoxBuffer_->SetSize(sizeof(BoundingBox) * aabbBuffer.Size());
    gpuBoundingBoxBuffer_->SetBind(BUFFER_BIND_COMPUTE_READ | BUFFER_BIND_COMPUTE_WRITE);
    gpuBoundingBoxBuffer_->SetAccess(BUFFER_ACCESS_NONE);
    gpuBoundingBoxBuffer_->SetUsage(BUFFER_USAGE_DYNAMIC);
    gpuBoundingBoxBuffer_->Apply(aabbBuffer.Buffer());
    gpuBoundingBoxBuffer_->SetGpuTag("BoundingBoxBuffer");
}

void PVSBakerPipline::BuildGpuScene_Geometry(const Vector<SceneInstance>& instances)
{
    // 模型vs、ib区间
    struct SceneGeometryRange
    {
        UInt32 startVertexLocation_;
        UInt32 startIndexLocation_;
    };

    // Instance data
    struct InstanceData
    {
        Vector4 instanceData0_;
        Vector4 instanceData1_;
        Vector4 instanceData2_;
        UInt32 instanceData3_;
        Color instanceData4_;
    };

    HashMap<SharedPtr<SceneGeometry>, SceneGeometryRange> sceneGeomMap;
    Vector<InstanceData> instanceData;

    UInt32 vertexCount = 0;
    UInt32 indexCount = 0;

    gpuSceneGeometryData_.Clear();
    gpuSceneGeometryData_.Reserve(instances.Size());
    instanceData.Reserve(instances.Size());

    for (auto& inst : instances)
    {
        auto it = sceneGeomMap.Find(inst.geometry_);
        if (it == sceneGeomMap.End())
        {
            it = sceneGeomMap.Insert(MakePair(inst.geometry_, SceneGeometryRange{ vertexCount, indexCount }));
            vertexCount += inst.geometry_->verts_.Size();
            indexCount += inst.geometry_->indices_.Size();
        }

        gpuSceneGeometryData_.Push({
            it->second_.startIndexLocation_,
            static_cast<UInt32>(it->first_->indices_.Size()),
            it->second_.startVertexLocation_,
            static_cast<UInt32>(instanceData.Size()),
        });
        InstanceData instData;
        instData.instanceData0_ = Vector4(inst.worldTransform_.m00_, inst.worldTransform_.m01_, inst.worldTransform_.m02_, inst.worldTransform_.m03_);
        instData.instanceData1_ = Vector4(inst.worldTransform_.m10_, inst.worldTransform_.m11_, inst.worldTransform_.m12_, inst.worldTransform_.m13_);
        instData.instanceData2_ = Vector4(inst.worldTransform_.m20_, inst.worldTransform_.m21_, inst.worldTransform_.m22_, inst.worldTransform_.m23_);
        instData.instanceData3_ = inst.modelID_;
        instData.instanceData4_ = inst.color_;
        instanceData.Push(instData);
    }

    gpuSceneGeometryBuffer_ = GfxDevice::GetDevice()->CreateBuffer();
    gpuSceneGeometryBuffer_->SetStride(sizeof(GpuSceneGeometry));
    gpuSceneGeometryBuffer_->SetSize(sizeof(GpuSceneGeometry) * gpuSceneGeometryData_.Size());
    gpuSceneGeometryBuffer_->SetBind(BUFFER_BIND_COMPUTE_READ);
    gpuSceneGeometryBuffer_->SetAccess(BUFFER_ACCESS_NONE);
    gpuSceneGeometryBuffer_->SetUsage(BUFFER_USAGE_DYNAMIC);
    gpuSceneGeometryBuffer_->Apply(gpuSceneGeometryData_.Buffer());
    gpuSceneGeometryBuffer_->SetGpuTag("GpuSceneGeometryBuffer");

    Vector<Vector3> verts;
    verts.Resize(vertexCount);

    Vector<UInt32> indices;
    indices.Resize(indexCount);

    for (auto& it : sceneGeomMap)
    {
        const auto geoRange = it.second_;
        auto geo = it.first_;
        memcpy(&verts[geoRange.startVertexLocation_], geo->verts_.Buffer(), geo->verts_.Size() * sizeof(Vector3));
        memcpy(&indices[geoRange.startIndexLocation_], geo->indices_.Buffer(), geo->indices_.Size() * sizeof(UInt32));
    }

    gpuSceneVertexBuffer_ = new VertexBuffer();
    gpuSceneVertexBuffer_->SetSize(vertexCount, PODVector<VertexElement>({ VertexElement(VE_VECTOR3, SEM_POSITION) }));
    gpuSceneVertexBuffer_->SetData(verts.Buffer());

    gpuSceneIndexBuffer_ = new IndexBuffer();
    gpuSceneIndexBuffer_->SetSize(sizeof(UInt32), indexCount);
    gpuSceneIndexBuffer_->SetData(indices.Buffer());

    PODVector<VertexElement> elements =
    {
        VertexElement(VE_VECTOR4, SEM_INSTANCE, 0, true),
        VertexElement(VE_VECTOR4, SEM_INSTANCE, 1, true),
        VertexElement(VE_VECTOR4, SEM_INSTANCE, 2, true),
        VertexElement(VE_INT,     SEM_INSTANCE, 3, true),
        VertexElement(VE_VECTOR4, SEM_INSTANCE, 4, true),
    };
    instanceDataBuffer_ = new VertexBuffer();
    instanceDataBuffer_->SetSize(instances.Size(), elements);
    instanceDataBuffer_->SetData(instanceData.Buffer());
}

void PVSBakerPipline::BuildGpuScene_RenderTarget(const Vector<SceneInstance>& instances)
{
    renderTextureModelIDArray_ = new Texture2DArray();
    renderTextureModelIDArray_->SetNumLevels(1);
    renderTextureModelIDArray_->SetSize(sampleCount_ * sampleFaceCount_, rtSize_.x_, rtSize_.y_, TEXTURE_FORMAT_R32U, TEXTURE_RENDERTARGET);
    renderTextureModelIDArray_->SetGpuTag("RT_ModelID");

    depthStencilArray_.Clear();
    for (UInt32 i = 0 ; i < sampleCount_ * sampleFaceCount_; ++i)
    {
        auto depthStencil = MakeShared<Texture2D>();
        depthStencil->SetNumLevels(1);
        depthStencil->SetSize(rtSize_.x_, rtSize_.y_, TEXTURE_FORMAT_D24S8, TEXTURE_DEPTHSTENCIL);
        depthStencil->SetGpuTag(ToString("DS_ModelID[%d]", i));
        depthStencilArray_.Push(depthStencil);

        if (renderSurfaceDebugView_)
        {
            auto depthRT = MakeShared<Texture2D>();
            depthRT->SetNumLevels(1);
            depthRT->SetSize(rtSize_.x_, rtSize_.y_, TEXTURE_FORMAT_RGBA8, TEXTURE_RENDERTARGET);
            renderTargetDebugViews_.Push(depthRT);
        }
    }
}

void PVSBakerPipline::BuildGpuScene_DrawIndirectBuffer(const Vector<SceneInstance>& instances)
{
    drawIndirectBuffer_ = GfxDevice::GetDevice()->CreateBuffer();
    drawIndirectBuffer_->SetStride(sizeof(DrawIndexedIndirectArgs));
    drawIndirectBuffer_->SetSize(sizeof(DrawIndexedIndirectArgs) * instances.Size() * sampleCount_ * sampleFaceCount_);
    drawIndirectBuffer_->SetBind(BUFFER_BIND_COMPUTE_READ | BUFFER_BIND_COMPUTE_WRITE);
    drawIndirectBuffer_->SetAccess(BUFFER_ACCESS_NONE);
    drawIndirectBuffer_->SetUsage(BUFFER_USAGE_DYNAMIC);
    drawIndirectBuffer_->Apply(nullptr);
    drawIndirectBuffer_->SetGpuTag("DrawGeometryCount");

    globalIndex_ = GfxDevice::GetDevice()->CreateBuffer();
    globalIndex_->SetStride(sizeof(UInt32));
    globalIndex_->SetSize(sizeof(UInt32) * sampleCount_ * sampleFaceCount_);
    globalIndex_->SetBind(BUFFER_BIND_COMPUTE_READ | BUFFER_BIND_COMPUTE_WRITE);
    globalIndex_->SetAccess(BUFFER_ACCESS_NONE);
    globalIndex_->SetUsage(BUFFER_USAGE_DYNAMIC);
    globalIndex_->Apply(nullptr);
    globalIndex_->SetGpuTag("DrawGeometryCount");
}

void PVSBakerPipline::BuildGpuScene_CellDataBuffer(const Vector<SceneInstance>& instances)
{
    visibilityMap_ = GfxDevice::GetDevice()->CreateBuffer();
    visibilityMap_->SetStride(sizeof(UInt32));
    visibilityMap_->SetSize(sizeof(UInt32) * instances.Size());
    visibilityMap_->SetBind(BUFFER_BIND_COMPUTE_READ | BUFFER_BIND_COMPUTE_WRITE);
    visibilityMap_->SetAccess(BUFFER_ACCESS_NONE);
    visibilityMap_->SetUsage(BUFFER_USAGE_DYNAMIC);
    visibilityMap_->Apply(nullptr);
    visibilityMap_->SetGpuTag("VisibilityMap");

    cellVisibilityIDRange_ = GfxDevice::GetDevice()->CreateBuffer();
    cellVisibilityIDRange_->SetStride(sizeof(VisibleIDPerCell));
    cellVisibilityIDRange_->SetSize(sizeof(VisibleIDPerCell) * READBACK_CELL_COUNT_EACH_TIME);
    cellVisibilityIDRange_->SetBind(BUFFER_BIND_COMPUTE_READ | BUFFER_BIND_COMPUTE_WRITE);
    cellVisibilityIDRange_->SetAccess(BUFFER_ACCESS_NONE);
    cellVisibilityIDRange_->SetUsage(BUFFER_USAGE_DYNAMIC);
    cellVisibilityIDRange_->Apply(nullptr);
    cellVisibilityIDRange_->SetGpuTag("CellVisibilityIDRange");

    cellVisibilityIDBuffer_ = GfxDevice::GetDevice()->CreateBuffer();
    cellVisibilityIDBuffer_->SetStride(sizeof(UInt32));
    cellVisibilityIDBuffer_->SetSize(sizeof(UInt32) * READBACK_CELL_COUNT_EACH_TIME * MAX_VISIBILITY_ID_COUNT_PER_CELL);
    cellVisibilityIDBuffer_->SetBind(BUFFER_BIND_COMPUTE_READ | BUFFER_BIND_COMPUTE_WRITE);
    cellVisibilityIDBuffer_->SetAccess(BUFFER_ACCESS_NONE);
    cellVisibilityIDBuffer_->SetUsage(BUFFER_USAGE_DYNAMIC);
    cellVisibilityIDBuffer_->Apply(nullptr);
    cellVisibilityIDBuffer_->SetGpuTag("CellVisibilityIDBuffer");

    cellVisibilityIDIndex_ = GfxDevice::GetDevice()->CreateBuffer();
    cellVisibilityIDIndex_->SetStride(sizeof(UInt32));
    cellVisibilityIDIndex_->SetSize(sizeof(UInt32));
    cellVisibilityIDIndex_->SetBind(BUFFER_BIND_COMPUTE_READ | BUFFER_BIND_COMPUTE_WRITE);
    cellVisibilityIDIndex_->SetAccess(BUFFER_ACCESS_NONE);
    cellVisibilityIDIndex_->SetUsage(BUFFER_USAGE_DYNAMIC);
    cellVisibilityIDIndex_->Apply(nullptr);
    cellVisibilityIDIndex_->SetGpuTag("CellVisibilityIDIndex");

    currentCellID_ = GfxDevice::GetDevice()->CreateBuffer();
    currentCellID_->SetStride(sizeof(UInt32));
    currentCellID_->SetSize(sizeof(UInt32));
    currentCellID_->SetBind(BUFFER_BIND_COMPUTE_READ | BUFFER_BIND_COMPUTE_WRITE);
    currentCellID_->SetAccess(BUFFER_ACCESS_NONE);
    currentCellID_->SetUsage(BUFFER_USAGE_DYNAMIC);
    currentCellID_->Apply(nullptr);
    currentCellID_->SetGpuTag("CurrentCellID");
}

void PVSBakerPipline::BuildGpuScene_Uniform(const Vector<SceneInstance>& instances)
{
    // MergeVisibleObjectsPerCell.hlsl
    {
        struct Params
        {
            UInt32 modelIDRange;
            IntVector2 textureSize;
            UInt32 sampleCount;
        };

        if (!mergeVisibleObjectsUniform_)
        {
            mergeVisibleObjectsUniform_ = GfxDevice::GetDevice()->CreateBuffer();
            mergeVisibleObjectsUniform_->SetStride(0);
            mergeVisibleObjectsUniform_->SetSize(128);
            mergeVisibleObjectsUniform_->SetBind(BUFFER_BIND_UNIFORM);
            mergeVisibleObjectsUniform_->SetAccess(BUFFER_ACCESS_WRITE);
            mergeVisibleObjectsUniform_->SetUsage(BUFFER_USAGE_DYNAMIC);
            mergeVisibleObjectsUniform_->Apply(nullptr);
            mergeVisibleObjectsUniform_->SetGpuTag("MergeVisibleObjectsUniform");
        }

        Params params =
        {
            gpuSceneModelIDRange_,
            rtSize_,
            sampleCount_ * sampleFaceCount_,
        };
        void* data = mergeVisibleObjectsUniform_->BeginWrite(0, sizeof(Params));
        Memory::Memcpy(data, &params, sizeof(params));
        mergeVisibleObjectsUniform_->EndWrite(sizeof(Params));
    }

    // SaveVisibleObjectsPerCell.hlsl
    {
        struct Params
        {
            UInt32 modelIDrange;
            UInt32 maxVisibilityIDCountPerCell;
        };

        if (!saveVisibleObjectsUniform_)
        {
            saveVisibleObjectsUniform_ = GfxDevice::GetDevice()->CreateBuffer();
            saveVisibleObjectsUniform_->SetStride(0);
            saveVisibleObjectsUniform_->SetSize(128);
            saveVisibleObjectsUniform_->SetBind(BUFFER_BIND_UNIFORM);
            saveVisibleObjectsUniform_->SetAccess(BUFFER_ACCESS_WRITE);
            saveVisibleObjectsUniform_->SetUsage(BUFFER_USAGE_DYNAMIC);
            saveVisibleObjectsUniform_->Apply(nullptr);
            saveVisibleObjectsUniform_->SetGpuTag("SaveVisibleObjectsUniform");
        }

        Params params = { gpuSceneModelIDRange_, MAX_VISIBILITY_ID_COUNT_PER_CELL };
        void* data = saveVisibleObjectsUniform_->BeginWrite(0, sizeof(Params));
        Memory::Memcpy(data, &params, sizeof(params));
        saveVisibleObjectsUniform_->EndWrite(sizeof(Params));
    }
}

static void GetRandomSamplePoints(const IntVector3& sampleCount, const Vector3& samplePointOffset, Vector<Vector4>& randomSamplePoints)
{
    randomSamplePoints.Clear();

    Int32 x1 = -sampleCount.x_ / 2;
    Int32 x2 = sampleCount.x_ / 2;
    Int32 y1 = -sampleCount.y_ / 2;
    Int32 y2 = sampleCount.y_ / 2;
    Int32 z1 = -sampleCount.z_ / 2;
    Int32 z2 = sampleCount.z_ / 2;

    float percentX = sampleCount.x_ > 1 ? 1.0f / (sampleCount.x_ - 1.0f) : 0.0f;
    float percentY = sampleCount.y_ > 1 ? 1.0f / (sampleCount.y_ - 1.0f) : 0.0f;
    float percentZ = sampleCount.z_ > 1 ? 1.0f / (sampleCount.z_ - 1.0f) : 0.0f;

    for (Int32 x = x1; x <= x2; ++x)
    {
        for (Int32 y = y1; y <= y2; ++y)
        {
            for (Int32 z = z1; z <= z2; ++z)
            {
                randomSamplePoints.Push(Vector4(percentX * x, percentY * y, percentZ * z, 0) + Vector4(samplePointOffset, 0));
            }
        }
    }
}

void PVSBakerPipline::MultiDrawCell(SharedPtr<Camera> renderCamera, const PODVector<IntVector3>& cellPositions, const TopologySceneInstance& topologyInfo, const Vector3& samplePointOffset)
{
    PrepareCellParams(renderCamera, cellPositions, samplePointOffset);

    // 拓扑渲染的这部分Gpu设置数据（每个区域只会更新一次，数据量比较小，暂时不加入构建数据的流程了）
    {
        PODVector<DrawIndexedIndirectArgs> drawIndirectBuffer(topologyInfo.sceneInstancesIndex_.Size());
        for (UInt32 i = 0; i < topologyInfo.sceneInstancesIndex_.Size(); ++i)
        {
            UInt32 index = topologyInfo.sceneInstancesIndex_[i];
            const GpuSceneGeometry& geomData = gpuSceneGeometryData_[index];

            DrawIndexedIndirectArgs& args = drawIndirectBuffer[i];
            args.indexCountPerInstance = geomData.indexCount_;
            args.instanceCount         = 1;
            args.startIndexLocation    = geomData.startIndexLocation_;
            args.baseVertexLocation    = geomData.baseVertexLocation_;
            args.startInstanceLocation = geomData.startInstanceLocation_;
        }

        topologyRanges_.Clear();
        topologyRanges_.Resize(topologyInfo.rangeStart_.Size());
        for (UInt32 i = 0; i < topologyInfo.rangeStart_.Size(); ++i)
        {
            topologyRanges_[i].start_ = topologyInfo.rangeStart_[i];
            if (i + 1 < topologyInfo.rangeStart_.Size())
                topologyRanges_[i].count_ = topologyInfo.rangeStart_[i + 1] - topologyInfo.rangeStart_[i];
            else
                topologyRanges_[i].count_ = topologyInfo.sceneInstancesIndex_.Size() - topologyInfo.rangeStart_[i];
        }

        drawTopologyGeometryIndirectBuffer_ = GfxDevice::GetDevice()->CreateBuffer();
        drawTopologyGeometryIndirectBuffer_->SetStride(sizeof(DrawIndexedIndirectArgs));
        drawTopologyGeometryIndirectBuffer_->SetSize(sizeof(DrawIndexedIndirectArgs) * topologyInfo.sceneInstancesIndex_.Size());
        drawTopologyGeometryIndirectBuffer_->SetBind(BUFFER_BIND_COMPUTE_READ);
        drawTopologyGeometryIndirectBuffer_->SetAccess(BUFFER_ACCESS_WRITE);
        drawTopologyGeometryIndirectBuffer_->SetUsage(BUFFER_USAGE_DYNAMIC);
        drawTopologyGeometryIndirectBuffer_->Apply(drawIndirectBuffer.Buffer());
        drawTopologyGeometryIndirectBuffer_->SetGpuTag("DrawTopologyGeometryIndirectBuffer");
    }

    for (UInt32 cellID = 0; cellID < cellPositions.Size(); ++cellID)
    {
        BeginNewCell(cellPositions[cellID]);
        GpuDrivenDrawFastImpl(cellID);
#if 1
        depthWrite_ = false;
        bool reset = true;
        TopologyGeometryRange range;

        for (UInt32 i = 0; i < topologyInfo.sceneInstancesIndex_.Size(); ++i)
        {
            range.start_ = i;
            range.count_ = 1;
            GpuDrivenDraw_DrawIndirect(cellID, false, &range);
            MergeVisibleObjectsPerCell(reset);
            reset = false;
        }

        depthWrite_ = true;
#else
        bool reset = true;

        for (Int32 index = topologyRanges_.Size() - 1; index >= 0; --index)
        {
            GpuDriveDraw_DrawIndirect(cellID, false, &topologyRanges_[index]);
            MergeVisibleObjectsPerCell(reset);
            reset = false;
        }
#endif
        EndCell();
    }
}

void PVSBakerPipline::PrepareCellParams(SharedPtr<Camera> renderCamera, const IntVector3& boundMin, const IntVector3& boundMax, const Vector3& samplePointOffset)
{
    boundMin_ = boundMin;
    boundMax_ = boundMax;

    PrepareCellParamsImpl(renderCamera, samplePointOffset, nullptr);
}

void PVSBakerPipline::PrepareCellParams(SharedPtr<Camera> renderCamera, const PODVector<IntVector3>& cellPositions, const Vector3& samplePointOffset)
{
    boundMin_ = IntVector3::ZERO;
    boundMax_ = IntVector3::ZERO;

    gpuCullUniformCompute_.Reset();
    drawIndirectUniformCompute_.Reset();

    PrepareCellParamsImpl(renderCamera, samplePointOffset, &cellPositions);
}

void PVSBakerPipline::PrepareCellParamsImpl(SharedPtr<Camera> renderCamera, const Vector3& samplePointOffset, const PODVector<IntVector3>* cellPositions)
{
    PROFILE_DETAIL(PVSBakerPipline::PrepareCellParams);

    const IntVector3 delta = boundMax_ - boundMin_ + IntVector3::ONE;
    const IntVector3 threadGroup = cellPositions ?
        IntVector3((cellPositions->Size() + THREAD_COUNT - 1) / THREAD_COUNT, 1, 1) :
        IntVector3((delta.x_ + 15) / 16, (delta.y_ + 3) / 4, (delta.z_ + 15) / 16);
    const UInt32 cellsCount = cellPositions ? cellPositions->Size() : delta.x_ * delta.y_ * delta.z_;
    const UInt32 dataElementCount = cellsCount * sampleCount_ * MAX_FACE_COUNT;

    if (!gpuPrepareCellParamsUniform_)
    {
        gpuPrepareCellParamsUniform_ = new ShaderParameters();
        gpuPrepareCellParamsUniform_->AddParametersDefine<float>("F_DEGTORAD_2");
        gpuPrepareCellParamsUniform_->AddParametersDefine<float>("fov");
        gpuPrepareCellParamsUniform_->AddParametersDefine<float>("aspectRatio");
        gpuPrepareCellParamsUniform_->AddParametersDefine<float>("zoom");
        gpuPrepareCellParamsUniform_->AddParametersDefine<float>("nearZ");
        gpuPrepareCellParamsUniform_->AddParametersDefine<float>("farZ");
        gpuPrepareCellParamsUniform_->AddParametersDefine<Matrix4>("projMatrix");
        gpuPrepareCellParamsUniform_->AddParametersDefine<IntVector3>("bound");
        gpuPrepareCellParamsUniform_->AddParametersDefine<Vector3>("boundMin");
        gpuPrepareCellParamsUniform_->AddParametersDefine<UInt32>("sampleCount");
    }

    gpuPrepareCellParamsUniform_->SetValue<float>("F_DEGTORAD_2", F_DEGTORAD_2);
    gpuPrepareCellParamsUniform_->SetValue<float>("fov", renderCamera->GetFov());
    gpuPrepareCellParamsUniform_->SetValue<float>("aspectRatio", renderCamera->GetAspect());
    gpuPrepareCellParamsUniform_->SetValue<float>("zoom", renderCamera->GetZoom());
    gpuPrepareCellParamsUniform_->SetValue<float>("nearZ", renderCamera->GetNearClip());
    gpuPrepareCellParamsUniform_->SetValue<float>("farZ", renderCamera->GetFarClip());
    gpuPrepareCellParamsUniform_->SetValue<Matrix4>("projMatrix", renderCamera->GetProjectionMatrix());
    gpuPrepareCellParamsUniform_->SetValue<IntVector3>("bound", !cellPositions ? delta : IntVector3(cellPositions->Size(), 0, 0));
    gpuPrepareCellParamsUniform_->SetValue<Vector3>("boundMin", Vector3(boundMin_));
    gpuPrepareCellParamsUniform_->SetValue<UInt32>("sampleCount", sampleCount_);

    if (!gpuCullUniformCompute_)
    {
        gpuCullUniformCompute_ = GfxDevice::GetDevice()->CreateBuffer();
        gpuCullUniformCompute_->SetStride(sizeof(PVSGpuCullUniform));
        gpuCullUniformCompute_->SetSize(sizeof(PVSGpuCullUniform) * dataElementCount);
        gpuCullUniformCompute_->SetBind(BUFFER_BIND_COMPUTE_READ | BUFFER_BIND_COMPUTE_WRITE);
        gpuCullUniformCompute_->SetAccess(BUFFER_ACCESS_NONE);
        gpuCullUniformCompute_->SetUsage(BUFFER_USAGE_DYNAMIC);
        gpuCullUniformCompute_->Apply(nullptr);
        gpuCullUniformCompute_->SetGpuTag("GpuCullUniformCompute");
    }

    if (!drawIndirectUniformCompute_)
    {
        drawIndirectUniformCompute_ = GfxDevice::GetDevice()->CreateBuffer();
        drawIndirectUniformCompute_->SetStride(sizeof(PVSDrawIndirectUniform));
        drawIndirectUniformCompute_->SetSize(sizeof(PVSDrawIndirectUniform) * dataElementCount);
        drawIndirectUniformCompute_->SetBind(BUFFER_BIND_COMPUTE_READ | BUFFER_BIND_COMPUTE_WRITE);
        drawIndirectUniformCompute_->SetAccess(BUFFER_ACCESS_NONE);
        drawIndirectUniformCompute_->SetUsage(BUFFER_USAGE_DYNAMIC);
        drawIndirectUniformCompute_->Apply(nullptr);
        drawIndirectUniformCompute_->SetGpuTag("DrawIndirectUniformCompute");
    }

    {
        // Vector4，Gpu字节对齐，避免跨CacheLine采样，加速5%
        Vector<Vector4> randomSamplePoints;
        GetRandomSamplePoints(sampleCount3D_, samplePointOffset, randomSamplePoints);

        samplePointsBuffer_ = GfxDevice::GetDevice()->CreateBuffer();
        samplePointsBuffer_->SetStride(sizeof(Vector4));
        samplePointsBuffer_->SetSize(sizeof(Vector4) * randomSamplePoints.Size());
        samplePointsBuffer_->SetBind(BUFFER_BIND_COMPUTE_READ);
        samplePointsBuffer_->SetAccess(BUFFER_ACCESS_WRITE);
        samplePointsBuffer_->SetUsage(BUFFER_USAGE_DYNAMIC);
        samplePointsBuffer_->Apply(randomSamplePoints.Buffer());
    }

    if (cellPositions)
    {
        static PODVector<Vector4> cellPositions_Vec4;
        cellPositions_Vec4.Resize(cellPositions->Size());
        for (Int32 i = 0; i < cellPositions_Vec4.Size(); ++i)
        {
            const IntVector3& iPos = (*cellPositions)[i];
            Vector4& fPos = cellPositions_Vec4[i];
            fPos.x_ = iPos.x_;
            fPos.y_ = iPos.y_;
            fPos.z_ = iPos.z_;
            fPos.w_ = 0.0f;
        }

        // Vector4，Gpu字节对齐，避免跨CacheLine采样，加速5%
        cellPositionBuffer_ = GfxDevice::GetDevice()->CreateBuffer();
        cellPositionBuffer_->SetStride(sizeof(Vector4));
        cellPositionBuffer_->SetSize(sizeof(Vector4) * cellPositions_Vec4.Size());
        cellPositionBuffer_->SetBind(BUFFER_BIND_COMPUTE_READ);
        cellPositionBuffer_->SetAccess(BUFFER_ACCESS_WRITE);
        cellPositionBuffer_->SetUsage(BUFFER_USAGE_DYNAMIC);
        cellPositionBuffer_->Apply(cellPositions_Vec4.Buffer());
        cellPositionBuffer_->SetGpuTag("CellPositionBuffer");
    }

    auto* gfxDevice = GfxDevice::GetDevice();

    // Reset constant buffer
    gfxDevice->ResetConstantBuffer();

    // Set uniforms
    gfxDevice->SetEngineShaderParameters(gpuPrepareCellParamsUniform_);

    // Set compute buffers
    gfxDevice->ResetComputeResources();
    gfxDevice->SetComputeBuffer(0, gpuCullUniformCompute_, COMPUTE_BIND_ACCESS_WRITE);
    gfxDevice->SetComputeBuffer(1, drawIndirectUniformCompute_, COMPUTE_BIND_ACCESS_WRITE);
    gfxDevice->SetComputeBuffer(2, samplePointsBuffer_, COMPUTE_BIND_ACCESS_READ);

    if (!cellPositions)
    {
        gfxDevice->SetComputeShader(gpuPrepareCellParamsCS_->GetGfxRef());
    }
    else
    {
        gfxDevice->SetComputeBuffer(3, cellPositionBuffer_, COMPUTE_BIND_ACCESS_READ);
        gfxDevice->SetComputeShader(gpuPrepareCellParams2CS_->GetGfxRef());
    }

    // Run
    gfxDevice->Dispatch(threadGroup.x_, threadGroup.y_, threadGroup.z_);

    // Flush
    gfxDevice->Flush();

    // Reset constant buffer
    gfxDevice->ResetConstantBuffer();

    // Reset uniforms
    gfxDevice->SetEngineShaderParameters(nullptr);
}

void PVSBakerPipline::GpuDrivenDraw(SharedPtr<Camera> cullCamera, SharedPtr<Camera> renderCamera)
{
    PROFILE_DETAIL(PVSBakerPipline::GpuDrivenDraw);

    GpuDrivenDraw_Uniform(cullCamera, renderCamera);
    GpuDrivenDraw_GpuCull();
    GpuDrivenDraw_DrawIndirect(-1);
}

void PVSBakerPipline::GpuDrivenDrawFast()
{
    // uint cellID = DTid.y * bound.x * bound.z +
    //               DTid.z * bound.x +
    //               DTid.x;
    const IntVector3 cellIndex3 = cellIndexArray_.Back() - boundMin_;
    const IntVector3 delta = boundMax_ - boundMin_ + IntVector3::ONE;
    const UInt32 cellID = cellIndex3.y_ * delta.x_ * delta.z_ +
                          cellIndex3.z_ * delta.x_ +
                          cellIndex3.x_;
    GpuDrivenDrawFastImpl(cellID);
}

void PVSBakerPipline::GpuDrivenDrawFastImpl(const UInt32 cellID)
{
    PROFILE_DETAIL(PVSBakerPipline::GpuDrivenDrawFast);

    GpuDrivenDraw_UniformFast(cellID);
    GpuDrivenDraw_GpuCull();
    GpuDrivenDraw_DrawIndirect(cellID);
}

void PVSBakerPipline::GpuDrivenDraw_Uniform(SharedPtr<Camera> cullCamera, SharedPtr<Camera> renderCamera)
{
    {
        if (!gpuCullUniform_)
        {
            gpuCullUniform_ = GfxDevice::GetDevice()->CreateBuffer();
            gpuCullUniform_->SetStride(0);
            gpuCullUniform_->SetSize(128);
            gpuCullUniform_->SetBind(BUFFER_BIND_UNIFORM);
            gpuCullUniform_->SetAccess(BUFFER_ACCESS_WRITE);
            gpuCullUniform_->SetUsage(BUFFER_USAGE_DYNAMIC);
            gpuCullUniform_->Apply(nullptr);
            gpuCullUniform_->SetGpuTag("GpuCullUniform");
        }

        struct Params
        {
            UInt32 geometryCount;
            UInt32 sampleCount;
            UInt32 startSampleID;
        };

        Params params = 
        {
            gpuSceneGeometryCount_,
            // 这个采样点个数包含了6个面的情况
            sampleCount_ * sampleFaceCount_,
            0,
        };

        void* data = gpuCullUniform_->BeginWrite(0, sizeof(Params));
        Memory::Memcpy(data, &params, sizeof(Params));
        gpuCullUniform_->EndWrite(sizeof(Params));

        const auto& cullFrustum = cullCamera->GetFrustum();
        PODVector<PVSGpuCullUniform> paramArray(sampleCount_ * sampleFaceCount_);
        for (UInt32 i = 0; i < paramArray.Size(); ++i)
        {
            paramArray[i] = 
            {
                Vector4(cullFrustum.planes_[0].normal_, cullFrustum.planes_[0].d_),
                Vector4(cullFrustum.planes_[1].normal_, cullFrustum.planes_[1].d_),
                Vector4(cullFrustum.planes_[2].normal_, cullFrustum.planes_[2].d_),
                Vector4(cullFrustum.planes_[3].normal_, cullFrustum.planes_[3].d_),
                Vector4(cullFrustum.planes_[4].normal_, cullFrustum.planes_[4].d_),
                Vector4(cullFrustum.planes_[5].normal_, cullFrustum.planes_[5].d_),
            };
        }
        gpuCullUniformCompute_->SetSize(sizeof(PVSGpuCullUniform) * sampleCount_ * sampleFaceCount_);
        gpuCullUniformCompute_->Apply(paramArray.Buffer());
    }

    {
        if (!drawIndirectUniform_)
        {
            drawIndirectUniform_ = GfxDevice::GetDevice()->CreateBuffer();
            drawIndirectUniform_->SetStride(0);
            drawIndirectUniform_->SetSize(128);
            drawIndirectUniform_->SetBind(BUFFER_BIND_UNIFORM);
            drawIndirectUniform_->SetAccess(BUFFER_ACCESS_WRITE);
            drawIndirectUniform_->SetUsage(BUFFER_USAGE_DYNAMIC);
            drawIndirectUniform_->Apply(nullptr);
            drawIndirectUniform_->SetGpuTag("DrawIndirectUniform");
        }

        PVSDrawIndirectUniform params =
        {
            renderCamera->GetProjectionMatrix() * renderCamera->GetViewMatrix(),
        };
        void* data = drawIndirectUniform_->BeginWrite(0, sizeof(PVSDrawIndirectUniform));
        Memory::Memcpy(data, &params, sizeof(PVSDrawIndirectUniform));
        drawIndirectUniform_->EndWrite(sizeof(PVSDrawIndirectUniform));
    }
}

void PVSBakerPipline::GpuDrivenDraw_UniformFast(UInt32 cellID)
{
    {
        if (!gpuCullUniform_)
        {
            gpuCullUniform_ = GfxDevice::GetDevice()->CreateBuffer();
            gpuCullUniform_->SetStride(0);
            gpuCullUniform_->SetSize(128);
            gpuCullUniform_->SetBind(BUFFER_BIND_UNIFORM);
            gpuCullUniform_->SetAccess(BUFFER_ACCESS_WRITE);
            gpuCullUniform_->SetUsage(BUFFER_USAGE_DYNAMIC);
            gpuCullUniform_->Apply(nullptr);
            gpuCullUniform_->SetGpuTag("GpuCullUniform");
        }

        struct Params
        {
            UInt32 geometryCount;
            UInt32 sampleCount;
            UInt32 startSampleID;
        };

        Params params = 
        {
            gpuSceneGeometryCount_,
            // 这个采样点个数包含6个面的情况
            sampleCount_ * sampleFaceCount_,
            // 采样开始的ID
            cellID * sampleCount_ * MAX_FACE_COUNT,
        };

        void* data = gpuCullUniform_->BeginWrite(0, sizeof(Params));
        Memory::Memcpy(data, &params, sizeof(Params));
        gpuCullUniform_->EndWrite(sizeof(Params));
    }

    if (!drawIndirectUniform_)
    {
        drawIndirectUniform_ = GfxDevice::GetDevice()->CreateBuffer();
        drawIndirectUniform_->SetStride(0);
        drawIndirectUniform_->SetSize(128);
        drawIndirectUniform_->SetBind(BUFFER_BIND_UNIFORM);
        drawIndirectUniform_->SetAccess(BUFFER_ACCESS_NONE);
        drawIndirectUniform_->SetUsage(BUFFER_USAGE_STATIC);
        drawIndirectUniform_->Apply(nullptr);
        drawIndirectUniform_->SetGpuTag("DrawIndirectUniform");
    }
}

void PVSBakerPipline::GpuDrivenDraw_GpuCull()
{
    PROFILE_DETAIL(PVSBakerPipline::GpuDrivenDraw_GpuCull);

    cellGpuCullTimer_.Reset();

    auto* gfxDevice = GfxDevice::GetDevice();

    // Set uniforms
    gfxDevice->SetConstantBuffer(0, gpuCullUniform_);

    // Set compute buffers
    gfxDevice->ResetComputeResources();
    gfxDevice->SetComputeBuffer(0, gpuCullUniformCompute_, COMPUTE_BIND_ACCESS_READ);
    gfxDevice->SetComputeBuffer(1, gpuBoundingBoxBuffer_, COMPUTE_BIND_ACCESS_READ);
    gfxDevice->SetComputeBuffer(2, gpuSceneGeometryBuffer_, COMPUTE_BIND_ACCESS_READ);
    gfxDevice->SetComputeBuffer(3, drawIndirectBuffer_, COMPUTE_BIND_ACCESS_WRITE);
    gfxDevice->SetComputeBuffer(4, globalIndex_, COMPUTE_BIND_ACCESS_READWRITE);

    // Init globalIndex
    gfxDevice->SetComputeShader(gpuFrustumCullInitCS_->GetGfxRef());
    gfxDevice->Dispatch((gpuSceneGeometryCount_ + THREAD_COUNT - 1) / THREAD_COUNT, 1, 1);

    // Gpu culling
    gfxDevice->SetComputeShader(gpuFrustumCullCS_->GetGfxRef());
    gfxDevice->Dispatch((gpuSceneGeometryCount_ + THREAD_COUNT - 1) / THREAD_COUNT, 1, 1);

    cellGpuCullTime_ += cellGpuCullTimer_.GetUSec(false);
}  

void PVSBakerPipline::GpuDrivenDraw_DrawIndirect(Int32 cellID, bool clearRT, const TopologyGeometryRange* topologyRange)
{
    PROFILE_DETAIL(PVSBakerPipline::GpuDrivenDraw_DrawIndirect);

    cellDrawIndirectTimer_.Reset();

    auto* gfxDevice = GfxDevice::GetDevice();

    // Reset
    gfxDevice->ResetComputeResources();

    // Set input layout
    gfxDevice->SetVertexDescription(gpuSceneVertexBuffer_->GetVertexDescription());
    gfxDevice->SetInstanceDescription(instanceDataBuffer_->GetVertexDescription());

    // Set vertex buffer
    gfxDevice->SetVertexBuffer(gpuSceneVertexBuffer_->GetGfxRef());
    
    // Set index buffer
    gfxDevice->SetIndexBuffer(gpuSceneIndexBuffer_->GetGfxRef());

    // Set instance buffer
    gfxDevice->SetInstanceBuffer(instanceDataBuffer_->GetGfxRef());

    // Set shaders
    gfxDevice->SetShaders(gpuDrawIndirectVS_->GetGfxRef(), gpuDrawIndirectPS_->GetGfxRef());

    // Set uniforms
    gfxDevice->SetConstantBuffer(0, drawIndirectUniform_);

    // Set primitive type
    gfxDevice->SetPrimitiveType(PRIMITIVE_TRIANGLE);

    // Set back cull
    gfxDevice->SetCullMode(cullMode_);

    // Set depth state
    gfxDevice->SetDepthTestMode(COMPARISON_LESS_EQUAL);
    gfxDevice->SetDepthWrite(depthWrite_);

    // Set viewport
    gfxDevice->SetViewport(IntRect(0, 0, rtSize_.x_, rtSize_.y_));

    for (UInt32 i = 0; i < sampleCount_ * sampleFaceCount_; ++i)
    {
        // Set render target
        if (renderSurfaceDebugView_)
        {
            gfxDevice->SetRenderTarget(0, renderTextureModelIDArray_->GetRenderSurface(i, 0));
            gfxDevice->SetRenderTarget(1, i == debugFace_ ? renderSurfaceDebugView_ : renderTargetDebugViews_[i]->GetRenderSurface());
            gfxDevice->SetDepthStencil(depthStencilArray_[i]->GetRenderSurface());
        }
        else
        {
            gfxDevice->SetRenderTarget(0, renderTextureModelIDArray_->GetRenderSurface(i, 0));
            gfxDevice->SetDepthStencil(depthStencilArray_[i]->GetRenderSurface());
        }

        // Clear render target and depth stencil
        if (clearRT)
        {
            gfxDevice->Clear(CLEAR_COLOR | CLEAR_DEPTH, Color::BLACK, 1.0f);
        }

        if (cellID >= 0)
        {
            // Copy uniforms
            drawIndirectUniform_->CopyData(drawIndirectUniformCompute_, (cellID * sampleCount_ * MAX_FACE_COUNT + i) * sizeof(PVSDrawIndirectUniform), 0, sizeof(PVSDrawIndirectUniform));
        }

        // Draw
        if (!topologyRange)
        {
            gfxDevice->DrawIndexedInstancedIndirect(drawIndirectBuffer_, sizeof(DrawIndexedIndirectArgs) * gpuSceneGeometryCount_ * i, sizeof(DrawIndexedIndirectArgs), gpuSceneGeometryCount_);
        }
        else
        {
            gfxDevice->DrawIndexedInstancedIndirect(drawTopologyGeometryIndirectBuffer_, sizeof(DrawIndexedIndirectArgs) * topologyRange->start_, sizeof(DrawIndexedIndirectArgs), topologyRange->count_);
        }
    }

    cellDrawIndirectTime_ += cellDrawIndirectTimer_.GetUSec(false);
}

void PVSBakerPipline::MergeVisibleObjectsPerCell(bool reset)
{
    PROFILE_DETAIL(VSBakerPipline::MergeVisibleObjectsPerCell);

    mergeVisibleObjectsTimer_.Reset();

    auto* gfxDevice = GfxDevice::GetDevice();

    // Set uniforms
    gfxDevice->SetConstantBuffer(0, mergeVisibleObjectsUniform_);

    // Set compute texture、buffer
    gfxDevice->ResetComputeResources();
    gfxDevice->SetComputeTexture(0, renderTextureModelIDArray_->GetGfxTextureRef(), COMPUTE_BIND_ACCESS_READ);
    gfxDevice->SetComputeBuffer(1, visibilityMap_, COMPUTE_BIND_ACCESS_WRITE);

    // Init
    if (reset)
    {
        gfxDevice->SetComputeShader(gpuMergeVisibleObjectsInitCS_->GetGfxRef());
        gfxDevice->Dispatch((gpuSceneModelIDRange_ + THREAD_COUNT - 1) / THREAD_COUNT, 1, 1);
    }

    // Set shader
    gfxDevice->SetComputeShader(gpuMergeVisibleObjectsCS_->GetGfxRef());

    // Run
    gfxDevice->Dispatch(rtSize_.x_ / THREAD_X_COUNT, rtSize_.y_ / THREAD_Y_COUNT, 1);

    mergeVisibleObjectsTime_ += mergeVisibleObjectsTimer_.GetUSec(false);
}

void PVSBakerPipline::SaveVisibleObjectPerCell(bool init)
{
    PROFILE_DETAIL(PVSBakerPipline::SaveVisibleObjectPerCell);

    saveVisibleObjectsTimer_.Reset();

    auto* gfxDevice = GfxDevice::GetDevice();

    // Set uniforms
    gfxDevice->SetConstantBuffer(0, saveVisibleObjectsUniform_);

    // Set compute buffers
    gfxDevice->ResetComputeResources();
    gfxDevice->SetComputeBuffer(0, visibilityMap_, COMPUTE_BIND_ACCESS_WRITE);
    gfxDevice->SetComputeBuffer(1, cellVisibilityIDRange_, COMPUTE_BIND_ACCESS_WRITE);
    gfxDevice->SetComputeBuffer(2, cellVisibilityIDBuffer_, COMPUTE_BIND_ACCESS_WRITE);
    gfxDevice->SetComputeBuffer(3, cellVisibilityIDIndex_, COMPUTE_BIND_ACCESS_WRITE);
    gfxDevice->SetComputeBuffer(4, currentCellID_, COMPUTE_BIND_ACCESS_WRITE);

    // Set shader
    if (init)
        gfxDevice->SetComputeShader(gpuSaveVisibleObjectsInitCS_->GetGfxRef());
    else
        gfxDevice->SetComputeShader(gpuSaveVisibleObjectsCS_->GetGfxRef());

    // Run
    gfxDevice->Dispatch(1, 1, 1);

    saveVisibleObjectsTime_ += saveVisibleObjectsTimer_.GetUSec(false);
}

void PVSBakerPipline::ClearCells()
{
    cellIndexArray_.Clear();
}

void PVSBakerPipline::BeginNewCell(const IntVector3& cellIndex)
{
    if (cellIndexArray_.Size() == 0)
    {
        SaveVisibleObjectPerCell(true);
        
        cellBatchTimer_.Reset();
        cellGpuCullTime_ = 0;
        cellDrawIndirectTime_ = 0;
        cellBuildVisibilityBufferTime_ = 0;
        mergeVisibleObjectsTime_ = 0;
        saveVisibleObjectsTime_ = 0;
    }
    // 超过内存上限，分段回读数据
    else if (cellIndexArray_.Size() >= READBACK_CELL_COUNT_EACH_TIME)
    {
        StoreCellsInfo();

        SaveVisibleObjectPerCell(true);

        float totalTime = cellBatchTimer_.GetUSec(true) / 1000.0f;
        FLAGGG_LOG_STD_INFO("Init new cell batch, const time:\n"
            "   total: %lf/ms\n"
            "   gpu-cull: %lf/ms\n"
            "   draw-indirect: %lf/ms\n"
            "   merge-visible-objects: %lf/ms\n"
            "   save-visible-objects: %lf/ms",
            totalTime,
            cellGpuCullTime_ / 1000.0f,
            cellDrawIndirectTime_ / 1000.0f,
            mergeVisibleObjectsTime_ / 1000.0f,
            saveVisibleObjectsTime_ / 10000.0f);
    }

    cellIndexArray_.Push(cellIndex);
}

void PVSBakerPipline::EndCell()
{
    SaveVisibleObjectPerCell(false);
}

void PVSBakerPipline::EndAll()
{
    if (cellIndexArray_.Size())
    {
        StoreCellsInfo();
    }
}

void PVSBakerPipline::StoreCellsInfo()
{
    auto& bakerData = pvsBakerDataArray_.EmplaceBack();
    bakerData.Reset(new PVSBakerData());
    ReadbackCellsInfo(*bakerData);
    cellIndexArray_.Clear();
}

void PVSBakerPipline::ReadbackCellsInfo(PVSBakerData& bakerData)
{
    PROFILE_DETAIL(PVSBakerPipline::ReadbackCellsInfo);

    UInt32 count = 0;

    if (!cellVisibilityIDIndex_->ReadBackSubResigon(&count, 0, sizeof(UInt32)))
    {
        FLAGGG_LOG_ERROR("Failed to readback cell visibility id index.");
        return;
    }

    if (count)
    {
        bakerData.cellDataRange_.Resize(cellIndexArray_.Size());
        if (!cellVisibilityIDRange_->ReadBackSubResigon(&bakerData.cellDataRange_[0], 0, bakerData.cellDataRange_.Size() * sizeof(VisibleIDPerCell)))
        {
            FLAGGG_LOG_ERROR("Failed to readback cell visibility data range.");
            return;
        }

        bakerData.cellIndexArray_ = cellIndexArray_;
        if (!cellVisibilityIDBuffer_->ReadBackSubResigon(&bakerData.modelIDs_[0], 0, count * sizeof(UInt32)))
        {
            FLAGGG_LOG_ERROR("Failed to readback cell visibility id arrary.");
            return;
        }
    }
}

void PVSBakerPipline::ClearBakerData()
{
    pvsBakerDataArray_.Clear();
}
