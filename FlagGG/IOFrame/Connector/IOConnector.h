#ifndef __IO_CONNECTOR__
#define __IO_CONNECTOR__

#include "IOFrame/Buffer/IOBuffer.h"
#include "Export.h"

#include <memory>
#include <stdint.h>

namespace FlagGG
{
	namespace IOFrame
	{
		namespace Connector
		{
			class FlagGG_API IOConnector
			{
			public:
				virtual ~IOConnector() = default;

				virtual bool connect(const char* ip, uint16_t port) = 0;

				virtual bool write(Buffer::IOBufferPtr buffer) = 0;
			};

			typedef std::shared_ptr < IOConnector > IOConnectorPtr;
		}
	}
}

#endif