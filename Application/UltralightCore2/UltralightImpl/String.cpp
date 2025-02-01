#include "Ultralight/String.h"

namespace ultralight
{

String::String()
{

}

String::String(const char* str)
	: str_(str)
{
	
}

String::String(const char* str, size_t len)
	: str_(str, len)
{

}

String::String(const String8& str)
	: str_(str)
{

}

String::String(const Char16* str, size_t len)
{
	String16 str16(str, len);
	str_ = std::move(str16.utf8());
}

String::String(const String16& str)
{
	String16 str16(str);
	str_ = std::move(str16.utf8());
}

String::String(const String32& str)
{
	String32 str32(str);
	str_ = std::move(str32.utf8());
}

String::String(const String& other)
{
	str_ = other.str_;
}

String::~String()
{

}

String& String::operator=(const String& other)
{
	str_ = other.str_;
	return *this;
}

String& String::operator+=(const String& other)
{
	str_ += other.str_;
	return *this;
}

String16 String::utf16() const
{
	return str_.utf16();
}

String32 String::utf32() const
{
	return str_.utf32();
}

}
