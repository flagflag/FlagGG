#ifndef __IO_ERROR__
#define __IO_ERROR__

#include <boost\asio\error.hpp>

#include "ErrorCode.h"

namespace FlagGG
{
	namespace IOFrame
	{
		namespace Error
		{
			class TCPError : public ErrorCode
			{
			public:
				TCPError(boost::system::error_code error_code);

				virtual ~TCPError() { }

				virtual int value() const override;

				virtual std::string message() const override;

			private:
				boost::system::error_code m_error_code;
			};
		}
	}
}

#endif