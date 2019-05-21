#pragma once

#include "Export.h"
#include "Define.h"
#include "Math/Vector3.h"
#include "Container/Str.h"

namespace FlagGG
{
	namespace Math
	{
		class FlagGG_API Matrix3
		{
		public:
			Matrix3() NOEXCEPT;

			Matrix3(const Matrix3& matrix) NOEXCEPT = default;

			Matrix3(
				float v00, float v01, float v02,
				float v10, float v11, float v12,
				float v20, float v21, float v22);

			explicit Matrix3(const float* data) NOEXCEPT;

			Matrix3& operator =(const Matrix3& rhs) NOEXCEPT = default;

			bool operator ==(const Matrix3& rhs) const;

			bool operator !=(const Matrix3& rhs) const;

			Vector3 operator *(const Vector3& rhs) const;

			Matrix3 operator +(const Matrix3& rhs) const;

			Matrix3 operator -(const Matrix3& rhs) const;

			Matrix3 operator *(float rhs) const;

			Matrix3 operator *(const Matrix3& rhs) const;

			void SetScale(const Vector3& scale);

			void SetScale(float scale);

			Vector3 Scale() const;

			Vector3 SignedScale(const Matrix3& rotation) const;

			Matrix3 Transpose() const;

			Matrix3 Scaled(const Vector3& scale) const;

			bool Equals(const Matrix3& rhs) const;

			Matrix3 Inverse() const;

			const float* Data() const { return &m00_; }

			float Element(unsigned i, unsigned j) const { return Data()[i * 3 + j]; }

			Vector3 Row(unsigned i) const { return Vector3(Element(i, 0), Element(i, 1), Element(i, 2)); }

			Vector3 Column(unsigned j) const { return Vector3(Element(0, j), Element(1, j), Element(2, j)); }

			Container::String ToString() const;

			float m00_;
			float m01_;
			float m02_;
			float m10_;
			float m11_;
			float m12_;
			float m20_;
			float m21_;
			float m22_;

			static void BulkTranspose(float* dest, const float* src, unsigned count)
			{
				for (unsigned i = 0; i < count; ++i)
				{
					dest[0] = src[0];
					dest[1] = src[3];
					dest[2] = src[6];
					dest[3] = src[1];
					dest[4] = src[4];
					dest[5] = src[7];
					dest[6] = src[2];
					dest[7] = src[5];
					dest[8] = src[8];

					dest += 9;
					src += 9;
				}
			}

			static const Matrix3 ZERO;
			static const Matrix3 IDENTITY;
		};

		inline Matrix3 operator *(float lhs, const Matrix3& rhs) { return rhs * lhs; }
	}
}
