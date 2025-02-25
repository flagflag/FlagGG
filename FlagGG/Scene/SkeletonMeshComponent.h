//
// 骨骼网格控件
//

#pragma once

#include "Export.h"
#include "Scene/StaticMeshComponent.h"
#include "Scene/Bone.h"
#include "Container/Vector.h"
#include "Math/Matrix3x4.h"

namespace FlagGG
{

class FlagGG_API SkeletonMeshComponent : public StaticMeshComponent
{
	OBJECT_OVERRIDE(SkeletonMeshComponent, StaticMeshComponent);
public:
	SkeletonMeshComponent();

	// 帧更新
	void Update(Real timeStep) override;

	// 获取DrawableFlags
	UInt32 GetDrawableFlags() const override { return DRAWABLE_SKELETONMESH; }

	// 控件树发生dirty
	void UpdateTreeDirty() override;

	// 更新包围盒
	void OnUpdateWorldBoundingBox() override;

	// 获取骨架
	Skeleton& GetSkeleton();

protected:
	// 模型数据发生变化时调用
	void OnModel() override;

	// 设置骨架
	void SetSkeleton(const Skeleton& skeleton);

	// 设置骨骼映射
	void SetBoneMappings(const Vector<PODVector<UInt32>>& boneMappings);

	// 更新蒙皮数据
	void UpdateSkinning();

	// 更新骨骼包围盒
	void UpdateBoneBoundingBox();

private:
	bool skinningDirty_;
	bool boneBoundingBoxDirty_;
		
	Skeleton skeleton_;

	PODVector<Matrix3x4> skinMatrices_;

	Vector<PODVector<Matrix3x4>> geometrySkinMatrices_;

	Vector<PODVector<Matrix3x4*>> geometrySkinMatrixPtrs_;

	BoundingBox boneBoundingBox_;
};

}
