#ifndef __ERROR_CODE__
#define __ERROR_CODE__

#include <string>

namespace FlagGG
{
	class ErrorCode
	{
	public:
		virtual ~ErrorCode() {};

		virtual int value() const = 0;

		virtual std::string message() const = 0;
	};
}

#endif