#include "SystemHelper.h"
#include "Container/Str.h"

#if _WIN32
#include <windows.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "winmm.lib")
#else
#include <unistd.h>
#include <limits.h>
#include <dirent.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <errno.h>
#define MAX_PATH NAME_MAX
#endif

namespace FlagGG
{
	namespace Utility
	{
		namespace SystemHelper
		{
#ifdef _WIN32
			const char* const PATH_SEPARATOR = "\\";
#else
			const char* const PATH_SEPARATOR = "/";
#endif

			Container::String FormatPath(const Container::String& path)
			{
				Container::WString wPath(path);
				uint32_t size = path.Length();

				wchar_t buffer[MAX_PATH] = { 0 };
				uint32_t cur = 0;
				bool lastMatch = false;
#ifdef _WIN32
				char match1 = wchar_t('\\');
				char match2 = wchar_t('/');
#else
				char match1 = wchar_t('/');
				char match2 = wchar_t('\\');
#endif

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
#if _WIN32
				::Sleep(time);
#else
				usleep(time * 1000);
#endif
			}

			uint32_t Tick()
			{
#if _WIN32
				return (uint32_t)timeGetTime();
#else
				struct timeval time{};
				gettimeofday(&time, nullptr);
				return (uint32_t)(time.tv_sec * 1000 + time.tv_usec / 1000);
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
#ifdef _WIN32
				DWORD attributes = GetFileAttributesW(Container::WString(path).CString());
				if (attributes == INVALID_FILE_ATTRIBUTES || attributes & FILE_ATTRIBUTE_DIRECTORY)
					return false;
#else
				struct stat st{};
				if (stat(path.CString(), &st) || st.st_mode & S_IFDIR)
					return false;
#endif
				return true;
			}

			bool FileExists(const Container::String& path)
			{
#ifdef _WIN32
				DWORD attributes = GetFileAttributesW(Container::WString(path).CString());
				if (attributes == INVALID_FILE_ATTRIBUTES || (attributes & FILE_ATTRIBUTE_DIRECTORY))
					return false;
#else
				struct stat st{};
				if (stat(path.CString(), &st) || !(st.st_mode & S_IFDIR))
					return false;
#endif
				return true;
			}

			bool CreateDir(const Container::String& path)
			{
#ifdef _WIN32
				Container::WString wPath(path);
				bool success = CreateDirectoryW(wPath.CString(), nullptr) == TRUE || GetLastError() == ERROR_ALREADY_EXISTS;
#else
				bool success = mkdir(path.CString(), S_IRWXU) == 0 || errno == EEXIST;
#endif
				return success;
			}

			bool HasAccess(const Container::String& path)
			{
				return true;
			}

			Math::IntRect GetDesktopRect()
			{
#if _WIN32
				RECT rect;
				SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
				return Math::IntRect(Math::IntVector2(rect.left, rect.top), Math::IntVector2(rect.right, rect.bottom));
#else
				return Mach::IntRect::ZERO;
#endif
			}


			Timer::Timer()
			{
				Reset();
			}

			uint32_t Timer::GetMilliSeconds(bool reset)
			{
				uint32_t curTime = Tick();
				uint32_t deltaTime = curTime - startTime_;
				if (reset)
				{
					startTime_ = curTime;
				}
				return deltaTime;
			}

			void Timer::Reset()
			{
				startTime_ = Tick();
			}
		}
	}
}