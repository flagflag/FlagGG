#ifndef __SYSTEM_HELPER__
#define __SYSTEM_HELPER__

#include "Export.h"
#include "Config/LJSONValue.h"

#include <stdint.h>
#include <string>

namespace FlagGG
{
	namespace Utility
	{
		namespace SystemHelper
		{
			FlagGG_API std::wstring FormatPath(const std::wstring& in_path);

			// 单位：毫秒
			FlagGG_API void Sleep(uint64_t time);

			FlagGG_API bool ParseCommand(const char** argv, uint32_t argc, Config::LJSONValue& result);
		}
	}
}

#endif