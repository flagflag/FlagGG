#include "SystemHelper.h"
#include "Container/Str.h"
#include "Format.h"

#include <ctime>
#include <chrono>
#if PLATFORM_WINDOWS
#include <windows.h>
#include <shlwapi.h>
#include <io.h>
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "winmm.lib")
#else
#if PLATFORM_APPLE
#include <mach-o/dyld.h>
#endif
#include <unistd.h>
#include <limits.h>
#include <dirent.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/param.h>
#include <stdio.h>
#include <dirent.h>
#include <fnmatch.h>
#include <string.h>
#include <stdlib.h>
#define MAX_PATH NAME_MAX
#endif

namespace FlagGG
{

#if PLATFORM_WINDOWS
const char* const PATH_SEPARATOR = "\\";
#else
const char* const PATH_SEPARATOR = "/";
#endif

String FormatPath(const String& path)
{
	WString wPath(path);
	UInt32 size = path.Length();

	wchar_t buffer[MAX_PATH] = { 0 };
	UInt32 cur = 0;
	bool lastMatch = false;
#if PLATFORM_WINDOWS
	char match1 = wchar_t('\\');
	char match2 = wchar_t('/');
#else
	char match1 = wchar_t('/');
	char match2 = wchar_t('\\');
#endif

	for (UInt32 i = 0; i < wPath.Length(); ++i)
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

	return std::move(String(buffer));
}

void Sleep(UInt64 time)
{
#if PLATFORM_WINDOWS
	::Sleep(time);
#else
	usleep(time * 1000);
#endif
}

void SleepNoStats(UInt64 time)
{
#if PLATFORM_WINDOWS
	if (time == 0)
		::SwitchToThread();
	else
		::Sleep(time);
#else
	if (time == 0)
		sched_yield();
	else
		usleep(time * 1000);
#endif
}

UInt32 Tick()
{
#if PLATFORM_WINDOWS
	return (UInt32)timeGetTime();
#else
	struct timeval time{};
	gettimeofday(&time, nullptr);
	return (UInt32)(time.tv_sec * 1000 + time.tv_usec / 1000);
#endif
}

UInt64 HiresTick()
{
#if PLATFORM_WINDOWS
	if (HiresTimer::IsSupported())
	{
		LARGE_INTEGER counter;
		QueryPerformanceCounter(&counter);
		return counter.QuadPart;
	}
	else
		return timeGetTime();
#else
	struct timeval time {};
	gettimeofday(&time, nullptr);
	return time.tv_sec * 1000000LL + time.tv_usec;
#endif
}

String GetTimeStamp()
{
	return GetTimeStamp("%Y-%m-%d %H_%M_%S");
}

String GetTimeStamp(const String& fmt)
{
	auto now = std::chrono::system_clock::now();

	time_t sysTime = std::chrono::system_clock::to_time_t(now);
	char dateTime[64];
	strftime(dateTime, sizeof(dateTime), fmt.CString(), localtime(&sysTime));

	auto duration = now.time_since_epoch();
	auto secs = std::chrono::duration_cast<std::chrono::seconds>(duration);
	auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() - secs.count() * 1000LL;
	return ToString("%s_%03lld", dateTime, msec);
}

bool ParseCommand(const char** argv, UInt32 argc,  LJSONValue& result)
{
	for (UInt32 i = 0; i < argc; ++i)
	{
		String command = argv[i];
		if (command.Length() > 0 && command[0] == '-')
		{
			UInt32 pos = command.Find('=');
			if (pos != String::NPOS)
			{
				const String key = command.Substring(1, pos - 1);
				const String value = command.Substring(pos + 1);
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

bool ParseCommand(const String& commandLine, LJSONValue& result)
{
	Vector<String> argv = commandLine.Split(' ');

	for (auto& command : argv)
	{
		if (command.Length() > 0 && command[0] == '-')
		{
			UInt32 pos = command.Find('=');
			if (pos != String::NPOS)
			{
				const String key = command.Substring(1, pos - 1);
				const String value = command.Substring(pos + 1);
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

bool HasAccess(const String& path)
{
	return true;
}

IntRect GetDesktopRect()
{
#if PLATFORM_WINDOWS
	RECT rect;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
	return IntRect(IntVector2(rect.left, rect.top), IntVector2(rect.right, rect.bottom));
#else
	return IntRect::ZERO;
#endif
}

static String GetInternalPath(const String& pathName)
{
	return pathName.Replaced('\\', '/');
}

static void SplitPath(const String& fullPath, String& pathName, String& fileName, String& extension, bool lowercaseExtension = true)
{
	String fullPathCopy = GetInternalPath(fullPath);

	unsigned extPos = fullPathCopy.FindLast('.');
	unsigned pathPos = fullPathCopy.FindLast('/');

	if (extPos != String::NPOS && (pathPos == String::NPOS || extPos > pathPos))
	{
		extension = fullPathCopy.Substring(extPos);
		if (lowercaseExtension)
			extension = extension.ToLower();
		fullPathCopy = fullPathCopy.Substring(0, extPos);
	}
	else
		extension.Clear();

	pathPos = fullPathCopy.FindLast('/');
	if (pathPos != String::NPOS)
	{
		fileName = fullPathCopy.Substring(pathPos + 1);
		pathName = fullPathCopy.Substring(0, pathPos + 1);
	}
	else
	{
		fileName = fullPathCopy;
		pathName.Clear();
	}
}

String GetPath(const String& fullPath)
{
	String path, file, extension;
	SplitPath(fullPath, path, file, extension);
	return path;
}

String GetFileName(const String& fullPath)
{
	String path, file, extension;
	SplitPath(fullPath, path, file, extension);
	return file;
}

String GetExtension(const String& fullPath, bool lowercaseExtension)
{
	String path, file, extension;
	SplitPath(fullPath, path, file, extension, lowercaseExtension);
	return extension;
}

String GetFileNameAndExtension(const String& fileName, bool lowercaseExtension)
{
	String path, file, extension;
	SplitPath(fileName, path, file, extension, lowercaseExtension);
	return file + extension;
}

String ReplaceExtension(const String& fullPath, const String& newExtension)
{
	String path, file, extension;
	SplitPath(fullPath, path, file, extension);
	return path + file + newExtension;
}

String GetProgramDir()
{
#if PLATFORM_WINDOWS
	wchar_t exeName[MAX_PATH];
	exeName[0] = 0;
	GetModuleFileNameW(nullptr, exeName, MAX_PATH);
	return GetPath(String(exeName));
#elif PLATFORM_APPLE
	char exeName[MAX_PATH];
	memset(exeName, 0, MAX_PATH);
	unsigned size = MAX_PATH;
	_NSGetExecutablePath(exeName, &size);
	return GetPath(String(exeName));
#elif PLATFORM_UNIX
	char exeName[MAX_PATH];
	memset(exeName, 0, MAX_PATH);
	pid_t pid = getpid();
	String link = "/proc/" + String(pid) + "/exe";
	readlink(link.CString(), exeName, MAX_PATH);
	return GetPath(String(exeName));
#else
	return "";
#endif
}

Timer::Timer()
{
	Reset();
}

UInt32 Timer::GetMilliSeconds(bool reset)
{
	UInt32 curTime = Tick();
	UInt32 deltaTime = curTime - startTime_;
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

bool HiresTimer::supported(false);
UInt64 HiresTimer::frequency(1000);

void HiresTimer::InitSupported()
{
#if PLATFORM_WINDOWS
	LARGE_INTEGER frequency;
	if (QueryPerformanceFrequency(&frequency))
	{
		HiresTimer::frequency = frequency.QuadPart;
		HiresTimer::supported = true;
	}
#else
	HiresTimer::frequency = 1000000;
	HiresTimer::supported = true;
#endif
}

HiresTimer::HiresTimer()
{
	Reset();
}

UInt64 HiresTimer::GetUSec(bool reset)
{
	UInt64 currentTime = HiresTick();
	UInt64 elapsedTime = currentTime - startTime_;

	if (elapsedTime < 0)
		elapsedTime = 0;

	if (reset)
		startTime_ = currentTime;

	return (elapsedTime * 1000000LL) / frequency;
}

void HiresTimer::Reset()
{
	startTime_ = HiresTick();
}

}