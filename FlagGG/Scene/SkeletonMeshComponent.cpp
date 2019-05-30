#include "Scene/SkeletonMeshComponent.h"
#include "Scene/Node.h"

namespace FlagGG
{
	namespace Scene
	{
		SkeletonMeshComponent::SkeletonMeshComponent() :
			skinningDirty_(true)
		{

		}

		void SkeletonMeshComponent::Update(float timeStep)
		{
			StaticMeshComponent::Update(timeStep);

			if (skinningDirty_)
			{
				UpdateSkinning();
			}
		}

		void SkeletonMeshComponent::Render(Graphics::RenderContext& renderContext)
		{
			StaticMeshComponent::Render(renderContext);
			renderContext.geometryType_ = GEOMETRY_SKINNED;
			renderContext.worldTransform_ = &skinMatrices_[0];
			renderContext.numWorldTransform_ = skinMatrices_.Size();
		}

		void SkeletonMeshComponent::SetModel(Graphics::Model* model)
		{
			if (model_ == model)
			{
				return;
			}

			StaticMeshComponent::SetModel(model);

			// 设置骨骼
			SetSkeleton(model->GetSkeleton());
			// 分配蒙皮矩阵，设置图形和骨骼的映射‘
			SetBoneMappings(model->GetBoneMappings());
			// 更新下dirty
			UpdateTreeDirty();
		}

		void SkeletonMeshComponent::SetSkeleton(const Skeleton& skeleton)
		{
			// 拷贝一份
			skeleton_ = skeleton;
			auto& bones = skeleton_.GetBones();
			// 骨骼Node缓存，避免再AddChild之前被释放
			Container::Vector<Container::SharedPtr<Node>> boneNodes;
			for (uint32_t i = 0; i < bones.Size(); ++i)
			{
				Bone& bone = bones[i];
				Container::SharedPtr<Node> node(new Node());
				node->SetTransform(bone.initPosition_, bone.initRotation_, bone.initScale_);
				boneNodes.Push(node);
				bone.node_ = node;
			}

			for (uint32_t i = 0; i < bones.Size(); ++i)
			{
				Bone& bone = bones[i];
				uint32_t parentIndex = bone.parentIndex_;
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

		void SkeletonMeshComponent::SetBoneMappings(const Container::Vector<Container::PODVector<uint32_t>>& boneMappings)
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
			for (uint32_t i = 0; i < boneMappings.Size(); ++i)
			{
				geometrySkinMatrices_[i].Resize(boneMappings[i].Size());
			}

			geometrySkinMatrixPtrs_.Resize(skeleton_.GetBones().Size());
			for (uint32_t i = 0; i < boneMappings.Size(); ++i)
			{
				for (uint32_t j = 0; j < boneMappings[i].Size(); ++j)
				{
					geometrySkinMatrixPtrs_[boneMappings[i][j]].Push(&geometrySkinMatrices_[i][j]);
				}
			}
		}

		void SkeletonMeshComponent::UpdateSkinning()
		{
			const auto& bones = skeleton_.GetBones();

			for (uint32_t i = 0; i < bones.Size(); ++i)
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
					for (uint32_t j = 0; j < geometrySkinMatrixPtrs_.Size(); ++j)
					{
						(*geometrySkinMatrixPtrs_[i][j]) = skinMatrices_[i];
					}
				}
			}

			skinningDirty_ = false;
		}

		void SkeletonMeshComponent::UpdateTreeDirty()
		{
			skinningDirty_ = true;
		}
	}
}
