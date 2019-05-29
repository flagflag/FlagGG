#include "SkeletonMeshComponent.h"

namespace FlagGG
{
	namespace Scene
	{
		void SkeletonMeshComponent::SetModel(Graphics::Model* model)
		{
			if (model_ == model)
			{
				return;
			}

			StaticMeshComponent::SetModel(model);

			const auto& boneMappings = model->GetBoneMappings();

		}
	}
}
