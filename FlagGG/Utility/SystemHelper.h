#ifndef __SYSTEM_HELPER__
#define __SYSTEM_HELPER__

#include "Export.h"

#include <stdint.h>
#include <string>

namespace FlagGG
{
	namespace Utility
	{
		namespace SystemHelper
		{
			std::wstring FlagGG_API FormatPath(const std::wstring& in_path);

			// 单位：毫秒
			void FlagGG_API Sleep(uint64_t time);
		}
	}
}

#endif