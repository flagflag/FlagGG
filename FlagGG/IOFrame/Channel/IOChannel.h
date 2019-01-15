#ifndef __IOCHANNEL__
#define __IOCHANNEL__

#include "IOFrame\Buffer\IOBuffer.h"
#include "Export.h"

namespace FlagGG
{
	namespace IOFrame
	{
		namespace Channel
		{
			class FlagGG_API IOChannel : public std::enable_shared_from_this < IOChannel >
			{
			public:
				virtual ~IOChannel() {};

				virtual bool write(Buffer::IOBufferPtr buffer) = 0;

				virtual bool flush() = 0;

				virtual bool connect(const char* ip, uint16_t port) = 0;

				virtual void close() = 0;

				virtual void shutdown() = 0;

				virtual bool isConnected() = 0;

				virtual bool isClosed() = 0;
			};

			typedef std::shared_ptr < IOChannel > IOChannelPtr;
		}
	}
}

#endif