#ifndef __IO_CHNTEXT__
#define __IO_CHNTEXT__

#include "IOFrame/Channel/IOChannel.h"
#include "Export.h"

namespace FlagGG
{
	namespace IOFrame
	{
		namespace Context
		{
			class FlagGG_API IOContext
			{
			public:
				virtual ~IOContext() = default;

				virtual Channel::IOChannelPtr getChannel() = 0;
			};

			typedef std::shared_ptr < IOContext > IOContextPtr;
		}
	}
}

#endif