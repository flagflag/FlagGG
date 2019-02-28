#ifndef __RESOURCE__
#define __RESOURCE__

#include <string>
#include "Export.h"

namespace FlagGG
{
	namespace Resource
	{
		class FlagGG_API Resource
		{
		public:
			virtual ~Resource() = default;

			virtual bool LoadFile(const std::wstring& fileName);
		};
	}
}

#endif
