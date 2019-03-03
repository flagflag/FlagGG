#include "Format.h"

#include <stdarg.h>

namespace FlagGG
{
	namespace Utility
	{
		namespace Format
		{
			Container::String ToString(const char* format, ...)
			{
				va_list va;
				va_start(va, format);
				size_t len = vsnprintf(nullptr, 0, format, va);

				Container::String buffer;
				buffer.Resize(len);

				vsnprintf(&buffer[0], len, format, va);
				va_end(va);

				return buffer;
			}

			int32_t ToInt(const Container::String& content)
			{
				int32_t result;
				sscanf(content.CString(), "%d", &result);
				return result;
			}

			uint32_t ToUInt(const Container::String& content)
			{
				uint32_t result;
				sscanf(content.CString(), "%u", &result);
				return result;
			}

			float ToFloat(const Container::String& content)
			{
				float result;
				sscanf(content.CString(), "%f", &result);
				return result;
			}

			double ToDouble(const Container::String& content)
			{
				double result;
				sscanf(content.CString(), "%lf", &result);
				return result;
			}
		}
	}
}