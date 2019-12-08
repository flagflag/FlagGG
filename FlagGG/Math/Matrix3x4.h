#pragma once

#include "Export.h"
#include "Define.h"
#include "Math/Quaternion.h"
#include "Container/Str.h"

namespace FlagGG
{
	namespace Math
	{
		class Matrix3;
		class Matrix3x4;
		class Matrix4;
		class Vector3;
		class Vector4;

		class FlagGG_API Matrix3x4
		{
		public:
			Matrix3x4() NOEXCEPT;

			Matrix3x4(const Matrix3x4& matrix) NOEXCEPT = default;

			explicit Matrix3x4(const Matrix3& matrix) NOEXCEPT;

			explicit Matrix3x4(const Matrix4& matrix) NOEXCEPT;

			Matrix3x4(
				float v00, float v01, float v02, float v03,
				float v10, float v11, float v12, float v13,
				float v20, float v21, float v22, float v23) NOEXCEPT;

			explicit Matrix3x4(const float* data) NOEXCEPT;

			Matrix3x4(const Vector3& translation, const Quaternion& rotation, float scale) NOEXCEPT;

			Matrix3x4(const Vector3& translation, const Quaternion& rotation, const Vector3& scale) NOEXCEPT;

			Matrix3x4(const Quaternion& rotation) NOEXCEPT;

			Matrix3x4& operator =(const Matrix3x4& rhs) NOEXCEPT = default;

			Matrix3x4& operator =(const Matrix3& rhs) NOEXCEPT;

			Matrix3x4& operator =(const Matrix4& rhs) NOEXCEPT;

			bool operator ==(const Matrix3x4& rhs) const;

			bool operator !=(const Matrix3x4& rhs) const;

			Vector3 operator *(const Vector3& rhs) const;

			Vector3 operator *(const Vector4& rhs) const;

			Matrix3x4 operator +(const Matrix3x4& rhs) const;

			Matrix3x4 operator -(const Matrix3x4& rhs) const;

			Matrix3x4 operator *(float rhs) const;

			Matrix3x4 operator *(const Matrix3x4& rhs) const;

			Matrix4 operator *(const Matrix4& rhs) const;

			void SetTranslation(const Vector3& translation);

			void SetRotation(const Matrix3& rotation);

			void SetScale(const Vector3& scale);

			void SetScale(float scale);

			Matrix3 ToMatrix3() const;

			Matrix4 ToMatrix4() const;

			Matrix3 RotationMatrix() const;

			Vector3 Translation() const;

			Quaternion Rotation() const;

			Vector3 Scale() const;

			Vector3 SignedScale(const Matrix3& rotation) const;

			bool Equals(const Matrix3x4& rhs) const;

			void Decompose(Vector3& translation, Quaternion& rotation, Vector3& scale) const;

			Matrix3x4 Inverse() const;

			const float* Data() const;

			float Element(unsigned i, unsigned j) const;

			Vector4 Row(unsigned i) const;

			Vector3 Column(unsigned j) const;

			Container::String ToString() const;

			float m00_;
			float m01_;
			float m02_;
			float m03_;
			float m10_;
			float m11_;
			float m12_;
			float m13_;
			float m20_;
			float m21_;
			float m22_;
			float m23_;

			static const Matrix3x4 ZERO;
			static const Matrix3x4 IDENTITY;
		};

		/// Multiply a 3x4 matrix with a scalar.
		inline Matrix3x4 operator *(float lhs, const Matrix3x4& rhs) { return rhs * lhs; }

	}
}