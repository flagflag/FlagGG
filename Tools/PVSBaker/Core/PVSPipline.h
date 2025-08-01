#pragma once

#include <Core/Object.h>
#include <Math/Matrix3x4.h>
#include <Math/Matrix4.h>
#include <Math/BoundingBox.h>
#include <Math/Color.h>
#include <Math/Frustum.h>
#include <Container/Ptr.h>
#include <Utility/SystemHelper.h>
#include <Graphics/GraphicsDef.h>

namespace FlagGG
{

class VertexBuffer;
class IndexBuffer;
class GfxBuffer;
class GfxRenderSurface;
class Texture2D;
class Texture2DArray;
class ShaderParameters;
class Shader;
class Camera;

}

using namespace FlagGG;

struct SceneGeometry : public RefCounted
{
    PODVector<Vector3> verts_;
    PODVector<UInt32> indices_;
    BoundingBox aabb_;
};

struct SceneInstance
{
    SceneInstance() = default;

    SceneInstance(SharedPtr<SceneGeometry> geometry, const Matrix3x4& worldTransform, UInt32 modelID, const Color& debugColor = Color::WHITE)
        : geometry_(geometry)
        , worldTransform_(worldTransform)
        , modelID_(modelID)
        , color_(debugColor)
    {

    }

    SharedPtr<SceneGeometry> geometry_;
    Matrix3x4 worldTransform_;
    UInt32 modelID_{};
    Color color_;
};

struct TopologySceneInstance
{
    PODVector<UInt32> sceneInstancesIndex_;
    PODVector<UInt32> rangeStart_;
};

struct VisibleIDPerCell
{
    UInt32 startLocation_;
    UInt32 validCount_;
    UInt32 totalCount_;
};

struct PVSBakerData
{
    PODVector<VisibleIDPerCell> cellDataRange_;
    PODVector<UInt32> modelIDs_;
    PODVector<IntVector3> cellIndexArray_;
};

class PVSBakerPipline : public Object
{
    OBJECT_OVERRIDE(PVSBakerPipline, Object);
public:
    PVSBakerPipline(const IntVector3& sampleCount3D, GfxRenderSurface* renderSurfaceDebugView = nullptr, UInt32 face = 0);

    ~PVSBakerPipline() override;

    // 构建Gpu场景
    void BuildGpuScene(const Vector<SceneInstance>& instances, Int32 invalidInstanceCount = -1);

    // 批量渲染格子
    void MultiDrawCell(SharedPtr<Camera> renderCamera, const PODVector<IntVector3>& cellPositions, const TopologySceneInstance& topologyInfo, const Vector3& samplePointOffset = Vector3::ZERO);

    // 预处理uniform，优化uniform设置开销
    void PrepareCellParams(SharedPtr<Camera> renderCamera, const IntVector3& boundMin, const IntVector3& boundMax, const Vector3& samplePointOffset = Vector3::ZERO);

    // 预处理uniform，优化uniform设置开销
    void PrepareCellParams(SharedPtr<Camera> renderCamera, const PODVector<IntVector3>& cellPositions, const Vector3& samplePointOffset = Vector3::ZERO);

    // 给定相机视椎体，在Gpu端做视椎体裁剪，然后渲染一帧
    void GpuDrivenDraw(SharedPtr<Camera> cullCamera, SharedPtr<Camera> renderCamera);

    // 预处理uniform，优化uniform设置开销
    void GpuDrivenDrawFast();

    // 合并单个Cell的可见对象
    void MergeVisibleObjectsPerCell(bool reset);

    // 清理格子数据
    void ClearCells();

    // 开始一个新的Cell
    void BeginNewCell(const IntVector3& cellIndex);

    // 结束当前Cell
    void EndCell();

    // 结束
    void EndAll();

    // 回读Cells信息
    void ReadbackCellsInfo(PVSBakerData& bakerData);

    // 清理烘焙数据
    void ClearBakerData();

    // 获取烘焙数据
    const Vector<UniquePtr<PVSBakerData>>& GetBakerData() const { return pvsBakerDataArray_; }

    Texture2DArray* GetModelIDRenderTargetArray() const { return renderTextureModelIDArray_; }

protected:
    struct TopologyGeometryRange
    {
        UInt32 start_;
        UInt32 count_;
    };

    void BuildGpuScene_AABB(const Vector<SceneInstance>& instances);

    void BuildGpuScene_Geometry(const Vector<SceneInstance>& instances);

    void BuildGpuScene_RenderTarget(const Vector<SceneInstance>& instances);

    void BuildGpuScene_DrawIndirectBuffer(const Vector<SceneInstance>& instances);

    void BuildGpuScene_CellDataBuffer(const Vector<SceneInstance>& instances);

    void PrepareCellParamsImpl(SharedPtr<Camera> renderCamera, const Vector3& samplePointsOffset, const PODVector<IntVector3>* cellPositions);

    void GpuDrivenDrawFastImpl(const UInt32 cellID);

    // 预处理的，静态不会改（优化设置uniform的开销）
    void BuildGpuScene_Uniform(const Vector<SceneInstance>& instances);

    void GpuDrivenDraw_Uniform(SharedPtr<Camera> cullCamera, SharedPtr<Camera> renderCamera);

    void GpuDrivenDraw_UniformFast(UInt32 cellID);

    void GpuDrivenDraw_GpuCull();

    void GpuDrivenDraw_DrawIndirect(Int32 cellID, bool clearRT = true, const TopologyGeometryRange* topologyRange = nullptr);

    // 保存当前格子数据
    void SaveVisibleObjectPerCell(bool init);

    // 缓存当前已有格子数据
    void StoreCellsInfo();

private:
// ========================= 对应Gpu结构，可以在Shader中找到对应定义 =========================
    struct PVSGpuCullUniform
    {
        Vector4 normal_[NUM_FRUSTUM_PLANES];
    };

    struct PVSDrawIndirectUniform
    {
        Matrix4 projViewMatrix;
    };

    struct GpuSceneGeometry
    {
        UInt32 startIndexLocation_;
        UInt32 indexCount_;
        UInt32 baseVertexLocation_;
        UInt32 startInstanceLocation_;
    };
// ======================================================================================

    // 预处理Cell的信息
    IntVector3 boundMin_;
    IntVector3 boundMax_;
    IntVector3 sampleCount3D_;
    UInt32 sampleCount_;
    UInt32 sampleFaceCount_;
    IntVector2 rtSize_;
    PODVector<IntRect> viewports_;
    PODVector<PVSGpuCullUniform> prepareUniform1_;
    PODVector<PVSDrawIndirectUniform> prepareUniform2_;

    // Gpu场景geometry个数
    UInt32 gpuSceneGeometryCount_;

    // ModelID最大值+1
    UInt32 gpuSceneModelIDRange_;

    // 拓扑geometry分段范围
    PODVector<TopologyGeometryRange> topologyRanges_;

    // Gpu场景顶点
    SharedPtr<VertexBuffer> gpuSceneVertexBuffer_;

    // Gpu场景顶点下标
    SharedPtr<IndexBuffer> gpuSceneIndexBuffer_;

    // Gpu场景，每个geometry的instance data
    SharedPtr<VertexBuffer> instanceDataBuffer_;

    // 材质参数
    SharedPtr<ShaderParameters> gpuPrepareCellParamsUniform_;
    SharedPtr<GfxBuffer> gpuCullUniform_;
    SharedPtr<GfxBuffer> drawIndirectUniform_;
    SharedPtr<GfxBuffer> mergeVisibleObjectsUniform_;
    SharedPtr<GfxBuffer> saveVisibleObjectsUniform_;

    // 预计算的
    SharedPtr<GfxBuffer> gpuCullUniformCompute_;
    SharedPtr<GfxBuffer> drawIndirectUniformCompute_;
    SharedPtr<GfxBuffer> samplePointsBuffer_;
    SharedPtr<GfxBuffer> cellPositionBuffer_;

    // Gpu场景，每个geometry的包围盒
    SharedPtr<GfxBuffer> gpuBoundingBoxBuffer_;

    // Gpu场景，每个geometry的场景信息（例如：世界空间下的transform）
    PODVector<GpuSceneGeometry> gpuSceneGeometryData_;
    SharedPtr<GfxBuffer> gpuSceneGeometryBuffer_;

    // 当前Frustum裁剪后需要渲染的geometry
    SharedPtr<GfxBuffer> drawIndirectBuffer_;
    // 拓扑的物件
    SharedPtr<GfxBuffer> drawTopologyGeometryIndirectBuffer_;

    // 渲染物体总数（按照采样分段结构，比如：采样点1的渲染总数 = globalIndex[1] - globalIndex[0]）
    SharedPtr<GfxBuffer> globalIndex_;

    // 可见的ModelID
    SharedPtr<GfxBuffer> visibilityMap_;

    // 每个格子可见的ModelID
    SharedPtr<GfxBuffer> cellVisibilityIDRange_;
    SharedPtr<GfxBuffer> cellVisibilityIDBuffer_;
    SharedPtr<GfxBuffer> cellVisibilityIDIndex_;
    SharedPtr<GfxBuffer> currentCellID_;

    // RT 保存可见的模型ID
    SharedPtr<Texture2DArray> renderTextureModelIDArray_;
    // 深度
    Vector<SharedPtr<Texture2D>> depthStencilArray_;
    // 调试视图
    UInt32 debugFace_;
    SharedPtr<GfxRenderSurface> renderSurfaceDebugView_;
    Vector<SharedPtr<Texture2D>> renderTargetDebugViews_;

    // Shaders
    SharedPtr<Shader> gpuPrepareCellParamsCS_;
    SharedPtr<Shader> gpuPrepareCellParams2CS_;

    SharedPtr<Shader> gpuFrustumCullInitCS_;
    SharedPtr<Shader> gpuFrustumCullCS_;

    SharedPtr<Shader> gpuDrawIndirectVS_;
    SharedPtr<Shader> gpuDrawIndirectPS_;

    SharedPtr<Shader> gpuMergeVisibleObjectsInitCS_;
    SharedPtr<Shader> gpuMergeVisibleObjectsCS_;

    SharedPtr<Shader> gpuSaveVisibleObjectsInitCS_;
    SharedPtr<Shader> gpuSaveVisibleObjectsCS_;

    // Cells
    PODVector<IntVector3> cellIndexArray_;

    Vector<UniquePtr<PVSBakerData>> pvsBakerDataArray_;

    CullMode cullMode_;
    bool depthWrite_;

    HiresTimer cellBatchTimer_;

    HiresTimer cellGpuCullTimer_;
    UInt64 cellGpuCullTime_;

    HiresTimer cellDrawIndirectTimer_;
    UInt64 cellDrawIndirectTime_;

    HiresTimer cellBuildVisibilityBufferTimer_;
    UInt64 cellBuildVisibilityBufferTime_;

    HiresTimer mergeVisibleObjectsTimer_;
    UInt64 mergeVisibleObjectsTime_;

    HiresTimer saveVisibleObjectsTimer_;
    UInt64 saveVisibleObjectsTime_;
};
