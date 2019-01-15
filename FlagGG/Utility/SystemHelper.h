#ifndef __SYSTEM_HELPER__
#define __SYSTEM_HELPER__

#include "Export.h"

#include <string>

namespace FlagGG
{
	namespace Utility
	{
		namespace SystemHelper
		{
			std::wstring FlagGG_API FormatPath(const std::wstring& in_path);
		}
	}
}

#endif