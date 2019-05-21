#pragma once

#include "Export.h"
#include "Define.h"
#include "Math/Matrix3.h"
#include "Math/Vector3.h"
#include "Container/Str.h"

namespace FlagGG
{
	namespace Math
	{
		class FlagGG_API Quaternion
		{
		public:
			Quaternion() NOEXCEPT;

			Quaternion(const Quaternion& quat) NOEXCEPT;

			Quaternion(float w, float x, float y, float z) NOEXCEPT;

			explicit Quaternion(const float* data) NOEXCEPT;

			Quaternion(float angle, const Vector3& axis) NOEXCEPT;

			explicit Quaternion(float angle) NOEXCEPT;

			Quaternion(float x, float y, float z) NOEXCEPT;

			Quaternion(const Vector3& start, const Vector3& end) NOEXCEPT;

			Quaternion(const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis) NOEXCEPT;

			explicit Quaternion(const Matrix3& matrix) NOEXCEPT;

			Quaternion& operator =(const Quaternion& rhs) NOEXCEPT;
			Quaternion& operator +=(const Quaternion& rhs);

			Quaternion& operator *=(float rhs);

			bool operator ==(const Quaternion& rhs) const;

			bool operator !=(const Quaternion& rhs) const;

			Quaternion operator *(float rhs) const;

			Quaternion operator -() const;

			Quaternion operator +(const Quaternion& rhs) const;

			Quaternion operator -(const Quaternion& rhs) const;

			Quaternion operator *(const Quaternion& rhs) const;

			Vector3 operator *(const Vector3& rhs) const;

			void FromAngleAxis(float angle, const Vector3& axis);

			void FromEulerAngles(float x, float y, float z);

			void FromRotationTo(const Vector3& start, const Vector3& end);

			void FromAxes(const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis);

			void FromRotationMatrix(const Matrix3& matrix);
			
			bool FromLookRotation(const Vector3& direction, const Vector3& up = Vector3::UP);

			void Normalize();

			Quaternion Normalized() const;

			Quaternion Inverse() const;

			float LengthSquared() const;

			float DotProduct(const Quaternion& rhs) const;

			bool Equals(const Quaternion& rhs) const;

			bool IsNaN() const;

			Quaternion Conjugate() const;

			Vector3 EulerAngles() const;

			float YawAngle() const;

			float PitchAngle() const;

			float RollAngle() const;

			Vector3 Axis() const;

			float Angle() const;

			Matrix3 RotationMatrix() const;

			Quaternion Slerp(const Quaternion& rhs, float t) const;

			Quaternion Nlerp(const Quaternion& rhs, float t, bool shortestPath = false) const;

			const float* Data() const { return &w_; }

			Container::String ToString() const;

			float w_;
			float x_;
			float y_;
			float z_;

			static const Quaternion IDENTITY;
		};
	}
}
