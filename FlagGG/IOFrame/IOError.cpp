#include "IOError.h"

namespace FlagGG
{
	namespace IOFrame
	{
		namespace Error
		{
#ifndef FLAGGG_NO_BOOST
			IOError::IOError(boost::system::error_code error_code)
				: errorCode_(error_code)
			{ }

			int IOError::Value() const
			{
				return errorCode_.value();
			}

			String IOError::Message() const
			{
				return errorCode_.message().c_str();
			}
#endif

			CustomIOError::CustomIOError(int value, const String& message) :
				value_(value),
				message_(message)
			{}

			int CustomIOError::Value() const
			{
				return value_;
			}

			String CustomIOError::Message() const
			{
				return message_;
			}
		}
	}
}