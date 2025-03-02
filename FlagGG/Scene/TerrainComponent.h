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

class TerrainPatchComponent;

class FlagGG_API TerrainComponent : public Component
{
	OBJECT_OVERRIDE(TerrainComponent, Component);
public:
	TerrainComponent();

	~TerrainComponent() override;

	// 设置Patch大小
	void SetPatchSize(UInt32 patchSize);

	// 设置三角面大小
	void SetQuadSize(const Vector3& quadSize);

	// 设置高度图
	void SetHeightMap(Image* image);

	// 获取高度图
	Image* GetHeightMap() const { return heightMap_; }

	// 设置材质
	void SetMaterial(Material* material);

	// 获取材质
	Material* GetMaterial() const { return material_; }

	// 创建图形数据
	void CreateGeometry();

protected:
	// 创建Patch网格
	void CreatePatchGeometry(TerrainPatchComponent* patch, int patchX, int patchY);

private:
	// Lod层数
	UInt32 numLodLevels_;
	// 最大Lod层数
	UInt32 maxLodLevels_;
	// 每个Patch的顶点数目
	UInt32 patchSize_;
	// 每个Quad的尺寸
	Vector3 quadSize_;

	IntVector2 numPatches_;
	IntVector2 numVertices_;
	Vector2 patchWorldSize_;
	Vector2 patchWorldOrigin_;
	Matrix3x4 indentity_;

	SharedPtr<Image> heightMap_;
	SharedPtr<Material> material_;
	//SharedPtr<VertexBuffer> vertexBuffer_;
	//SharedPtr<IndexBuffer> indexBuffer_;

	Vector<WeakPtr<TerrainPatchComponent>> patches_;
};

}
