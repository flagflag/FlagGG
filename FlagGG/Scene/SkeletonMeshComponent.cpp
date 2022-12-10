#include "Scene/SkeletonMeshComponent.h"
#include "Scene/Node.h"

namespace FlagGG
{

SkeletonMeshComponent::SkeletonMeshComponent() :
	skinningDirty_(true),
	boneBoundingBoxDirty_(true)
{

}

void SkeletonMeshComponent::Update(Real timeStep)
{
	StaticMeshComponent::Update(timeStep);

	if (skinningDirty_)
	{
		UpdateSkinning();
	}

	if (boneBoundingBoxDirty_)
	{
		UpdateBoneBoundingBox();
	}
}

Skeleton& SkeletonMeshComponent::GetSkeleton()
{
	return skeleton_;
}

void SkeletonMeshComponent::OnModel()
{
	StaticMeshComponent::OnModel();

	// 设置骨骼
	SetSkeleton(model_->GetSkeleton());
	// 分配蒙皮矩阵，设置图形和骨骼的映射‘
	SetBoneMappings(model_->GetBoneMappings());
	// 更新下dirty
	UpdateTreeDirty();

	// 设置渲染Context
	renderContext_.geometryType_ = GEOMETRY_SKINNED;
	renderContext_.worldTransform_ = &skinMatrices_[0];
	renderContext_.numWorldTransform_ = skinMatrices_.Size();
	renderContext_.viewMask_ = GetViewMask();

	boneBoundingBox_ = boundingBox_;
}

void SkeletonMeshComponent::SetSkeleton(const Skeleton& skeleton)
{
	// 拷贝一份
	skeleton_ = skeleton;
	auto& bones = skeleton_.GetBones();
	// 骨骼Node缓存，避免再AddChild之前被释放
	Vector<SharedPtr<Node>> boneNodes;
	for (UInt32 i = 0; i < bones.Size(); ++i)
	{
		Bone& bone = bones[i];
		SharedPtr<Node> node(new Node());
		node->SetName(bone.name_);
		node->SetTransform(bone.initPosition_, bone.initRotation_, bone.initScale_);
		boneNodes.Push(node);
		bone.node_ = node;
	}

	for (UInt32 i = 0; i < bones.Size(); ++i)
	{
		Bone& bone = bones[i];
		UInt32 parentIndex = bone.parentIndex_;
		if (parentIndex != i && parentIndex < bones.Size())
		{
			bones[parentIndex].node_->AddChild(bone.node_);
		}
		else
		{
			node_->AddChild(bone.node_);
		}
	}
}

void SkeletonMeshComponent::SetBoneMappings(const Vector<PODVector<UInt32>>& boneMappings)
{
	skinMatrices_.Resize(skeleton_.GetBones().Size());
			
	geometrySkinMatrices_.Clear();
	geometrySkinMatrixPtrs_.Clear();
			
	if (!boneMappings.Size()) return;

	bool allEmpty = true;
	for (const auto& boneMapping : boneMappings)
	{
		if (boneMapping.Size())
		{
			allEmpty = false;
			break;
		}
	}

	if (allEmpty) return;

	geometrySkinMatrices_.Resize(boneMappings.Size());
	for (UInt32 i = 0; i < boneMappings.Size(); ++i)
	{
		geometrySkinMatrices_[i].Resize(boneMappings[i].Size());
	}

	geometrySkinMatrixPtrs_.Resize(skeleton_.GetBones().Size());
	for (UInt32 i = 0; i < boneMappings.Size(); ++i)
	{
		for (UInt32 j = 0; j < boneMappings[i].Size(); ++j)
		{
			geometrySkinMatrixPtrs_[boneMappings[i][j]].Push(&geometrySkinMatrices_[i][j]);
		}
	}
}

void SkeletonMeshComponent::UpdateSkinning()
{
	const auto& bones = skeleton_.GetBones();

	for (UInt32 i = 0; i < bones.Size(); ++i)
	{
		const Bone& bone = bones[i];
		if (bone.node_)
		{
			skinMatrices_[i] = bone.node_->GetWorldTransform() * bone.offsetMatrix;
		}
		else
		{
			skinMatrices_[i] = node_->GetWorldTransform();
		}

		if (geometrySkinMatrices_.Size())
		{
			for (UInt32 j = 0; j < geometrySkinMatrixPtrs_.Size(); ++j)
			{
				(*geometrySkinMatrixPtrs_[i][j]) = skinMatrices_[i];
			}
		}
	}

	skinningDirty_ = false;
}

void SkeletonMeshComponent::UpdateBoneBoundingBox()
{
	boneBoundingBox_.Clear();

	Matrix3x4 inverseTransform = node_->GetWorldTransform().Inverse();

	const auto& bones = skeleton_.GetBones();
	for (const auto& bone : bones)
	{
		if (bone.node_)
		{
			if (bone.collisionMask_ & BONE_COLLISSION_BOX)
				boneBoundingBox_.Merge(bone.boundingBox_.Transformed(inverseTransform * bone.node_->GetWorldTransform()));
			//if(bone.collisionMask_ & BONE_COLLISSION_SPHERE)
			//	boneBoundingBox_.Merge()
		}
	}

	boneBoundingBoxDirty_ = false;
	worldBoundingBoxDirty_ = true;
}

void SkeletonMeshComponent::UpdateTreeDirty()
{
	Component::UpdateTreeDirty();

	skinningDirty_ = true;
	boneBoundingBoxDirty_ = true;
}

void SkeletonMeshComponent::OnUpdateWorldBoundingBox()
{
	worldBoundingBox_ = boneBoundingBox_.Transformed(node_->GetWorldTransform());
}

}
