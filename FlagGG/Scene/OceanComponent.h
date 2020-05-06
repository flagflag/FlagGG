#pragma once

#include "Export.h"
#include "Scene/DynamicMeshComponent.h"
#include "Container/Ptr.h"
#include "Graphics/Material.h"
#include "Math/Vector2.h"
#include "Math/Matrix3x4.h"
#include "Core/BaseTypes.h"

namespace FlagGG
{
	namespace Scene
	{
		class FlagGG_API OceanComponent : public DynamicMeshComponent
		{
			OBJECT_OVERRIDE(OceanComponent, DynamicMeshComponent);
		public:
		};
	}
}
