#include "Container/StringHash.h"
#include "Math/Math.h"

namespace FlagGG
{
	namespace Container
	{
		const StringHash StringHash::ZERO(0u);

		StringHash::StringHash() :
			value_(0)
		{ }

		StringHash::StringHash(const char* str) :
			value_(Math::HashString(str))
		{ }

		StringHash::StringHash(const String& str) :
			value_(Math::HashString(str.CString()))
		{ }

		StringHash::StringHash(uint32_t value) :
			value_(value)
		{ }

		StringHash::StringHash(const StringHash& value) :
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

		uint32_t StringHash::ToHash() const
		{
			return value_;
		}
	}
}
