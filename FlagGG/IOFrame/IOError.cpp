#include "IOError.h"

namespace FlagGG
{
	namespace IOFrame
	{
		namespace Error
		{
			IOError::IOError(boost::system::error_code error_code)
				: m_error_code(error_code)
			{ }

			int IOError::Value() const
			{
				return m_error_code.value();
			}

			std::string IOError::Message() const
			{
				return std::move(m_error_code.message());
			}
		}
	}
}