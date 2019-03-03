#ifndef __TCP_CONTEXT__
#define __TCP_CONTEXT__

#include "IOContext.h"

namespace FlagGG
{
	namespace IOFrame
	{
		namespace Context
		{
			class TCPContext : public IOContext
			{
			public:
				TCPContext(Channel::IOChannelPtr channel);

				~TCPContext() override = default;

				Channel::IOChannelPtr GetChannel() override;

			private:
				Channel::IOChannelPtr channel_;
			};

			typedef Container::SharedPtr < TCPContext > TCPContextPtr;
		}
	}
}

#endif