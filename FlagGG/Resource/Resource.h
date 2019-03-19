#ifndef __RESOURCE__
#define __RESOURCE__

#include <string>
#include "Export.h"
#include "Container/Str.h"
#include "IOFrame/Stream/FileStream.h"

namespace FlagGG
{
	namespace Resource
	{
		class FlagGG_API Resource
		{
		public:
			virtual ~Resource() = default;

			bool LoadFile(const Container::String& fileName);

		protected:
			virtual bool BeginLoad(IOFrame::Stream::FileStream& fileStream);

			virtual bool EndLoad();
		};
	}
}

#endif
