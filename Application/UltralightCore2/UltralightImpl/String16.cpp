#include "Ultralight/String16.h"
#include "Ultralight/String8.h"
#include "Ultralight/String32.h"
#include "Ultralight/private/util/RefCountedImpl.h"

#include <Container/Str.h>
#include <Container/Vector.h>
#include <Memory/Memory.h>

using namespace FlagGG;

namespace ultralight
{

String16::String16()
	: data_(nullptr)
	, length_(0u)
{

}

String16::String16(const Char16* str, size_t len)
{
	length_ = len;
	data_ = new Char16[length_ + 1];
	Memory::Memcpy(data_, str, length_ * sizeof(Char16));
	data_[length_] = 0;
}

String16::String16(const unsigned short* str, size_t len)
{
	length_ = len;
	data_ = new Char16[length_ + 1];
	Memory::Memcpy(data_, str, length_ * sizeof(Char16));
	data_[length_] = 0;
}

String16::String16(const String16& other)
	: String16(other.data_, other.length_)
{

}

String16::~String16()
{
	if (data_)
	{
		delete[] data_;
	}
}

String16& String16::operator=(const String16& other)
{
	if (data_)
	{
		delete[] data_;
	}

	length_ = other.length_;
	data_ = new Char16[length_ + 1];
	Memory::Memcpy(data_, other.data_, length_ * sizeof(Char16));
	data_[length_] = 0;

	return *this;
}

String16& String16::operator+=(const String16& other)
{
	Char16* oldData = data_;
	size_t oldLength = length_;
	length_ += other.length_;
	data_ = new Char16[length_ + 1];
	if (oldData)
	{
		Memory::Memcpy(data_, oldData, oldLength * sizeof(Char16));
		delete[] oldData;
	}
	Memory::Memcpy(data_ + oldLength, other.data_, other.length_ * sizeof(Char16));
	data_[length_] = 0;

	return *this;
}

String8 String16::utf8() const
{
	const wchar_t* str = data_;
	char temp[7];

	String8 str8;

	while (*str)
	{
		unsigned unicodeChar = FlagGG::String::DecodeUTF16(str);
		char* dest = temp;
		FlagGG::String::EncodeUTF8(dest, unicodeChar);
		*dest = 0;
		str8 += String8(temp);
	}

	return str8;
}

String32 String16::utf32() const
{
	return String32();
}


class String16VectorImpl : public String16Vector, public RefCountedImpl<String16VectorImpl>
{
public:
	REF_COUNTED_IMPL(String16VectorImpl);

	String16VectorImpl()
	{

	}

	String16VectorImpl(const String16* stringArray, size_t len)
		: strVec_(stringArray, len)
	{

	}

	~String16VectorImpl() override
	{

	}

	// Add an element to the back of the string vector
	virtual void push_back(const String16& val) override
	{
		strVec_.Push(val);
	}

	// Get raw String16 vector array
	virtual String16* data() override
	{
		return strVec_.Buffer();
	}

	// Get the number of elements in vector
	virtual size_t size() const override
	{
		return strVec_.Size();
	}

private:
	Vector<String16> strVec_;
};

String16Vector::String16Vector() = default;

String16Vector::~String16Vector() = default;

String16Vector::String16Vector(const String16Vector&)
{

}

void String16Vector::operator=(const String16Vector&)
{

}

RefPtr<String16Vector> String16Vector::Create()
{
	return RefPtr<String16Vector>(new String16VectorImpl());
}

RefPtr<String16Vector> String16Vector::Create(const String16* stringArray, size_t len)
{
	return RefPtr<String16Vector>(new String16VectorImpl(stringArray, len));
}

}
