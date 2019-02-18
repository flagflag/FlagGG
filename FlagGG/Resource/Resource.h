#ifndef __RESOURCE__
#define __RESOURCE__

#include <string>

namespace FlagGG
{
	namespace Resource
	{
		class Resource
		{
		public:
			virtual ~Resource() = default;

			virtual bool LoadFile(const std::wstring& fileName);
		};
	}
}

#endif
