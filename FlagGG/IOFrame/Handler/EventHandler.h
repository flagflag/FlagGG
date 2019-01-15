#ifndef __EVENT_HANDLER__
#define __EVENT_HANDLER__

#include "IOFrame\Context\IOContext.h"
#include "IOFrame\Buffer\IOBuffer.h"
#include "ErrorCode.h"
#include "Export.h"

namespace FlagGG
{
	namespace IOFrame
	{
		namespace Handler
		{
			class FlagGG_API EventHandler
			{
			public:
				virtual ~EventHandler() = default;

				virtual void channelRegisterd(Context::IOContextPtr context) = 0;

				virtual void channelOpend(Context::IOContextPtr context) = 0;

				virtual void channelClosed(Context::IOContextPtr context) = 0;

				virtual void messageRecived(Context::IOContextPtr context, Buffer::IOBufferPtr buffer) = 0;

				virtual void errorCatch(Context::IOContextPtr context, const ErrorCode& error_code) = 0;
			};

			typedef std::shared_ptr < EventHandler > EventHandlerPtr;

			class NullEventHandler : public EventHandler
			{
			public:
				~NullEventHandler() override = default;

				virtual void channelRegisterd(Context::IOContextPtr context) {}

				virtual void channelOpend(Context::IOContextPtr context) {}

				virtual void channelClosed(Context::IOContextPtr context) {}

				virtual void messageRecived(Context::IOContextPtr context, Buffer::IOBufferPtr buffer) {}

				virtual void errorCatch(Context::IOContextPtr context, const ErrorCode& error_code) {}
			};

			typedef std::shared_ptr < NullEventHandler > NullEventHandlerPtr;
		}
	}
}

#endif