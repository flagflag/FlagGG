#include "IOError.h"

namespace FlagGG
{
	namespace IOFrame
	{
		namespace Error
		{
			TCPError::TCPError(boost::system::error_code error_code)
				: m_error_code(error_code)
			{ }

			int TCPError::value() const
			{
				return m_error_code.value();
			}

			std::string TCPError::message() const
			{
				return std::move(m_error_code.message());
			}
		}
	}
}