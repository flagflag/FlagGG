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

			bool IsInterger(const Container::String& content)
			{
				for (uint32_t i = 0; i < content.Length(); ++i)
				{
					if (content[i] < '0' || content[i] > '9')
					{
						return false;
					}
				}
				return true;
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