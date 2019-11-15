#pragma once

#include "IOContext.h"

namespace FlagGG
{
	namespace IOFrame
	{
		namespace Context
		{
			class UDPContext : public IOContext
			{
			public:
				UDPContext(Channel::IOChannelPtr channel);

				Channel::IOChannelPtr GetChannel() override;

			private:
				Channel::IOChannelPtr channel_;
			};

			typedef Container::SharedPtr < UDPContext > UDPContextPtr;
		}
	}
}
