#include "Code.h"
#include "Core/GenericPlatform.h"

#if PLATFORM_WINDOWS
#include <windows.h>
#else
#include <iconv.h>
#include <wchar.h>
#endif
#include <memory>

namespace FlagGG
{

std::wstring AnsiToWide(const std::string& ansi_string)
{
#if PLATFORM_WINDOWS
	int count = MultiByteToWideChar(CP_ACP, 0, ansi_string.data(), ansi_string.length(), NULL, 0);
	std::wstring unicode_string(count + 1, '\0');
	MultiByteToWideChar(CP_ACP, 0, ansi_string.data(), ansi_string.length(), &unicode_string[0], count);
	return std::move(unicode_string);
#else
	return L"";
#endif
}

std::wstring Utf8ToWide(const std::string& utf8_string)
{
#if PLATFORM_WINDOWS
	int count = MultiByteToWideChar(CP_UTF8, 0, utf8_string.data(), utf8_string.length(), NULL, 0);
	std::wstring unicode_string(count + 1, '\0');
	MultiByteToWideChar(CP_UTF8, 0, utf8_string.data(), utf8_string.length(), &unicode_string[0], count);
	return std::move(unicode_string);
#else
	return L"";
#endif
}

std::string WideToAnsi(const std::wstring& wide_string)
{
#if PLATFORM_WINDOWS
	int count = WideCharToMultiByte(CP_ACP, 0, wide_string.data(), wide_string.length(), NULL, 0, NULL, NULL);
	std::string ansi_string(count + 1, '\0');
	WideCharToMultiByte(CP_ACP, 0, wide_string.data(), wide_string.length(), &ansi_string[0], count, NULL, NULL);
	return std::move(ansi_string);
#else
	return "";
#endif
}

std::string WideToUtf8(const std::wstring& wide_string)
{
#if PLATFORM_WINDOWS
	int count = WideCharToMultiByte(CP_UTF8, 0, wide_string.data(), wide_string.length(), NULL, 0, NULL, NULL);
	std::string utf8_string(count + 1, '\0');
	WideCharToMultiByte(CP_UTF8, 0, wide_string.data(), wide_string.length(), &utf8_string[0], count, NULL, NULL);
	return std::move(utf8_string);
#else
	return "";
#endif
}

}