#pragma once

#include "Container/Str.h"
#include "Container/Ptr.h"
#include "Graphics/Material.h"

namespace FlagGG
{

struct NamedEmitterMaterial
{
	String name_;

	SharedPtr<Material> material_;
};

}
