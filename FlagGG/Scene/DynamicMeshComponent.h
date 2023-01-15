//
// 动态网格控件
//

#pragma once

#include "Export.h"
#include "Scene/DrawableComponent.h"
#include "Container/Ptr.h"
#include "Graphics/Material.h"
#include "Math/Vector2.h"
#include "Math/Matrix3x4.h"
#include "Core/BaseTypes.h"

namespace FlagGG
{

class FlagGG_API DynamicMeshComponent : public DrawableComponent
{
	OBJECT_OVERRIDE(DynamicMeshComponent, DrawableComponent);
public:
	DynamicMeshComponent();

	// 设置大小
	void SetSize(UInt32 width, UInt32 height);

	// 设置元素大小
	void SetElementSize(Real elementSize);

	// 设置材质
	void SetMaterial(Material* material);

	// 是否可渲染
	bool IsRenderable() override { return hasGeometry_; }

	// 获取渲染上下文
	RenderContext* GetRenderContext() override;

	// 帧更新
	void Update(Real timeStep) override;

	// 设置相机可见性Mask
	void SetViewMask(UInt32 viewMask) override;

	// 获取高度
	virtual Real GetHeight(Int32 x, Int32 y, Int32 verticesNumX, Int32 verticesNumY);

	// 获取法线
	virtual Vector3 GetNormal(Int32 x, Int32 y, Int32 verticesNumX, Int32 verticesNumY);

	// 获取UV
	virtual Vector2 GetUV(Int32 x, Int32 y, Int32 verticesNumX, Int32 verticesNumY);

	// 获取顶点描述
	virtual const PODVector<VertexElement>& GetVertexElement();

protected:
	// 创建图形数据
	void CreateGeometry();

private:
	UInt32 width_;
	UInt32 height_;
	Real elementSize_;

	SharedPtr<Material> material_;
	SharedPtr<Geometry> geometry_;
	SharedPtr<VertexBuffer> vertexBuffer_;
	SharedPtr<IndexBuffer> indexBuffer_;

	RenderContext renderContext_;

	bool geometryDirty_;
	bool hasGeometry_;
};

}
