#include "Scene/Light.h"
#include "Scene/Node.h"

namespace FlagGG
{

Light::Light()
	: lightType_(LIGHT_TYPE_DIRECTIONAL)
	, brightness_(3.f)
	, range_(300.f)
{ 
	SetNearClip(1.0f);
	SetFarClip(1e5f);
}

}
