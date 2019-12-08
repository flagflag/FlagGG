#include "Matrix4.h"
#include "Math/Vector3.h"
#include "Math/Matrix3x4.h"

#include "Math/Math.h"

namespace FlagGG
{
	namespace Math
	{
		const Matrix4 Matrix4::ZERO(
			0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f);

		const Matrix4 Matrix4::IDENTITY;

		Matrix4::Matrix4() :
			m00_(1.0f),
			m01_(0.0f),
			m02_(0.0f),
			m03_(0.0f),
			m10_(0.0f),
			m11_(1.0f),
			m12_(0.0f),
			m13_(0.0f),
			m20_(0.0f),
			m21_(0.0f),
			m22_(1.0f),
			m23_(0.0f),
			m30_(0.0f),
			m31_(0.0f),
			m32_(0.0f),
			m33_(1.0f)
		{
		}


		Matrix4::Matrix4(const Matrix4& matrix) :
			m00_(matrix.m00_),
			m01_(matrix.m01_),
			m02_(matrix.m02_),
			m03_(matrix.m03_),
			m10_(matrix.m10_),
			m11_(matrix.m11_),
			m12_(matrix.m12_),
			m13_(matrix.m13_),
			m20_(matrix.m20_),
			m21_(matrix.m21_),
			m22_(matrix.m22_),
			m23_(matrix.m23_),
			m30_(matrix.m30_),
			m31_(matrix.m31_),
			m32_(matrix.m32_),
			m33_(matrix.m33_)
		{
		}

		Matrix4::Matrix4(
			float v00, float v01, float v02, float v03,
			float v10, float v11, float v12, float v13,
			float v20, float v21, float v22, float v23,
			float v30, float v31, float v32, float v33) :
			m00_(v00),
			m01_(v01),
			m02_(v02),
			m03_(v03),
			m10_(v10),
			m11_(v11),
			m12_(v12),
			m13_(v13),
			m20_(v20),
			m21_(v21),
			m22_(v22),
			m23_(v23),
			m30_(v30),
			m31_(v31),
			m32_(v32),
			m33_(v33)
		{
		}

		Matrix4::Matrix4(const float* data) :
			m00_(data[0]),
			m01_(data[1]),
			m02_(data[2]),
			m03_(data[3]),
			m10_(data[4]),
			m11_(data[5]),
			m12_(data[6]),
			m13_(data[7]),
			m20_(data[8]),
			m21_(data[9]),
			m22_(data[10]),
			m23_(data[11]),
			m30_(data[12]),
			m31_(data[13]),
			m32_(data[14]),
			m33_(data[15])
		{
		}

		Matrix4& Matrix4::operator =(const Matrix4& rhs)
		{
			m00_ = rhs.m00_;
			m01_ = rhs.m01_;
			m02_ = rhs.m02_;
			m03_ = rhs.m03_;
			m10_ = rhs.m10_;
			m11_ = rhs.m11_;
			m12_ = rhs.m12_;
			m13_ = rhs.m13_;
			m20_ = rhs.m20_;
			m21_ = rhs.m21_;
			m22_ = rhs.m22_;
			m23_ = rhs.m23_;
			m30_ = rhs.m30_;
			m31_ = rhs.m31_;
			m32_ = rhs.m32_;
			m33_ = rhs.m33_;
			return *this;
		}

		bool Matrix4::operator ==(const Matrix4& rhs) const
		{
			const float* leftData = Data();
			const float* rightData = rhs.Data();

			for (unsigned i = 0; i < 16; ++i)
			{
				if (leftData[i] != rightData[i])
					return false;
			}

			return true;
		}

		bool Matrix4::operator !=(const Matrix4& rhs) const
		{
			return !(*this == rhs);
		}

		Matrix4 Matrix4::operator + (const Matrix4& rhs) const
		{
			return Matrix4(
				m00_ + rhs.m00_,
				m01_ + rhs.m01_,
				m02_ + rhs.m02_,
				m03_ + rhs.m03_,
				m10_ + rhs.m10_,
				m11_ + rhs.m11_,
				m12_ + rhs.m12_,
				m13_ + rhs.m13_,
				m20_ + rhs.m20_,
				m21_ + rhs.m21_,
				m22_ + rhs.m22_,
				m23_ + rhs.m23_,
				m30_ + rhs.m30_,
				m31_ + rhs.m31_,
				m32_ + rhs.m32_,
				m33_ + rhs.m33_
				);
		}

		Matrix4 Matrix4::operator -(const Matrix4& rhs) const
		{
			return Matrix4(
				m00_ - rhs.m00_,
				m01_ - rhs.m01_,
				m02_ - rhs.m02_,
				m03_ - rhs.m03_,
				m10_ - rhs.m10_,
				m11_ - rhs.m11_,
				m12_ - rhs.m12_,
				m13_ - rhs.m13_,
				m20_ - rhs.m20_,
				m21_ - rhs.m21_,
				m22_ - rhs.m22_,
				m23_ - rhs.m23_,
				m30_ - rhs.m30_,
				m31_ - rhs.m31_,
				m32_ - rhs.m32_,
				m33_ - rhs.m33_
				);
		}

		Matrix4 Matrix4::operator *(float rhs) const
		{
			return Matrix4(
				m00_ * rhs,
				m01_ * rhs,
				m02_ * rhs,
				m03_ * rhs,
				m10_ * rhs,
				m11_ * rhs,
				m12_ * rhs,
				m13_ * rhs,
				m20_ * rhs,
				m21_ * rhs,
				m22_ * rhs,
				m23_ * rhs,
				m30_ * rhs,
				m31_ * rhs,
				m32_ * rhs,
				m33_ * rhs
				);
		}

		Matrix4 Matrix4::operator *(const Matrix4& rhs) const
		{
			return Matrix4(
				m00_ * rhs.m00_ + m01_ * rhs.m10_ + m02_ * rhs.m20_ + m03_ * rhs.m30_,
				m00_ * rhs.m01_ + m01_ * rhs.m11_ + m02_ * rhs.m21_ + m03_ * rhs.m31_,
				m00_ * rhs.m02_ + m01_ * rhs.m12_ + m02_ * rhs.m22_ + m03_ * rhs.m32_,
				m00_ * rhs.m03_ + m01_ * rhs.m13_ + m02_ * rhs.m23_ + m03_ * rhs.m33_,
				m10_ * rhs.m00_ + m11_ * rhs.m10_ + m12_ * rhs.m20_ + m13_ * rhs.m30_,
				m10_ * rhs.m01_ + m11_ * rhs.m11_ + m12_ * rhs.m21_ + m13_ * rhs.m31_,
				m10_ * rhs.m02_ + m11_ * rhs.m12_ + m12_ * rhs.m22_ + m13_ * rhs.m32_,
				m10_ * rhs.m03_ + m11_ * rhs.m13_ + m12_ * rhs.m23_ + m13_ * rhs.m33_,
				m20_ * rhs.m00_ + m21_ * rhs.m10_ + m22_ * rhs.m20_ + m23_ * rhs.m30_,
				m20_ * rhs.m01_ + m21_ * rhs.m11_ + m22_ * rhs.m21_ + m23_ * rhs.m31_,
				m20_ * rhs.m02_ + m21_ * rhs.m12_ + m22_ * rhs.m22_ + m23_ * rhs.m32_,
				m20_ * rhs.m03_ + m21_ * rhs.m13_ + m22_ * rhs.m23_ + m23_ * rhs.m33_,
				m30_ * rhs.m00_ + m31_ * rhs.m10_ + m32_ * rhs.m20_ + m33_ * rhs.m30_,
				m30_ * rhs.m01_ + m31_ * rhs.m11_ + m32_ * rhs.m21_ + m33_ * rhs.m31_,
				m30_ * rhs.m02_ + m31_ * rhs.m12_ + m32_ * rhs.m22_ + m33_ * rhs.m32_,
				m30_ * rhs.m03_ + m31_ * rhs.m13_ + m32_ * rhs.m23_ + m33_ * rhs.m33_
				);
		}

		Matrix4 Matrix4::operator *(const Matrix3x4& rhs) const
		{
			return Matrix4(
				m00_ * rhs.m00_ + m01_ * rhs.m10_ + m02_ * rhs.m20_,
				m00_ * rhs.m01_ + m01_ * rhs.m11_ + m02_ * rhs.m21_,
				m00_ * rhs.m02_ + m01_ * rhs.m12_ + m02_ * rhs.m22_,
				m00_ * rhs.m03_ + m01_ * rhs.m13_ + m02_ * rhs.m23_ + m03_,
				m10_ * rhs.m00_ + m11_ * rhs.m10_ + m12_ * rhs.m20_,
				m10_ * rhs.m01_ + m11_ * rhs.m11_ + m12_ * rhs.m21_,
				m10_ * rhs.m02_ + m11_ * rhs.m12_ + m12_ * rhs.m22_,
				m10_ * rhs.m03_ + m11_ * rhs.m13_ + m12_ * rhs.m23_ + m13_,
				m20_ * rhs.m00_ + m21_ * rhs.m10_ + m22_ * rhs.m20_,
				m20_ * rhs.m01_ + m21_ * rhs.m11_ + m22_ * rhs.m21_,
				m20_ * rhs.m02_ + m21_ * rhs.m12_ + m22_ * rhs.m22_,
				m20_ * rhs.m03_ + m21_ * rhs.m13_ + m22_ * rhs.m23_ + m23_,
				m30_ * rhs.m00_ + m31_ * rhs.m10_ + m32_ * rhs.m20_,
				m30_ * rhs.m01_ + m31_ * rhs.m11_ + m32_ * rhs.m21_,
				m30_ * rhs.m02_ + m31_ * rhs.m12_ + m32_ * rhs.m22_,
				m30_ * rhs.m03_ + m31_ * rhs.m13_ + m32_ * rhs.m23_ + m33_
			);
		}

		Vector3 Matrix4::operator *(const Vector3& rhs) const
		{
			float invW = 1.0f / (m30_ * rhs.x_ + m31_ * rhs.y_ + m32_ * rhs.z_ + m33_);

			return Vector3(
				(m00_ * rhs.x_ + m01_ * rhs.y_ + m02_ * rhs.z_ + m03_) * invW,
				(m10_ * rhs.x_ + m11_ * rhs.y_ + m12_ * rhs.z_ + m13_) * invW,
				(m20_ * rhs.x_ + m21_ * rhs.y_ + m22_ * rhs.z_ + m23_) * invW
			);
		}

		Matrix4 Matrix4::Transpose() const
		{
			return Matrix4(
				m00_,
				m10_,
				m20_,
				m30_,
				m01_,
				m11_,
				m21_,
				m31_,
				m02_,
				m12_,
				m22_,
				m32_,
				m03_,
				m13_,
				m23_,
				m33_
				);
		}

		Matrix4 Matrix4::Inverse() const
		{
			float v0 = m20_ * m31_ - m21_ * m30_;
			float v1 = m20_ * m32_ - m22_ * m30_;
			float v2 = m20_ * m33_ - m23_ * m30_;
			float v3 = m21_ * m32_ - m22_ * m31_;
			float v4 = m21_ * m33_ - m23_ * m31_;
			float v5 = m22_ * m33_ - m23_ * m32_;

			float i00 = (v5 * m11_ - v4 * m12_ + v3 * m13_);
			float i10 = -(v5 * m10_ - v2 * m12_ + v1 * m13_);
			float i20 = (v4 * m10_ - v2 * m11_ + v0 * m13_);
			float i30 = -(v3 * m10_ - v1 * m11_ + v0 * m12_);

			float invDet = 1.0f / (i00 * m00_ + i10 * m01_ + i20 * m02_ + i30 * m03_);

			i00 *= invDet;
			i10 *= invDet;
			i20 *= invDet;
			i30 *= invDet;

			float i01 = -(v5 * m01_ - v4 * m02_ + v3 * m03_) * invDet;
			float i11 = (v5 * m00_ - v2 * m02_ + v1 * m03_) * invDet;
			float i21 = -(v4 * m00_ - v2 * m01_ + v0 * m03_) * invDet;
			float i31 = (v3 * m00_ - v1 * m01_ + v0 * m02_) * invDet;

			v0 = m10_ * m31_ - m11_ * m30_;
			v1 = m10_ * m32_ - m12_ * m30_;
			v2 = m10_ * m33_ - m13_ * m30_;
			v3 = m11_ * m32_ - m12_ * m31_;
			v4 = m11_ * m33_ - m13_ * m31_;
			v5 = m12_ * m33_ - m13_ * m32_;

			float i02 = (v5 * m01_ - v4 * m02_ + v3 * m03_) * invDet;
			float i12 = -(v5 * m00_ - v2 * m02_ + v1 * m03_) * invDet;
			float i22 = (v4 * m00_ - v2 * m01_ + v0 * m03_) * invDet;
			float i32 = -(v3 * m00_ - v1 * m01_ + v0 * m02_) * invDet;

			v0 = m21_ * m10_ - m20_ * m11_;
			v1 = m22_ * m10_ - m20_ * m12_;
			v2 = m23_ * m10_ - m20_ * m13_;
			v3 = m22_ * m11_ - m21_ * m12_;
			v4 = m23_ * m11_ - m21_ * m13_;
			v5 = m23_ * m12_ - m22_ * m13_;

			float i03 = -(v5 * m01_ - v4 * m02_ + v3 * m03_) * invDet;
			float i13 = (v5 * m00_ - v2 * m02_ + v1 * m03_) * invDet;
			float i23 = -(v4 * m00_ - v2 * m01_ + v0 * m03_) * invDet;
			float i33 = (v3 * m00_ - v1 * m01_ + v0 * m02_) * invDet;

			return Matrix4(
				i00, i01, i02, i03,
				i10, i11, i12, i13,
				i20, i21, i22, i23,
				i30, i31, i32, i33);
		}

		const float* Matrix4::Data() const
		{ 
			return &m00_;
		}

		float Matrix4::Element(unsigned i, unsigned j) const
		{
			return Data()[i * 4 + j];
		}

		void Matrix4::BulkTranspose(float* dest, const float* src, unsigned count)
		{
			for (unsigned i = 0; i < count; ++i)
			{
				dest[0] = src[0];
				dest[1] = src[4];
				dest[2] = src[8];
				dest[3] = src[12];
				dest[4] = src[1];
				dest[5] = src[5];
				dest[6] = src[9];
				dest[7] = src[13];
				dest[8] = src[2];
				dest[9] = src[6];
				dest[10] = src[10];
				dest[11] = src[14];
				dest[12] = src[3];
				dest[13] = src[7];
				dest[14] = src[11];
				dest[15] = src[15];

				dest += 16;
				src += 16;
			}
		}
	}
}

