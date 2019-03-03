#ifndef __ERROR_CODE__
#define __ERROR_CODE__

#include "Export.h"

#include <string>

namespace FlagGG
{
	class FlagGG_API ErrorCode
	{
	public:
		virtual ~ErrorCode() {};

		virtual int Value() const = 0;

		virtual std::string Message() const = 0;
	};
}

#endif