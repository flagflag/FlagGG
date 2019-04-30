#include "SystemHelper.h"
#include "Container/Str.h"

#if WIN32 || WIN64
#include <windows.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#else
#include <unistd.h>
#include <limits.h>
#include <dirent.h>
#define MAX_PATH NAME_MAX
#endif

namespace FlagGG
{
	namespace Utility
	{
		namespace SystemHelper
		{
			Container::String FormatPath(const Container::String& path)
			{
				Container::WString wPath(path);
				uint32_t size = path.Length();

				wchar_t buffer[MAX_PATH] = { 0 };
				uint32_t cur = 0;
				bool lastMatch = false;
				char match1 = wchar_t('\\');
				char match2 = wchar_t('/');

				for (uint32_t i = 0; i < wPath.Length(); ++i)
				{
					bool curMatch = wPath[i] == match1 || wPath[i] == match2;

					if (i == 0 && curMatch)
					{
						while (i < wPath.Length())
						{
							curMatch = wPath[i] == match1 || wPath[i] == match2;
							if (!curMatch)
							{
								break;
							}
							++i;
						}
					}

					if (!curMatch)
					{
						buffer[cur++] = wPath[i];
					}
					else if (!lastMatch)
					{
						buffer[cur++] = match1;
					}

					lastMatch = curMatch;
					}

				return std::move(Container::String(buffer));
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

			bool DirExists(const Container::String& path)
			{
#ifdef WIN32 || WIN64
				DWORD attributes = GetFileAttributesW(Container::WString(path).CString());
				if (attributes == INVALID_FILE_ATTRIBUTES || attributes & FILE_ATTRIBUTE_DIRECTORY)
					return false;
#else
				struct stat st{};
				if (stat(fixedName.CString(), &st) || st.st_mode & S_IFDIR)
					return false;
#endif
				return true;
			}

			bool FileExists(const Container::String& path)
			{
#ifdef WIN32 || WIN64
				DWORD attributes = GetFileAttributesW(Container::WString(path).CString());
				if (attributes == INVALID_FILE_ATTRIBUTES || !(attributes & FILE_ATTRIBUTE_DIRECTORY))
					return false;
#else
				struct stat st{};
				if (stat(fixedName.CString(), &st) || !(st.st_mode & S_IFDIR))
					return false;
#endif
				return true;
			}
		}
	}
}