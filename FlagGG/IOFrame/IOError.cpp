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

			Container::String IOError::Message() const
			{
				return errorCode_.message().c_str();
			}
		}
	}
}