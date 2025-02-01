#include "Ultralight/String32.h"
#include "Ultralight/String8.h"
#include "Ultralight/String16.h"

#include <Container/Str.h>
#include <Memory/Memory.h>

using namespace FlagGG;

namespace ultralight
{

String32::String32()
	: data_(nullptr)
	, length_(0u)
{

}

String32::String32(const char32_t* c_str, size_t len)
{
	length_ = len;
	data_ = new char32_t[length_ + 1];
	Memory::Memcpy(data_, c_str, length_ * sizeof(char32_t));
	data_[length_] = 0;
}

String32::String32(const String32& other)
	: String32(other.data_, other.length_)
{

}

String32::~String32()
{
	if (data_)
	{
		delete[] data_;
	}
}

String32& String32::operator=(const String32& other)
{
	if (data_)
	{
		delete[] data_;
	}

	length_ = other.length_;
	data_ = new char32_t[length_ + 1];
	Memory::Memcpy(data_, other.data_, length_ * sizeof(char32_t));
	data_[length_] = 0;

	return *this;
}

String32& String32::operator+=(const String32& other)
{
	char32_t* oldData = data_;
	size_t oldLength = length_;
	length_ += other.length_;
	data_ = new char32_t[length_ + 1];
	if (oldData)
	{
		Memory::Memcpy(data_, oldData, oldLength * sizeof(char32_t));
		delete[] oldData;
	}
	Memory::Memcpy(data_ + oldLength, other.data_, other.length_ * sizeof(char32_t));
	data_[length_] = 0;

	return *this;
}

String8 String32::utf8() const
{
	return String8();
}

String16 String32::utf16() const
{
	return String16();
}

}
