#include "SkeletonMeshComponent.h"

namespace FlagGG
{
	namespace Scene
	{
		void SkeletonMeshComponent::SetModel(Graphics::Model* model)
		{
			StaticMeshComponent::SetModel(model);

			const auto& boneMappings = model->GetBoneMappings();
		}
	}
}
