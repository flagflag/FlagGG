#ifndef __IO_CONNECTOR__
#define __IO_CONNECTOR__

#include "IOFrame/Buffer/IOBuffer.h"
#include "Core/BaseTypes.h"
#include "Export.h"

namespace FlagGG
{
	namespace IOFrame
	{
		namespace Connector
		{
			class FlagGG_API IOConnector : public Container::RefCounted
			{
			public:
				virtual ~IOConnector() = default;

				virtual bool Connect(const char* ip, UInt16 port) = 0;

				virtual bool Disconnect() = 0;

				virtual bool Write(Buffer::IOBufferPtr buffer) = 0;
			};

			typedef Container::SharedPtr < IOConnector > IOConnectorPtr;
		}
	}
}

#endif