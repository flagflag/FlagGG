#pragma once

#include "Export.h"
#include "Define.h"
#include "Math/Quaternion.h"
#include "Container/Str.h"

namespace FlagGG
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
		Real v00, Real v01, Real v02, Real v03,
		Real v10, Real v11, Real v12, Real v13,
		Real v20, Real v21, Real v22, Real v23) NOEXCEPT;

	explicit Matrix3x4(const Real* data) NOEXCEPT;

	Matrix3x4(const Vector3& translation, const Quaternion& rotation, Real scale) NOEXCEPT;

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

	Matrix3x4 operator *(Real rhs) const;

	Matrix3x4 operator *(const Matrix3x4& rhs) const;

	Matrix4 operator *(const Matrix4& rhs) const;

	void SetTranslation(const Vector3& translation);

	void SetRotation(const Matrix3& rotation);

	void SetScale(const Vector3& scale);

	void SetScale(Real scale);

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

	const Real* Data() const;

	Real Element(unsigned i, unsigned j) const;

	Vector4 Row(unsigned i) const;

	Vector3 Column(unsigned j) const;

	String ToString() const;

	Real m00_;
	Real m01_;
	Real m02_;
	Real m03_;
	Real m10_;
	Real m11_;
	Real m12_;
	Real m13_;
	Real m20_;
	Real m21_;
	Real m22_;
	Real m23_;

	static const Matrix3x4 ZERO;
	static const Matrix3x4 IDENTITY;
};

inline Matrix3x4 operator *(Real lhs, const Matrix3x4& rhs) { return rhs * lhs; }

}