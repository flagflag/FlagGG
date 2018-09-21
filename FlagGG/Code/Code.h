#ifndef __CODE__
#define __CODE__

#include <string>

namespace FlagGG
{
	namespace Code
	{
		std::wstring AnsiToWide(const std::string& ansi_string);

		std::wstring Ut8ToWide(const std::string& utf8_string);

		std::string WideToAnsi(const std::wstring wide_string);

		std::string WideToUtf8(const std::wstring wide_string);
	}
}

#endif