#pragma once

#include <type_traits>

namespace FlagGG
{
	namespace Container
	{
		/// Type trait which enables Enum to be used as FlagSet template parameter. Bitwise operators (| & ^ ~) over enabled Enum will result in FlagSet<Enum>.
		template <typename T> struct IsFlagSet
		{
			/*constexpr*/const static bool value_ = false;
		};

		/// Enable enum for using in FlagSet. Shall be called within Urho3D namespace.
#define FLAGGG_ENABLE_FLAGSET(enumName) \
	template<> struct IsFlagSet<enumName> { /*constexpr*/const static bool value_ = true; } \

		/// Enable enum for using in FlagSet and declare FlagSet specialization. Shall be called within Urho3D namespace.
#define FLAGGG_FLAGSET(enumName, flagsetName) \
	FLAGGG_ENABLE_FLAGSET(enumName); \
	using flagsetName = FlagSet<enumName>

		/// A set of flags defined by an Enum.
		template <class E, class = typename std::enable_if<IsFlagSet<E>::value_>::type>
		class FlagSet
		{
		public:
			/// Enum type
			using Enum = E;
			/// Integer type
			using Integer = typename std::underlying_type<Enum>::type;

		public:
			/// Ctor by integer
			explicit FlagSet(Integer value)
				: value_(value)
			{
			}
			/// Empty constructor.
			FlagSet() = default;

			/// Copy constructor.
			FlagSet(const FlagSet& another) = default;

			/// Construct from Enum value.
			FlagSet(const Enum value)
				: value_(static_cast<Integer>(value))
			{
			}

			/// Assignment operator from flagset.
			FlagSet& operator = (const FlagSet& rhs) = default;

			/// Bitwise AND against Enum value.
			FlagSet& operator &= (const Enum value)
			{
				value_ &= static_cast<Integer>(value);
				return *this;
			}

			/// Bitwise AND against flagset value.
			FlagSet& operator &= (const FlagSet value)
			{
				value_ &= value.value_;
				return *this;
			}

			/// Bitwise OR against Enum value.
			FlagSet& operator |= (const Enum value)
			{
				value_ |= static_cast<Integer>(value);
				return *this;
			}

			/// Bitwise OR against flagset value.
			FlagSet& operator |= (const FlagSet value)
			{
				value_ |= value.value_;
				return *this;
			}

			/// Bitwise XOR against Enum value.
			FlagSet& operator ^= (const Enum value)
			{
				value_ ^= static_cast<Integer>(value);
				return *this;
			}

			/// Bitwise XOR against flagset value.
			FlagSet& operator ^= (const FlagSet value)
			{
				value_ ^= value.value_;
				return *this;
			}

			/// Bitwise AND against Enum value.
			FlagSet operator & (const Enum value) const
			{
				return FlagSet(value_ & static_cast<Integer>(value));
			}

			/// Bitwise AND against flagset value.
			FlagSet operator & (const FlagSet value) const
			{
				return FlagSet(value_ & value.value_);
			}

			/// Bitwise OR against Enum value.
			FlagSet operator | (const Enum value) const
			{
				return FlagSet(value_ | static_cast<Integer>(value));
			}

			/// Bitwise OR against flagset value.
			FlagSet operator | (const FlagSet value) const
			{
				return FlagSet(value_ | value.value_);
			}

			/// Bitwise XOR against Enum value.
			FlagSet operator ^ (const Enum value) const
			{
				return FlagSet(value_ ^ static_cast<Integer>(value));
			}

			/// Bitwise XOR against flagset value.
			FlagSet operator ^ (const FlagSet value) const
			{
				return FlagSet(value_ ^ value.value_);
			}

			/// Bitwise negation.
			FlagSet operator ~ () const
			{
				return FlagSet(~value_);
			}

			/// Boolean negation.
			bool operator ! () const
			{
				return !value_;
			}

			/// Returns true if any flag is set.
			operator bool() const
			{
				return value_ != 0;
			}

			/// Cast to underlying type of enum.
			operator Integer() const
			{
				return value_;
			}

			/// Cast to enum value.
			explicit operator Enum() const
			{
				return static_cast<Enum>(value_);
			}

			/// Cast to double. Used by Lua bindings.
			explicit operator double() const
			{
				return static_cast<double>(value_);
			}

			/// Equality check against enum value.
			bool operator ==(Enum rhs) const
			{
				return value_ == static_cast<Integer>(rhs);
			}

			/// Equality check against another flagset.
			bool operator ==(FlagSet rhs) const
			{
				return value_ == rhs.value_;
			}

			/// Non-equality check against enum value.
			bool operator !=(Enum rhs) const
			{
				return !(*this == rhs);
			}

			/// Non-equality check against another flagset value.
			bool operator !=(FlagSet rhs) const
			{
				return !(*this == rhs);
			}

			/// Return true if specified enum value is set.
			inline bool Test(const Enum value) const
			{
				return Test(static_cast<Integer>(value));
			}

			/// Return true if specified bits are set.
			inline bool Test(const Integer flags) const
			{
				return (value_ & flags) == flags && (flags != 0 || value_ == flags);
			}

			/// Return underlying integer (constant).
			Integer AsInteger() const { return value_; }

			/// Return underlying integer (non-constant).
			Integer& AsInteger() { return value_; }

		protected:
			/// Value
			Integer value_ = 0;
		};

		/// Bitwise Operator OR for against Enum values
		template <class Enum, class = typename std::enable_if<IsFlagSet<Enum>::value_>::type>
		FlagSet<Enum> operator |(const Enum lhs, const Enum rhs)
		{
			return FlagSet<Enum>(lhs) | rhs;
		}

		/// Bitwise Operator AND for against Enum values
		template <class Enum, class = typename std::enable_if<IsFlagSet<Enum>::value_>::type>
		FlagSet<Enum> operator & (const Enum lhs, const Enum rhs)
		{
			return FlagSet<Enum>(lhs) & rhs;
		}

		/// Bitwise Operator XOR for against Enum values
		template <class Enum, class = typename std::enable_if<IsFlagSet<Enum>::value_>::type>
		FlagSet<Enum> operator ^ (const Enum lhs, const Enum rhs)
		{
			return FlagSet<Enum>(lhs) ^ rhs;
		}

		/// Bitwise Operator INVERSION for against Enum values
		template <class Enum, class = typename std::enable_if<IsFlagSet<Enum>::value_>::type>
		FlagSet<Enum> operator ~ (const Enum rhs)
		{
			return ~FlagSet<Enum>(rhs);
		}
	}
}

