#include "Format.h"

#include <stdarg.h>

namespace FlagGG
{

String ToString(const char* format, ...)
{
	va_list va;
	va_start(va, format);
	Size len = vsnprintf(nullptr, 0, format, va);

	String buffer;
	buffer.Resize(len);

	vsnprintf(&buffer[0], len + 1, format, va);
	va_end(va);

	return buffer;
}

bool IsInterger(const String& content)
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

Int32 ToInt(const String& content)
{
	Int32 result;
	sscanf(content.CString(), "%d", &result);
	return result;
}

UInt32 ToUInt(const String& content)
{
	UInt32 result;
	sscanf(content.CString(), "%u", &result);
	return result;
}

float ToFloat(const String& content)
{
	float result;
	sscanf(content.CString(), "%f", &result);
	return result;
}

double ToDouble(const String& content)
{
	double result;
	sscanf(content.CString(), "%lf", &result);
	return result;
}

}