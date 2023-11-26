#ifndef __SYSTEM_HELPER__
#define __SYSTEM_HELPER__

#include "Export.h"
#include "Config/LJSONValue.h"
#include "Container/Str.h"
#include "Math/Rect.h"

#include <string>

namespace FlagGG
{
	namespace Core
	{
		class Profiler;
	}
}

namespace FlagGG
{

extern const char* const PATH_SEPARATOR;

FlagGG_API String FormatPath(const String& path);

// 单位：毫秒
FlagGG_API void Sleep(uint64_t time);

// 单位：毫秒
FlagGG_API UInt32 Tick();

FlagGG_API UInt64 HiresTick();

FlagGG_API String GetTimeStamp();

FlagGG_API String GetTimeStamp(const String& fmt);

FlagGG_API bool ParseCommand(const char** argv, UInt32 argc, LJSONValue& result);

FlagGG_API bool DirExists(const String& path);

FlagGG_API bool FileExists(const String& path);

FlagGG_API bool CreateDir(const String& path);

FlagGG_API bool HasAccess(const String& path);

FlagGG_API bool Copy(const String& source, const String& target);

enum FindFilesMode
{
	FindFilesMode_Dir = 1,
	FindFilesMode_File = 2,
};

FlagGG_API void FindFiles(const String& dirPath, const String& pattern, bool recursive, int fileMode, Vector<String>& fileNames);

FlagGG_API IntRect GetDesktopRect();

FlagGG_API String GetPath(const String& fullPath);

FlagGG_API String GetFileName(const String& fullPath);

FlagGG_API String GetExtension(const String& fullPath, bool lowercaseExtension = false);

FlagGG_API String GetFileNameAndExtension(const String& fileName, bool lowercaseExtension = false);

FlagGG_API String ReplaceExtension(const String& fullPath, const String& newExtension);

FlagGG_API String GetProgramDir();

class FlagGG_API Timer
{
public:
	Timer();

	UInt32 GetMilliSeconds(bool reset);

	void Reset();

private:
	UInt32 startTime_;
};

class FlagGG_API HiresTimer
{
	friend class Profiler;

public:
	HiresTimer();

	UInt64 GetUSec(bool reset);
	void Reset();

	static void InitSupported();

	static bool IsSupported() { return supported; }

	static UInt64 GetFrequency() { return frequency; }

private:
	UInt64 startTime_{};

	static bool supported;
	static UInt64 frequency;
};

}

#endif