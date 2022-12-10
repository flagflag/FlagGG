#ifndef __MATRIX4__
#define __MATRIX4__

#include "Export.h"
#include "Define.h"
#include "Core/BaseTypes.h"

namespace FlagGG
{

class Vector3;
class Vector4;
class Matrix3x4;
class Matrix3;

class FlagGG_API Matrix4
{
public:
	Matrix4() NOEXCEPT;

	Matrix4(const Matrix3& matrix) NOEXCEPT;

	Matrix4(const Matrix4& matrix) NOEXCEPT;

	Matrix4(
		Real v00, Real v01, Real v02, Real v03,
		Real v10, Real v11, Real v12, Real v13,
		Real v20, Real v21, Real v22, Real v23,
		Real v30, Real v31, Real v32, Real v33) NOEXCEPT;

	explicit Matrix4(const Real* data) NOEXCEPT;

	Matrix4& operator =(const Matrix4& rhs);

	bool operator ==(const Matrix4& rhs) const;

	bool operator !=(const Matrix4& rhs) const;

	// 矩阵分量减rhs
	Matrix4 operator +(const Matrix4& rhs) const;

	// 矩阵减法
	Matrix4 operator -(const Matrix4& rhs) const;

	// 矩阵分量乘rhs
	Matrix4 operator *(Real rhs) const;

	// 矩阵乘法
	Matrix4 operator *(const Matrix4& rhs) const;

	Matrix4 operator *(const Matrix3x4& rhs) const;

	Vector3 operator *(const Vector3& rhs) const;

	Vector4 operator *(const Vector4& rhs) const;

	// 矩阵转置
	Matrix4 Transpose() const;

	// 逆矩阵
	Matrix4 Inverse() const;

	// 矩阵数据数组
	const Real* Data() const;

	// 矩阵元素 <=> [i][j]
	Real Element(unsigned i, unsigned j) const;

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
	Real m30_;
	Real m31_;
	Real m32_;
	Real m33_;

	// 批量转置
	static void BulkTranspose(Real* dest, const Real* src, unsigned count);

	static const Matrix4 ZERO;
	static const Matrix4 IDENTITY;
};

}

#endif