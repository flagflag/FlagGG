#include "IOFrame/Context/UDPContext.h"

namespace FlagGG
{
	namespace IOFrame
	{
		namespace Context
		{
			UDPContext::UDPContext(Channel::IOChannelPtr channel) :
				channel_(channel)
			{}

			Channel::IOChannelPtr UDPContext::GetChannel()
			{
				return channel_;
			}
		}
	}
}
