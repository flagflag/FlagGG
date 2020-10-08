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
	namespace Utility
	{
		namespace SystemHelper
		{
			extern const char* const PATH_SEPARATOR;

			FlagGG_API Container::String FormatPath(const Container::String& path);

			// 单位：毫秒
			FlagGG_API void Sleep(uint64_t time);

			// 单位：毫秒
			FlagGG_API UInt32 Tick();

			FlagGG_API UInt64 HiresTick();

			FlagGG_API bool ParseCommand(const char** argv, UInt32 argc, Config::LJSONValue& result);

			FlagGG_API bool DirExists(const Container::String& path);

			FlagGG_API bool FileExists(const Container::String& path);

			FlagGG_API bool CreateDir(const Container::String& path);

			FlagGG_API bool HasAccess(const Container::String& path);

			enum FindFilesMode
			{
				FindFilesMode_Dir = 1,
				FindFilesMode_File = 2,
			};

			FlagGG_API void FindFiles(const Container::String& dirPath, const Container::String& pattern, bool recursive, int fileMode, Container::Vector<Container::String>& fileNames);

			FlagGG_API Math::IntRect GetDesktopRect();

			FlagGG_API Container::String GetPath(const Container::String& fullPath);

			FlagGG_API Container::String GetFileName(const Container::String& fullPath);

			FlagGG_API Container::String GetExtension(const Container::String& fullPath, bool lowercaseExtension);

			FlagGG_API Container::String GetFileNameAndExtension(const Container::String& fileName, bool lowercaseExtension);

			FlagGG_API Container::String ReplaceExtension(const Container::String& fullPath, const Container::String& newExtension);

			FlagGG_API Container::String GetProgramDir();

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
				friend class FlagGG::Core::Profiler;

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
	}
}

#endif