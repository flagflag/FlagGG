#pragma once

#include "Export.h"
#include "Container/Str.h"
#include "Core/BaseTypes.h"

namespace FlagGG
{
	namespace Utility
	{
		namespace Format
		{
			FlagGG_API Container::String ToString(const char* format, ...);

			FlagGG_API bool IsInterger(const Container::String& content);

			FlagGG_API Int32 ToInt(const Container::String& content);

			FlagGG_API UInt32 ToUInt(const Container::String& content);

			FlagGG_API float ToFloat(const Container::String& content);

			FlagGG_API double ToDouble(const Container::String& content);
		}
	}
}

