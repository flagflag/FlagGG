#pragma once

#include "Scene/StaticMeshComponent.h"

namespace FlagGG
{
	namespace Scene
	{
		class SkeletonMeshComponent : public StaticMeshComponent
		{
		public:
			void SetModel(Graphics::Model* model) override;
		};
	}
}
