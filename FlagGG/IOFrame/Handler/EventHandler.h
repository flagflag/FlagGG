#ifndef __EVENT_HANDLER__
#define __EVENT_HANDLER__

#include "IOFrame\Context\IOContext.h"
#include "IOFrame\Buffer\Buffer.h"
#include "ErrorCode.h"

namespace FlagGG
{
	namespace IOFrame
	{
		namespace Handler
		{
			class EventHandler
			{
			public:
				virtual ~EventHandler() {};

				virtual void channelRegisterd(Context::IOContextPtr context) = 0;

				virtual void channelOpend(Context::IOContextPtr context) = 0;

				virtual void channelClosed(Context::IOContextPtr context) = 0;

				virtual void messageRecived(Context::IOContextPtr context, Buffer::IOBufferPtr buffer) = 0;

				virtual void errorCatch(Context::IOContextPtr context, const ErrorCode& error_code) = 0;
			};

			typedef std::shared_ptr < EventHandler > EventHandlerPtr;
		}
	}
}

#endif