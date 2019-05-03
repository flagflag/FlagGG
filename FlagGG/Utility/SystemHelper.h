#ifndef __SYSTEM_HELPER__
#define __SYSTEM_HELPER__

#include "Export.h"
#include "Config/LJSONValue.h"
#include "Container/Str.h"

#include <stdint.h>
#include <string>

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
			FlagGG_API uint32_t Tick();

			FlagGG_API bool ParseCommand(const char** argv, uint32_t argc, Config::LJSONValue& result);

			FlagGG_API bool DirExists(const Container::String& path);

			FlagGG_API bool FileExists(const Container::String& path);

			FlagGG_API bool HasAccess(const Container::String& path);

			class FlagGG_API Timer
			{
			public:
				Timer();

				uint32_t GetMilliSeconds(bool reset);

				void Reset();

			private:
				uint32_t startTime_;
			};
		}
	}
}

#endif