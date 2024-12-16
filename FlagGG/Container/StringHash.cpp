#include "Container/StringHash.h"
#include "Math/Math.h"

namespace FlagGG
{

const StringHash StringHash::ZERO(0u);

StringHash::StringHash() noexcept :
	value_(0)
{ }

StringHash::StringHash(const char* str) noexcept :
	value_(HashString(str))
{ }

StringHash::StringHash(const String& str) noexcept :
	value_(HashString(str.CString()))
{ }

StringHash::StringHash(UInt32 value) noexcept :
	value_(value)
{ }

StringHash::StringHash(const StringHash& value) noexcept :
	value_(value.value_)
{ }

StringHash StringHash::operator+(const StringHash& rhs) const
{
	StringHash ret;
	ret.value_ = value_ + rhs.value_;
	return ret;
}

StringHash& StringHash::operator+=(const StringHash& rhs)
{
	value_ += rhs.value_;
	return *this;
}

bool StringHash::operator==(const StringHash& rhs) const
{
	return value_ == rhs.value_;
}

bool StringHash::operator!=(const StringHash& rhs) const
{
	return value_ != rhs.value_;
}

bool StringHash::operator<(const StringHash& rhs) const
{
	return value_ < rhs.value_;
}

bool StringHash::operator>(const StringHash& rhs) const
{
	return value_ > rhs.value_;
}

StringHash::operator bool() const
{
	return value_ != 0;
}

UInt32 StringHash::ToHash() const
{
	return value_;
}

}
