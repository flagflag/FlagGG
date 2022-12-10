#pragma once

#include "Export.h"
#include "Scene/Component.h"
#include "Graphics/Material.h"
#include "Scene/Camera.h"
#include "Core/Context.h"

namespace FlagGG
{

class FlagGG_API Light : public Camera
{
	OBJECT_OVERRIDE(Light, Camera);
public:
	Light();
};

}
