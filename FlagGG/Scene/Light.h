#pragma once

#include "Export.h"
#include "Scene/Component.h"
#include "Graphics/Material.h"
#include "Graphics/Camera.h"
#include "Core/Context.h"

namespace FlagGG
{
	namespace Scene
	{
		class FlagGG_API Light : public Component
		{
			OBJECT_OVERRIDE(Light);
		public:
			Light();

			Graphics::Camera* GetCamera();

			void UpdateTreeDirty() override;

			void Update(float timeStep) override;

		private:
			Graphics::Camera camera_;

			bool cameraDirty_;
		};
	}
}