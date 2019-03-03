#include "TCPContext.h"

namespace FlagGG
{
	namespace IOFrame
	{
		namespace Context
		{
			TCPContext::TCPContext(Channel::IOChannelPtr channel)
				: channel_(channel)
			{ }

			Channel::IOChannelPtr TCPContext::GetChannel()
			{
				return channel_;
			}
		}
	}
}