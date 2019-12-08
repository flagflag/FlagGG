#ifndef __MATRIX4__
#define __MATRIX4__

#include "Export.h"
#include "Define.h"

namespace FlagGG
{
	namespace Math
	{
		class Vector3;
		class Matrix3x4;

		/// 4x4 matrix for arbitrary linear transforms including projection.
		class FlagGG_API Matrix4
		{
		public:
			Matrix4() NOEXCEPT;

			Matrix4(const Matrix4& matrix) NOEXCEPT;

			Matrix4(
				float v00, float v01, float v02, float v03,
				float v10, float v11, float v12, float v13,
				float v20, float v21, float v22, float v23,
				float v30, float v31, float v32, float v33) NOEXCEPT;

			explicit Matrix4(const float* data) NOEXCEPT;

			Matrix4& operator =(const Matrix4& rhs);

			bool operator ==(const Matrix4& rhs) const;

			bool operator !=(const Matrix4& rhs) const;

			// 矩阵分量减rhs
			Matrix4 operator +(const Matrix4& rhs) const;

			// 矩阵减法
			Matrix4 operator -(const Matrix4& rhs) const;

			// 矩阵分量乘rhs
			Matrix4 operator *(float rhs) const;

			// 矩阵乘法
			Matrix4 operator *(const Matrix4& rhs) const;

			Matrix4 operator *(const Matrix3x4& rhs) const;

			Vector3 operator *(const Vector3& rhs) const;

			// 矩阵转置
			Matrix4 Transpose() const;

			// 逆矩阵
			Matrix4 Inverse() const;

			// 矩阵数据数组
			const float* Data() const;

			// 矩阵元素 <=> [i][j]
			float Element(unsigned i, unsigned j) const;

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
			float m30_;
			float m31_;
			float m32_;
			float m33_;

			// 批量转置
			static void BulkTranspose(float* dest, const float* src, unsigned count);

			/// Zero matrix.
			static const Matrix4 ZERO;
			/// Identity matrix.
			static const Matrix4 IDENTITY;
		};
	}
}

#endif