#include "Scene/Light.h"
#include "Scene/Node.h"

namespace FlagGG
{
	namespace Scene
	{
		Light::Light() :
			camera_(Graphics::AIRCRAFT)
		{ }

		Graphics::Camera* Light::GetCamera()
		{
			return &camera_;
		}

		void Light::UpdateTreeDirty()
		{
			Node* node = GetNode();
			camera_.SetPosition(node->GetPosition());
			camera_.SetRotation(node->GetRotation());
		}
	}
}
