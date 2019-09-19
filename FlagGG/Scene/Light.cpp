#include "Scene/Light.h"
#include "Scene/Node.h"

namespace FlagGG
{
	namespace Scene
	{
		Light::Light() :
			camera_(Graphics::AIRCRAFT),
			cameraDirty_(false)
		{ }

		Graphics::Camera* Light::GetCamera()
		{
			return &camera_;
		}

		void Light::UpdateTreeDirty()
		{
			cameraDirty_ = true;
		}

		void Light::Update(float timeStep)
		{
			if (cameraDirty_)
			{
				Node* node = GetNode();
				camera_.SetPosition(node->GetPosition());
				camera_.SetRotation(node->GetRotation());

				cameraDirty_ = false;
			}
		}
	}
}
