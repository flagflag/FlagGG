#ifndef __IO_CHNTEXT__
#define __IO_CHNTEXT__

#include "IOFrame\Channel\IOChannel.h"

namespace FlagGG
{
	namespace IOFrame
	{
		namespace Context
		{
			class IOContext
			{
			public:
				virtual ~IOContext() {}

				virtual Channel::IOChannelPtr getChannel() = 0;
			};

			typedef std::shared_ptr < IOContext > IOContextPtr;
		}
	}
}

#endif