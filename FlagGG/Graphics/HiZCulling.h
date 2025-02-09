//
// Hi-Z 遮挡裁剪
// 为了Cpu更高效的查询可见性：
// 1.设计上将HiZVisibilityTestInfo存放在DrawableComponent对象中，这样查询只需要O(1)
// 2.这样避免了开辟一个HashMap保存指针，尤其场景实例极多时，HashMap的常数开销将影响巨大
// 3.这样设计会有一些耦合，但是为了性能忍了！
//

#pragma once

#include "Core/Object.h"
#include "Math/BoundingBox.h"
#include "Container/Ptr.h"
#include "Container/Vector.h"

namespace FlagGG
{

class Texture2D;
class GfxSampler;
class DrawableComponent;
class Shader;
class ShaderParameters;
struct HiZVisibilityTestInfo;
class Camera;

class HiZCulling : public Object
{
	OBJECT_OVERRIDE(HiZCulling, Object);
public:
	HiZCulling();

	~HiZCulling() override;

	// 每帧初始化
	void InitializeFrame(Camera* camera);

	// 构建Hi-Z map
	void BuildHiZMap(Texture2D* depthTexture);

	// 清空要测试的图形
	void ClearGeometries();

	// 增加要测试的图形
	void AddGeometry(DrawableComponent* drawable);

	// 通过Hi-Z map计算已添加图形的可见性
	void CalcGeometriesVisibility();

	// 回读图形可见性结果
	void FetchGeometriesVisibilityResults();

	// 获取图形的可见性
	bool IsGeometryVisible(DrawableComponent* drawable);

protected:
	void AllocTexture();

	void EncodeGeometriesAABB();

private:
	// 是否初始化过
	bool inited_{};

	// 当前帧深度是否是reverseZ
	bool reverseZ_{};

	// 当前帧视图投影
	Matrix4 viewProjectMatrix_;

	// 构建Hi-Z map
	SharedPtr<Shader> buildHiZVS_;
	SharedPtr<Shader> buildHiZPS_;
	SharedPtr<Shader> buildHiZMipsPS_;
	SharedPtr<ShaderParameters> buildHiZPrams_;

	// 计算可见性
	SharedPtr<Shader> calcVisibilityVS_;
	SharedPtr<Shader> calcVisibilityPS_;
	SharedPtr<ShaderParameters> calcVisibilityParams_;

	Vector<SharedPtr<DrawableComponent>> geometries_;

	SharedPtr<GfxSampler> depthSampler_;

	// Hi-Z map
	SharedPtr<Texture2D> HiZMap_;

	// 当前帧 Hi-Z map的高
	UInt32 frameHiZHeight_{};

	// 保存Hi-Z测试的图形数据
	SharedPtr<Texture2D> HiZAABBMinPos_;
	SharedPtr<Texture2D> HiZAABBMaxPos_;

	// 保存Hi-Z测试结果（通过一定的uv关系和图形数据一一对应）
	SharedPtr<Texture2D> HiZResults_;

	//
	PODVector<Vector4> MiZAABBBuffer1_;
	PODVector<Vector4> MiZAABBBuffer2_;
	PODVector<UInt8> MiZResultsBuffer_;
};

}
