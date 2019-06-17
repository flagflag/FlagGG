#pragma once

#include "Export.h"
#include "Container/Str.h"

#include <stdint.h>

namespace FlagGG
{
	namespace Utility
	{
		namespace Format
		{
			FlagGG_API Container::String ToString(const char* format, ...);

			FlagGG_API bool IsInterger(const Container::String& content);

			FlagGG_API int32_t ToInt(const Container::String& content);

			FlagGG_API uint32_t ToUInt(const Container::String& content);

			FlagGG_API float ToFloat(const Container::String& content);

			FlagGG_API double ToDouble(const Container::String& content);
		}
	}
}

