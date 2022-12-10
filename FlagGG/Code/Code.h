#ifndef __CODE__
#define __CODE__

#include "Export.h"

#include <string>

namespace FlagGG
{

std::wstring FlagGG_API AnsiToWide(const std::string& ansi_string);

std::wstring FlagGG_API Utf8ToWide(const std::string& utf8_string);

std::string FlagGG_API WideToAnsi(const std::wstring& wide_string);

std::string FlagGG_API WideToUtf8(const std::wstring& wide_string);

}

#endif