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

			Vector4(const Vector3& vector, float w) NOEXCEPT;

			Vector4(float x, float y, float z, float w) NOEXCEPT;

			explicit Vector4(const float* data) NOEXCEPT;

			Vector4& operator =(const Vector4& rhs) NOEXCEPT = default;

			bool operator ==(const Vector4& rhs) const;

			bool operator !=(const Vector4& rhs) const;

			Vector4 operator +(const Vector4& rhs) const;

			Vector4 operator -() const;

			Vector4 operator -(const Vector4& rhs) const;

			Vector4 operator *(float rhs) const;

			Vector4 operator *(const Vector4& rhs) const;

			Vector4 operator /(float rhs) const;

			Vector4 operator /(const Vector4& rhs) const;

			Vector4& operator +=(const Vector4& rhs);

			Vector4& operator -=(const Vector4& rhs);

			Vector4& operator *=(float rhs);

			Vector4& operator *=(const Vector4& rhs);

			Vector4& operator /=(float rhs);

			Vector4& operator /=(const Vector4& rhs);

			float operator[](unsigned index) const;

			float& operator[](unsigned index);

			float DotProduct(const Vector4& rhs) const;

			float AbsDotProduct(const Vector4& rhs) const;

			float ProjectOntoAxis(const Vector3& axis) const;

			Vector4 Abs() const;

			Vector4 Lerp(const Vector4& rhs, float t) const;

			bool Equals(const Vector4& rhs) const;

			bool IsNaN() const;

			const float* Data() const;

			Container::String ToString() const;

			unsigned ToHash() const;
				
			float x_;
			float y_;
			float z_;
			float w_;

			static const Vector4 ZERO;
			static const Vector4 ONE;
		};
	}
}
