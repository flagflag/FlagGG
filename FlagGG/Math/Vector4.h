#pragma once

#include "Export.h"
#include "Define.h"
#include "Math/Vector3.h"
#include "Container/Str.h"

namespace FlagGG
{
	namespace Math
	{
		class FlagGG_API Vector4
		{
		public:
			Vector4() NOEXCEPT;

			Vector4(const Vector4& vector) NOEXCEPT = default;

			Vector4(const Vector3& vector, Real w) NOEXCEPT;

			Vector4(Real x, Real y, Real z, Real w) NOEXCEPT;

			explicit Vector4(const Real* data) NOEXCEPT;

			Vector4& operator =(const Vector4& rhs) NOEXCEPT = default;

			bool operator ==(const Vector4& rhs) const;

			bool operator !=(const Vector4& rhs) const;

			Vector4 operator +(const Vector4& rhs) const;

			Vector4 operator -() const;

			Vector4 operator -(const Vector4& rhs) const;

			Vector4 operator *(Real rhs) const;

			Vector4 operator *(const Vector4& rhs) const;

			Vector4 operator /(Real rhs) const;

			Vector4 operator /(const Vector4& rhs) const;

			Vector4& operator +=(const Vector4& rhs);

			Vector4& operator -=(const Vector4& rhs);

			Vector4& operator *=(Real rhs);

			Vector4& operator *=(const Vector4& rhs);

			Vector4& operator /=(Real rhs);

			Vector4& operator /=(const Vector4& rhs);

			Real operator[](unsigned index) const;

			Real& operator[](unsigned index);

			Real DotProduct(const Vector4& rhs) const;

			Real AbsDotProduct(const Vector4& rhs) const;

			Real ProjectOntoAxis(const Vector3& axis) const;

			Vector4 Abs() const;

			Vector4 Lerp(const Vector4& rhs, Real t) const;

			bool Equals(const Vector4& rhs) const;

			bool IsNaN() const;

			const Real* Data() const;

			Container::String ToString() const;

			unsigned ToHash() const;
				
			Real x_;
			Real y_;
			Real z_;
			Real w_;

			static const Vector4 ZERO;
			static const Vector4 ONE;
		};
	}
}
