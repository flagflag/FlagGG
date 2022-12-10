#pragma once

#include "Export.h"
#include "Define.h"
#include "Math/Vector3.h"
#include "Container/Str.h"

namespace FlagGG
{

class FlagGG_API Matrix3
{
public:
	Matrix3() NOEXCEPT;

	Matrix3(const Matrix3& matrix) NOEXCEPT = default;

	Matrix3(
		Real v00, Real v01, Real v02,
		Real v10, Real v11, Real v12,
		Real v20, Real v21, Real v22);

	explicit Matrix3(const Real* data) NOEXCEPT;

	Matrix3& operator =(const Matrix3& rhs) NOEXCEPT = default;

	bool operator ==(const Matrix3& rhs) const;

	bool operator !=(const Matrix3& rhs) const;

	Vector3 operator *(const Vector3& rhs) const;

	Matrix3 operator +(const Matrix3& rhs) const;

	Matrix3 operator -(const Matrix3& rhs) const;

	Matrix3 operator *(Real rhs) const;

	Matrix3 operator *(const Matrix3& rhs) const;

	void SetScale(const Vector3& scale);

	void SetScale(Real scale);

	Vector3 Scale() const;

	Vector3 SignedScale(const Matrix3& rotation) const;

	Matrix3 Transpose() const;

	Matrix3 Scaled(const Vector3& scale) const;

	bool Equals(const Matrix3& rhs) const;

	Matrix3 Inverse() const;

	const Real* Data() const { return &m00_; }

	Real Element(unsigned i, unsigned j) const { return Data()[i * 3 + j]; }

	Vector3 Row(unsigned i) const { return Vector3(Element(i, 0), Element(i, 1), Element(i, 2)); }

	Vector3 Column(unsigned j) const { return Vector3(Element(0, j), Element(1, j), Element(2, j)); }

	String ToString() const;

	Real m00_;
	Real m01_;
	Real m02_;
	Real m10_;
	Real m11_;
	Real m12_;
	Real m20_;
	Real m21_;
	Real m22_;

	static void BulkTranspose(Real* dest, const Real* src, unsigned count)
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

inline Matrix3 operator *(Real lhs, const Matrix3& rhs) { return rhs * lhs; }

}
