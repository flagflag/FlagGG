#ifndef __IO_ERROR__
#define __IO_ERROR__

#include <boost/asio/error.hpp>

#include "ErrorCode.h"
#include "IOFrame/Channel/IOChannel.h"

#define THROW_IO_ERROR(IOContextType, channel, handler, error_code) \
	{ \
		IOContextType##Ptr context(new IOContextType(channel)); \
		FlagGG::IOFrame::Error::IOError error(error_code); \
		handler->errorCatch(context, error); \
	}

namespace FlagGG
{
	namespace IOFrame
	{
		namespace Error
		{
			class IOError : public ErrorCode
			{
			public:
				IOError(boost::system::error_code error_code);

				virtual ~IOError() { }

				virtual int value() const override;

				virtual std::string message() const override;

			private:
				boost::system::error_code m_error_code;
			};
		}
	}
}

#endif