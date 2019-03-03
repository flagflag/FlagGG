#ifndef __EVENT_HANDLER__
#define __EVENT_HANDLER__

#include "IOFrame/Context/IOContext.h"
#include "IOFrame/Buffer/IOBuffer.h"
#include "ErrorCode.h"
#include "Export.h"

namespace FlagGG
{
	namespace IOFrame
	{
		namespace Handler
		{
			class FlagGG_API EventHandler : public Container::RefCounted
			{
			public:
				virtual ~EventHandler() = default;

				virtual void ChannelRegisterd(Context::IOContextPtr context) = 0;

				virtual void ChannelOpend(Context::IOContextPtr context) = 0;

				virtual void ChannelClosed(Context::IOContextPtr context) = 0;

				virtual void MessageRecived(Context::IOContextPtr context, Buffer::IOBufferPtr buffer) = 0;

				virtual void ErrorCatch(Context::IOContextPtr context, const ErrorCode& error_code) = 0;
			};

			typedef Container::SharedPtr < EventHandler > EventHandlerPtr;

			class NullEventHandler : public EventHandler
			{
			public:
				~NullEventHandler() override = default;

				void ChannelRegisterd(Context::IOContextPtr context) override {}

				void ChannelOpend(Context::IOContextPtr context) override {}

				void ChannelClosed(Context::IOContextPtr context) override {}

				void MessageRecived(Context::IOContextPtr context, Buffer::IOBufferPtr buffer) override {}

				void ErrorCatch(Context::IOContextPtr context, const ErrorCode& error_code) override {}
			};

			typedef Container::SharedPtr < NullEventHandler > NullEventHandlerPtr;
		}
	}
}

#endif