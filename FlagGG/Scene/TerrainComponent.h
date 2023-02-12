//
// 地形控件
//

#pragma once

#include "Export.h"
#include "Scene/DrawableComponent.h"
#include "Container/Ptr.h"
#include "Graphics/Material.h"
#include "Graphics/Geometry.h"
#include "Resource/Image.h"
#include "Math/Vector2.h"
#include "Math/Matrix3x4.h"

namespace FlagGG
{

class FlagGG_API TerrainComponent : public DrawableComponent
{
	OBJECT_OVERRIDE(TerrainComponent, DrawableComponent);
public:
	// 设置路径大小
	void SetPathSize(UInt32 pathSize);

	// 设置高度图
	void SetHeightMap(Image* image);

	// 设置材质
	void SetMaterial(Material* material);

	// 是否可渲染
	bool IsRenderable() override { return true; }

	// 获取DrawableFlags
	UInt32 GetDrawableFlags() const override { return DRAWABLE_GEOMETRY; }

	// 更新包围盒
	void OnUpdateWorldBoundingBox() override;

protected:
	// 创建图形数据
	void CreateGeometry();

private:
	UInt32 pathSize_;

	IntVector2 patchesNum_;
	IntVector2 verticesNum_;
	Vector2 patchWorldSize_;
	Vector2 patchWorldOrigin_;
	Matrix3x4 indentity_;

	SharedPtr<Image> heightMap_;
	SharedPtr<Material> material_;
	SharedPtr<Geometry> geometry_;
	SharedPtr<VertexBuffer> vertexBuffer_;
	SharedPtr<IndexBuffer> indexBuffer_;

	BoundingBox meshBoundingBox_;
};

}
