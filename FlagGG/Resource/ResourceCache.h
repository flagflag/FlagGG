#ifndef __RESOURCE_CACHE__
#define __RESOURCE_CACHE__

#include "Container/Str.h"

namespace FlagGG
{
	namespace Resource
	{
		class ResourceCahce
		{
		public:
			template < class T >
			SharedPtr<T> GetResource(const Container::String& path);

		private:
		};
	}
}

#endif
