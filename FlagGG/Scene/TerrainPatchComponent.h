//
// 地形Patch控件
//

#include "Scene/DrawableComponent.h"

namespace FlagGG
{

class FlagGG_API TerrainPatchComponent : public DrawableComponent
{
	OBJECT_OVERRIDE(TerrainPatchComponent, DrawableComponent);
public:
	// 设置材质
	void SetMaterial(Material* material);

	// 获取材质
	Material* GetMaterial() const { return material_; }

	// 设置Geometry
	void SetGeometry(Geometry* geometry);

	// 设置AABB
	void SetBoundingBox(const BoundingBox& aabb);

	// 是否可渲染
	bool IsRenderable() override { return true; }

	// 获取DrawableFlags
	UInt32 GetDrawableFlags() const override { return DRAWABLE_TERRAIN; }

	// 更新包围盒
	void OnUpdateWorldBoundingBox() override;

private:
	SharedPtr<Material> material_;
	SharedPtr<Geometry> geometry_;

	BoundingBox meshBoundingBox_;
};

}
