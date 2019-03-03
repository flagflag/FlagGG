#ifndef __IOCHANNEL__
#define __IOCHANNEL__

#include "IOFrame/Buffer/IOBuffer.h"
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

				virtual bool Write(Buffer::IOBufferPtr buffer) = 0;

				virtual bool Flush() = 0;

				virtual bool Connect(const char* ip, uint16_t port) = 0;

				virtual void Close() = 0;

				virtual void Shutdown() = 0;

				virtual bool IsConnected() = 0;

				virtual bool IsClosed() = 0;
			};

			typedef std::shared_ptr < IOChannel > IOChannelPtr;
		}
	}
}

#endif