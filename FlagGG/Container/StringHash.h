#pragma once
#include "Container/Str.h"
#include "Core/BaseTypes.h"

namespace FlagGG
{

class FlagGG_API StringHash
{
public:
	StringHash() noexcept;

	StringHash(const char* str) noexcept;

	StringHash(const String& str) noexcept;

	StringHash(UInt32 value) noexcept;

	StringHash(const StringHash& value) noexcept;

	StringHash& operator=(const StringHash& rhs) noexcept = default;

	StringHash operator+(const StringHash& rhs) const;

	StringHash& operator+=(const StringHash& rhs);

	bool operator==(const StringHash& rhs) const;

	bool operator!=(const StringHash& rhs) const;

	bool operator<(const StringHash& rhs) const;

	bool operator>(const StringHash& rhs) const;

	explicit operator bool() const;

	UInt32 ToHash() const;

	static const StringHash ZERO;

private:
	UInt32 value_;
};

}
