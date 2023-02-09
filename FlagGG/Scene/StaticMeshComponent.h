//
// 静态网格控件
//

#pragma once

#include "Export.h"
#include "Scene/DrawableComponent.h"
#include "Graphics/Model.h"
#include "Graphics/Material.h"
#include "Math/BoundingBox.h"

namespace FlagGG
{

class FlagGG_API StaticMeshComponent : public DrawableComponent
{
	OBJECT_OVERRIDE(StaticMeshComponent, DrawableComponent);
public:
	// 帧更新
	void Update(Real timeStep) override;

	// 是否可渲染
	bool IsRenderable() override { return true; }

	// 获取DrawableFlags
	UInt32 GetDrawableFlags() const override { return DRAWABLE_GEOMETRY; }

	// 更新包围盒
	void OnUpdateWorldBoundingBox() override;

	// 设置查询
	void ProcessRayQuery(const RayOctreeQuery& query, PODVector<RayQueryResult>& results) override;

	// 设置相机可见性Mask
	void SetViewMask(UInt32 viewMask) override;

	// 设置模型数据
	void SetModel(Model* model);

	// 设置材质
	void SetMaterial(Material* material);

	// 获取模型数据
	Model* GetModel();

	// 获取材质
	Material* GetMaterial();

protected:
	// 模型数据发生变化时调用
	virtual void OnModel();

	// 材质发生变化时调用
	virtual void OnMaterial();

	SharedPtr<Model> model_;

	SharedPtr<Material> material_;

	BoundingBox boundingBox_;
};

}
