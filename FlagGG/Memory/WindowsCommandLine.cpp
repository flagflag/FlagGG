#include "WindowsCommandLine.h"
#include "Core/BaseTypes.h"

#include <string.h>
#include <windows.h>

namespace FlagGG
{

static Int32 Find(const char* str, Int32 strLen, const char* target, bool caseSensitive)
{
	Int32 targetLen = strlen(target);

	if (!targetLen || targetLen > strLen)
		return -1;

	char first = target[0];
	if (!caseSensitive)
		first = (char)::tolower(first);

	for (Int32 i = 0; i <= strLen - targetLen; ++i)
	{
		char c = str[i];
		if (!caseSensitive)
			c = (char)::tolower(c);

		if (c == first)
		{
			Int32 skip = -1;
			bool found = true;
			for (Int32 j = 1; j < targetLen; ++j)
			{
				c = str[i + j];
				char d = target[j];
				if (!caseSensitive)
				{
					c = (char)::tolower(c);
					d = (char)::tolower(d);
				}

				if (skip == -1 && c == first)
					skip = i + j - 1;

				if (c != d)
				{
					found = false;
					if (skip != -1)
						i = skip;
					break;
				}
			}
			if (found)
				return i;
		}
	}

	return -1;
}

bool CommandLineHas(const char* key)
{
#if PLATFORM_WINDOWS
	const char* commandLine = ::GetCommandLine();
	Int32 commandLineLength = strlen(commandLine);
	while (commandLineLength > 0)
	{
		const Int32 pos = Find(commandLine, commandLineLength, " ", false);

		if (pos == -1)
		{
			return memcmp(commandLine, key, commandLineLength) == 0;
		}
		else
		{
			if (memcmp(commandLine, key, pos) == 0)
				return true;

			commandLine += (pos + 1);
			commandLineLength -= (pos + 1);
		}
	}
#endif
	return false;
}

}
