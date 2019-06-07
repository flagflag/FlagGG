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
			OBJECT_OVERRIDE(SkeletonMeshComponent);
		public:
			SkeletonMeshComponent();

			void Update(float timeStep) override;

			void UpdateTreeDirty() override;

			Skeleton& GetSkeleton();

		protected:
			void OnModel() override;

			void SetSkeleton(const Skeleton& skeleton);

			void SetBoneMappings(const Container::Vector<Container::PODVector<uint32_t>>& boneMappings);

			void UpdateSkinning();

		private:
			bool skinningDirty_;
		
			Skeleton skeleton_;

			Container::PODVector<Math::Matrix3x4> skinMatrices_;

			Container::Vector<Container::PODVector<Math::Matrix3x4>> geometrySkinMatrices_;

			Container::Vector<Container::PODVector<Math::Matrix3x4*>> geometrySkinMatrixPtrs_;
		};
	}
}
