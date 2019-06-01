#pragma once

#include "Export.h"
#include "Config/LJSONValue.h"
#include "Container/Vector.h"
#include "Container/Str.h"

namespace FlagGG
{
	namespace Config
	{
		FlagGG_API bool ParseStringVector(const LJSONValue& value, Container::Vector<Container::String>& result);
	}
}

