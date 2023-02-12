#include "Scene/Light.h"
#include "Scene/Node.h"

namespace FlagGG
{

Light::Light()
	: lightType_(LIGHT_TYPE_DIRECTIONAL)
{ 
	SetNearClip(1.0f);
	SetFarClip(1000000000.0f);
}

}
