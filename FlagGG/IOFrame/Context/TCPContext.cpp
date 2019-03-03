#include "TCPContext.h"

namespace FlagGG
{
	namespace IOFrame
	{
		namespace Context
		{
			TCPContext::TCPContext(Channel::IOChannelPtr channel)
				: m_channel(channel)
			{ }

			Channel::IOChannelPtr TCPContext::GetChannel()
			{
				return m_channel;
			}
		}
	}
}