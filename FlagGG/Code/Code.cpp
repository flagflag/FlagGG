#include "Code.h"

#include <windows.h>
#include <memory>

namespace FlagGG
{
	namespace Code
	{
		std::wstring AnsiToWide(const std::string& ansi_string)
		{
			int count = MultiByteToWideChar(CP_ACP, 0, ansi_string.data(), ansi_string.length(), NULL, 0);
			std::wstring unicode_string(count + 1, '\0');
			MultiByteToWideChar(CP_ACP, 0, ansi_string.data(), ansi_string.length(), &unicode_string[0], count);
			return std::move(unicode_string);
		}

		std::wstring Utf8ToWide(const std::string& utf8_string)
		{
			int count = MultiByteToWideChar(CP_UTF8, 0, utf8_string.data(), utf8_string.length(), NULL, 0);
			std::wstring unicode_string(count + 1, '\0');
			MultiByteToWideChar(CP_UTF8, 0, utf8_string.data(), utf8_string.length(), &unicode_string[0], count);
			return std::move(unicode_string);
		}

		std::string WideToAnsi(const std::wstring& wide_string)
		{
			int count = WideCharToMultiByte(CP_ACP, 0, wide_string.data(), wide_string.length(), NULL, 0, NULL, NULL);
			std::string ansi_string(count + 1, '\0');
			WideCharToMultiByte(CP_ACP, 0, wide_string.data(), wide_string.length(), &ansi_string[0], count, NULL, NULL);
			return std::move(ansi_string);
		}

		std::string WideToUtf8(const std::wstring& wide_string)
		{
			int count = WideCharToMultiByte(CP_UTF8, 0, wide_string.data(), wide_string.length(), NULL, 0, NULL, NULL);
			std::string utf8_string(count + 1, '\0');
			WideCharToMultiByte(CP_UTF8, 0, wide_string.data(), wide_string.length(), &utf8_string[0], count, NULL, NULL);
			return std::move(utf8_string);
		}
	}
}