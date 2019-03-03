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
			class FlagGG_API IOContext : public Container::RefCounted
			{
			public:
				virtual ~IOContext() = default;

				virtual Channel::IOChannelPtr GetChannel() = 0;
			};

			typedef Container::SharedPtr < IOContext > IOContextPtr;
		}
	}
}

#endif