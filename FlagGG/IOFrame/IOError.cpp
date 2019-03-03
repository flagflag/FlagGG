#include "IOError.h"

namespace FlagGG
{
	namespace IOFrame
	{
		namespace Error
		{
			IOError::IOError(boost::system::error_code error_code)
				: errorCode_(error_code)
			{ }

			int IOError::Value() const
			{
				return errorCode_.value();
			}

			std::string IOError::Message() const
			{
				return std::move(errorCode_.message());
			}
		}
	}
}