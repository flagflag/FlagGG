#pragma once

#include "Export.h"
#include "Container/Str.h"
#include "Core/BaseTypes.h"

namespace FlagGG
{

FlagGG_API String ToString(const char* format, ...);

FlagGG_API bool IsInterger(const String& content);

FlagGG_API Int32 ToInt(const String& content);

FlagGG_API UInt32 ToUInt(const String& content);

FlagGG_API float ToFloat(const String& content);

FlagGG_API double ToDouble(const String& content);

}

