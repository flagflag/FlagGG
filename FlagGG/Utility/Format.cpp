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
				Size len = vsnprintf(nullptr, 0, format, va);

				Container::String buffer;
				buffer.Resize(len);

				vsnprintf(&buffer[0], len + 1, format, va);
				va_end(va);

				return buffer;
			}

			bool IsInterger(const Container::String& content)
			{
				for (UInt32 i = 0; i < content.Length(); ++i)
				{
					if (content[i] < '0' || content[i] > '9')
					{
						return false;
					}
				}
				return true;
			}

			Int32 ToInt(const Container::String& content)
			{
				Int32 result;
				sscanf(content.CString(), "%d", &result);
				return result;
			}

			UInt32 ToUInt(const Container::String& content)
			{
				UInt32 result;
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