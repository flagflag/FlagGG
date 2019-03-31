#include "SystemHelper.h"
#include "Container/Str.h"

#if WIN32 || WIN64
#include <windows.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#else
#include <unistd.h>
#include <limits.h>
#define MAX_PATH NAME_MAX
#endif

namespace FlagGG
{
	namespace Utility
	{
		namespace SystemHelper
		{
			std::wstring FormatPath(const std::wstring& in_path)
			{
				wchar_t format_path[MAX_PATH] = { 0 };
				int count = 0;
				const wchar_t match1 = wchar_t('\\');
				const wchar_t match2 = wchar_t('/');
				const wchar_t* start = in_path.c_str();
				const wchar_t* end = start + in_path.length();
				wchar_t last_char = '\0';

				const wchar_t* index = start;
				for (; index < end && ((*index) == match1) || (*index) == match2; ++index);

				for (; index < end; ++index)
				{
					wchar_t ch = *index;
					if (ch == match2) ch = match1;

					if (ch != match1 || ch != last_char)
					{
						format_path[count++] = ch;
					}

					last_char = ch;
				}

#if WIN32 || WIN64
				wchar_t out_path[MAX_PATH] = { 0 };
				::PathCanonicalizeW(out_path, format_path);
				return out_path;
#else
				return format_path;
#endif
			}

			void Sleep(uint64_t time)
			{
#if WIN32 || WIN64
				::Sleep(time);
#else
				usleep(time * 1000);
#endif
			}

			bool ParseCommand(const char** argv, uint32_t argc, Config::LJSONValue& result)
			{
				for (uint32_t i = 0; i < argc; ++i)
				{
					Container::String command = argv[i];
					if (command.Length() > 0 && command[0] == '-')
					{
						uint32_t pos = command.Find('=');
						if (pos != Container::String::NPOS)
						{
							const Container::String key = command.Substring(1, pos - 1);
							const Container::String value = command.Substring(pos + 1);
							result[key] = value;
						}
						else
						{
							result[command.Substring(1)] = true;
						}
					}
				}

				return true;
			}
		}
	}
}