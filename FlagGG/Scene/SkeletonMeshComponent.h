#pragma once

#include "Export.h"
#include "Scene/StaticMeshComponent.h"
#include "Scene/Bone.h"
#include "Container/Vector.h"
#include "Math/Matrix3x4.h"

namespace FlagGG
{
	namespace Scene
	{
		class FlagGG_API SkeletonMeshComponent : public StaticMeshComponent
		{
			OBJECT_OVERRIDE(SkeletonMeshComponent, StaticMeshComponent);
		public:
			SkeletonMeshComponent();

			void Update(Real timeStep) override;

			void UpdateTreeDirty() override;

			void OnUpdateWorldBoundingBox() override;

			Skeleton& GetSkeleton();

		protected:
			void OnModel() override;

			void SetSkeleton(const Skeleton& skeleton);

			void SetBoneMappings(const Container::Vector<Container::PODVector<UInt32>>& boneMappings);

			void UpdateSkinning();

			void UpdateBoneBoundingBox();

		private:
			bool skinningDirty_;
			bool boneBoundingBoxDirty_;
		
			Skeleton skeleton_;

			Container::PODVector<Math::Matrix3x4> skinMatrices_;

			Container::Vector<Container::PODVector<Math::Matrix3x4>> geometrySkinMatrices_;

			Container::Vector<Container::PODVector<Math::Matrix3x4*>> geometrySkinMatrixPtrs_;

			Math::BoundingBox boneBoundingBox_;
		};
	}
}
