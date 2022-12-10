#pragma once

#include "Export.h"
#include "Config/LJSONValue.h"
#include "Container/Vector.h"
#include "Container/Str.h"

namespace FlagGG
{

FlagGG_API bool ParseStringVector(const LJSONValue& value, Vector<String>& result);

}

