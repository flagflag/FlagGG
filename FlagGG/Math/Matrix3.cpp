#include "Math/Matrix3.h"
#include "Math/Math.h"

#include <math.h>
#include <cstdio>

namespace FlagGG
{
	namespace Math
	{
		const Matrix3 Matrix3::ZERO(
			0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f);

		const Matrix3 Matrix3::IDENTITY;

		Matrix3::Matrix3() :
			m00_(1.0f),
			m01_(0.0f),
			m02_(0.0f),
			m10_(0.0f),
			m11_(1.0f),
			m12_(0.0f),
			m20_(0.0f),
			m21_(0.0f),
			m22_(1.0f)
		{
		}

		Matrix3::Matrix3(
			Real v00, Real v01, Real v02,
			Real v10, Real v11, Real v12,
			Real v20, Real v21, Real v22) :
			m00_(v00),
			m01_(v01),
			m02_(v02),
			m10_(v10),
			m11_(v11),
			m12_(v12),
			m20_(v20),
			m21_(v21),
			m22_(v22)
		{
		}

		Matrix3::Matrix3(const Real* data) :
			m00_(data[0]),
			m01_(data[1]),
			m02_(data[2]),
			m10_(data[3]),
			m11_(data[4]),
			m12_(data[5]),
			m20_(data[6]),
			m21_(data[7]),
			m22_(data[8])
		{
		}

		bool Matrix3::operator ==(const Matrix3& rhs) const
		{
			const Real* leftData = Data();
			const Real* rightData = rhs.Data();

			for (unsigned i = 0; i < 9; ++i)
			{
				if (leftData[i] != rightData[i])
					return false;
			}

			return true;
		}

		bool Matrix3::operator !=(const Matrix3& rhs) const
		{
			return !(*this == rhs);
		}

		Vector3 Matrix3::operator *(const Vector3& rhs) const
		{
			return Vector3(
				m00_ * rhs.x_ + m01_ * rhs.y_ + m02_ * rhs.z_,
				m10_ * rhs.x_ + m11_ * rhs.y_ + m12_ * rhs.z_,
				m20_ * rhs.x_ + m21_ * rhs.y_ + m22_ * rhs.z_
				);
		}

		Matrix3 Matrix3::operator +(const Matrix3& rhs) const
		{
			return Matrix3(
				m00_ + rhs.m00_,
				m01_ + rhs.m01_,
				m02_ + rhs.m02_,
				m10_ + rhs.m10_,
				m11_ + rhs.m11_,
				m12_ + rhs.m12_,
				m20_ + rhs.m20_,
				m21_ + rhs.m21_,
				m22_ + rhs.m22_
				);
		}

		Matrix3 Matrix3::operator -(const Matrix3& rhs) const
		{
			return Matrix3(
				m00_ - rhs.m00_,
				m01_ - rhs.m01_,
				m02_ - rhs.m02_,
				m10_ - rhs.m10_,
				m11_ - rhs.m11_,
				m12_ - rhs.m12_,
				m20_ - rhs.m20_,
				m21_ - rhs.m21_,
				m22_ - rhs.m22_
				);
		}

		Matrix3 Matrix3::operator *(Real rhs) const
		{
			return Matrix3(
				m00_ * rhs,
				m01_ * rhs,
				m02_ * rhs,
				m10_ * rhs,
				m11_ * rhs,
				m12_ * rhs,
				m20_ * rhs,
				m21_ * rhs,
				m22_ * rhs
				);
		}

		Matrix3 Matrix3::operator *(const Matrix3& rhs) const
		{
			return Matrix3(
				m00_ * rhs.m00_ + m01_ * rhs.m10_ + m02_ * rhs.m20_,
				m00_ * rhs.m01_ + m01_ * rhs.m11_ + m02_ * rhs.m21_,
				m00_ * rhs.m02_ + m01_ * rhs.m12_ + m02_ * rhs.m22_,
				m10_ * rhs.m00_ + m11_ * rhs.m10_ + m12_ * rhs.m20_,
				m10_ * rhs.m01_ + m11_ * rhs.m11_ + m12_ * rhs.m21_,
				m10_ * rhs.m02_ + m11_ * rhs.m12_ + m12_ * rhs.m22_,
				m20_ * rhs.m00_ + m21_ * rhs.m10_ + m22_ * rhs.m20_,
				m20_ * rhs.m01_ + m21_ * rhs.m11_ + m22_ * rhs.m21_,
				m20_ * rhs.m02_ + m21_ * rhs.m12_ + m22_ * rhs.m22_
				);
		}

		void Matrix3::SetScale(const Vector3& scale)
		{
			m00_ = scale.x_;
			m11_ = scale.y_;
			m22_ = scale.z_;
		}

		void Matrix3::SetScale(Real scale)
		{
			m00_ = scale;
			m11_ = scale;
			m22_ = scale;
		}

		Vector3 Matrix3::Scale() const
		{
			return Vector3(
				sqrtf(m00_ * m00_ + m10_ * m10_ + m20_ * m20_),
				sqrtf(m01_ * m01_ + m11_ * m11_ + m21_ * m21_),
				sqrtf(m02_ * m02_ + m12_ * m12_ + m22_ * m22_)
				);
		}

		Vector3 Matrix3::SignedScale(const Matrix3& rotation) const
		{
			return Vector3(
				rotation.m00_ * m00_ + rotation.m10_ * m10_ + rotation.m20_ * m20_,
				rotation.m01_ * m01_ + rotation.m11_ * m11_ + rotation.m21_ * m21_,
				rotation.m02_ * m02_ + rotation.m12_ * m12_ + rotation.m22_ * m22_
				);
		}

		Matrix3 Matrix3::Transpose() const
		{
			return Matrix3(
				m00_,
				m10_,
				m20_,
				m01_,
				m11_,
				m21_,
				m02_,
				m12_,
				m22_
				);
		}

		Matrix3 Matrix3::Scaled(const Vector3& scale) const
		{
			return Matrix3(
				m00_ * scale.x_,
				m01_ * scale.y_,
				m02_ * scale.z_,
				m10_ * scale.x_,
				m11_ * scale.y_,
				m12_ * scale.z_,
				m20_ * scale.x_,
				m21_ * scale.y_,
				m22_ * scale.z_
				);
		}

		bool Matrix3::Equals(const Matrix3& rhs) const
		{
			const Real* leftData = Data();
			const Real* rightData = rhs.Data();

			for (unsigned i = 0; i < 9; ++i)
			{
				if (!FlagGG::Math::Equals(leftData[i], rightData[i]))
					return false;
			}

			return true;
		}

		Matrix3 Matrix3::Inverse() const
		{
			Real det = m00_ * m11_ * m22_ +
				m10_ * m21_ * m02_ +
				m20_ * m01_ * m12_ -
				m20_ * m11_ * m02_ -
				m10_ * m01_ * m22_ -
				m00_ * m21_ * m12_;

			Real invDet = 1.0f / det;

			return Matrix3(
				(m11_ * m22_ - m21_ * m12_) * invDet,
				-(m01_ * m22_ - m21_ * m02_) * invDet,
				(m01_ * m12_ - m11_ * m02_) * invDet,
				-(m10_ * m22_ - m20_ * m12_) * invDet,
				(m00_ * m22_ - m20_ * m02_) * invDet,
				-(m00_ * m12_ - m10_ * m02_) * invDet,
				(m10_ * m21_ - m20_ * m11_) * invDet,
				-(m00_ * m21_ - m20_ * m01_) * invDet,
				(m00_ * m11_ - m10_ * m01_) * invDet
				);
		}

		Container::String Matrix3::ToString() const
		{
			char tempBuffer[Container::MATRIX_CONVERSION_BUFFER_LENGTH];
			sprintf(tempBuffer, "%g %g %g %g %g %g %g %g %g", m00_, m01_, m02_, m10_, m11_, m12_, m20_, m21_, m22_);
			return Container::String(tempBuffer);
		}
	}
}
