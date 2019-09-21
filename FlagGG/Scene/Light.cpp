#include "Scene/Light.h"
#include "Scene/Node.h"

namespace FlagGG
{
	namespace Scene
	{
		Light::Light() :
			camera_(Graphics::AIRCRAFT),
			cameraDirty_(false)
		{ 
			camera_.SetNearClip(1.0f);
			camera_.SetFarClip(1000000000.0f);
		}

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
				camera_.SetPosition(node->GetWorldPosition());
				camera_.SetRotation(node->GetWorldRotation());

				cameraDirty_ = false;
			}
		}
	}
}
