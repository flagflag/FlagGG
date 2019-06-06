#pragma once
#include "Container/Str.h"

namespace FlagGG
{
	namespace Container
	{
		class FlagGG_API StringHash
		{
		public:
			StringHash() NOEXCEPT;

			StringHash(const char* str) NOEXCEPT;

			StringHash(const String& str) NOEXCEPT;

			StringHash(uint32_t value) NOEXCEPT;

			StringHash(const StringHash& value) NOEXCEPT;

			StringHash& operator=(const StringHash& rhs) NOEXCEPT = default;

			StringHash operator+(const StringHash& rhs) const;

			StringHash& operator+=(const StringHash& rhs);

			bool operator==(const StringHash& rhs) const;

			bool operator!=(const StringHash& rhs) const;

			bool operator<(const StringHash& rhs) const;

			bool operator>(const StringHash& rhs) const;

			explicit operator bool() const;

			uint32_t ToHash() const;

			static const StringHash ZERO;

		private:
			uint32_t value_;
		};
	}
}
