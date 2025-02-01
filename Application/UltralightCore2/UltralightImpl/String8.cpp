#include "Ultralight/String8.h"
#include "Ultralight/String16.h"
#include "Ultralight/String32.h"

#include <string.h>
#include <Container/Str.h>
#include <Memory/Memory.h>

using namespace FlagGG;

namespace ultralight
{

String8::String8()
	: data_(nullptr)
	, length_(0u)
{

}

String8::String8(const char* c_str)
{
	length_ = ::strlen(c_str);
	data_ = new char[length_ + 1];
	Memory::Memcpy(data_, c_str, length_);
	data_[length_] = '\0';
}

String8::String8(const char* c_str, size_t len)
{
	length_ = len;
	data_ = new char[length_ + 1];
	Memory::Memcpy(data_, c_str, length_);
	data_[length_] = '\0';
}

String8::String8(const String8& other)
	: String8(other.data_, other.length_)
{

}

String8::~String8()
{
	if (data_)
	{
		delete[] data_;
	}
}

String8& String8::operator=(const String8& other)
{
	if (data_)
	{
		delete[] data_;
	}
	
	length_ = other.length_;
	data_ = new char[length_ + 1];
	Memory::Memcpy(data_, other.data_, length_);
	data_[length_] = '\0';

	return *this;
}

String8& String8:: operator+=(const String8& other)
{
	char* oldData = data_;
	size_t oldLength = length_;
	length_ += other.length_;
	data_ = new char[length_ + 1];
	if (oldData)
	{
		Memory::Memcpy(data_, oldData, oldLength);
		delete[] oldData;
	}
	Memory::Memcpy(data_ + oldLength, other.data_, other.length_);
	data_[length_] = '\0';

	return *this;
}

String16 String8::utf16() const
{
	unsigned neededSize = 0;
	unsigned byteOffset = 0;
	wchar_t temp[3];

	while (byteOffset < length_)
	{
		wchar_t* dest = temp;

		const char* src = data_ + byteOffset;
		unsigned ret = FlagGG::String::DecodeUTF8(src);
		byteOffset = (unsigned)(src - data_);

		FlagGG::String::EncodeUTF16(dest, ret);
		neededSize += dest - temp;
	}

	wchar_t* c_str16 = new wchar_t[neededSize + 1];
	byteOffset = 0;
	wchar_t* dest = c_str16;
	while (byteOffset < length_)
	{
		const char* src = data_ + byteOffset;
		unsigned ret = FlagGG::String::DecodeUTF8(src);
		byteOffset = (unsigned)(src - data_);

		String::EncodeUTF16(dest, ret);
	}
	c_str16[neededSize] = 0;

	String16 str16(c_str16, neededSize);

	delete[] c_str16;

	return str16;
}

String32 String8::utf32() const
{
	return String32();
}

}
