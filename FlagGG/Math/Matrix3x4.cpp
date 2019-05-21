#include "Math/Matrix3x4.h"

#include <cstdio>

namespace FlagGG
{
	namespace Math
	{
		const Matrix3x4 Matrix3x4::ZERO(
			0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f);

		const Matrix3x4 Matrix3x4::IDENTITY;

		Matrix3x4::Matrix3x4() :
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
			m23_(0.0f)
		{
		}

		Matrix3x4::Matrix3x4(const Matrix3& matrix) :
			m00_(matrix.m00_),
			m01_(matrix.m01_),
			m02_(matrix.m02_),
			m03_(0.0f),
			m10_(matrix.m10_),
			m11_(matrix.m11_),
			m12_(matrix.m12_),
			m13_(0.0f),
			m20_(matrix.m20_),
			m21_(matrix.m21_),
			m22_(matrix.m22_),
			m23_(0.0f)
		{
		}

		Matrix3x4::Matrix3x4(const Matrix4& matrix) :
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
			m23_(matrix.m23_)
		{
		}

		Matrix3x4::Matrix3x4(
			float v00, float v01, float v02, float v03,
			float v10, float v11, float v12, float v13,
			float v20, float v21, float v22, float v23) :
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
			m23_(v23)
		{
		}

		Matrix3x4::Matrix3x4(const float* data) :
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
			m23_(data[11])
		{
		}

		Matrix3x4::Matrix3x4(const Vector3& translation, const Quaternion& rotation, float scale)
		{
			SetRotation(rotation.RotationMatrix() * scale);
			SetTranslation(translation);
		}

		Matrix3x4::Matrix3x4(const Vector3& translation, const Quaternion& rotation, const Vector3& scale)
		{
			SetRotation(rotation.RotationMatrix().Scaled(scale));
			SetTranslation(translation);
		}

		Matrix3x4::Matrix3x4(const Quaternion& rotation)
		{
			SetRotation(rotation.RotationMatrix().Scaled(Vector3::ONE));
			SetTranslation(Vector3::ZERO);
		}

		Matrix3x4& Matrix3x4::operator =(const Matrix3& rhs)
		{
			m00_ = rhs.m00_;
			m01_ = rhs.m01_;
			m02_ = rhs.m02_;
			m03_ = 0.0f;
			m10_ = rhs.m10_;
			m11_ = rhs.m11_;
			m12_ = rhs.m12_;
			m13_ = 0.0f;
			m20_ = rhs.m20_;
			m21_ = rhs.m21_;
			m22_ = rhs.m22_;
			m23_ = 0.0f;
			return *this;
		}

		Matrix3x4& Matrix3x4::operator =(const Matrix4& rhs)
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
			return *this;
		}

		bool Matrix3x4::operator ==(const Matrix3x4& rhs) const
		{
			const float* leftData = Data();
			const float* rightData = rhs.Data();

			for (unsigned i = 0; i < 12; ++i)
			{
				if (leftData[i] != rightData[i])
					return false;
			}

			return true;
		}

		bool Matrix3x4::operator !=(const Matrix3x4& rhs) const
		{
			return !(*this == rhs);
		}

		Vector3 Matrix3x4::operator *(const Vector3& rhs) const
		{
			return Vector3(
				(m00_ * rhs.x_ + m01_ * rhs.y_ + m02_ * rhs.z_ + m03_),
				(m10_ * rhs.x_ + m11_ * rhs.y_ + m12_ * rhs.z_ + m13_),
				(m20_ * rhs.x_ + m21_ * rhs.y_ + m22_ * rhs.z_ + m23_)
			);
		}

		Vector3 Matrix3x4::operator *(const Vector4& rhs) const
		{
			return Vector3(
				(m00_ * rhs.x_ + m01_ * rhs.y_ + m02_ * rhs.z_ + m03_ * rhs.w_),
				(m10_ * rhs.x_ + m11_ * rhs.y_ + m12_ * rhs.z_ + m13_ * rhs.w_),
				(m20_ * rhs.x_ + m21_ * rhs.y_ + m22_ * rhs.z_ + m23_ * rhs.w_)
			);
		}

		Matrix3x4 Matrix3x4::operator +(const Matrix3x4& rhs) const
		{
			return Matrix3x4(
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
				m23_ + rhs.m23_
			);
		}

		Matrix3x4 Matrix3x4::operator -(const Matrix3x4& rhs) const
		{
			return Matrix3x4(
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
				m23_ - rhs.m23_
			);
		}

		Matrix3x4 Matrix3x4::operator *(float rhs) const
		{
			return Matrix3x4(
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
				m23_ * rhs
			);
		}

		Matrix3x4 Matrix3x4::operator *(const Matrix3x4& rhs) const
		{
			return Matrix3x4(
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
				m20_ * rhs.m03_ + m21_ * rhs.m13_ + m22_ * rhs.m23_ + m23_
			);
		}

		Matrix4 Matrix3x4::operator *(const Matrix4& rhs) const
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
				rhs.m30_,
				rhs.m31_,
				rhs.m32_,
				rhs.m33_
			);
		}

		void Matrix3x4::SetTranslation(const Vector3& translation)
		{
			m03_ = translation.x_;
			m13_ = translation.y_;
			m23_ = translation.z_;
		}

		void Matrix3x4::SetRotation(const Matrix3& rotation)
		{
			m00_ = rotation.m00_;
			m01_ = rotation.m01_;
			m02_ = rotation.m02_;
			m10_ = rotation.m10_;
			m11_ = rotation.m11_;
			m12_ = rotation.m12_;
			m20_ = rotation.m20_;
			m21_ = rotation.m21_;
			m22_ = rotation.m22_;
		}

		void Matrix3x4::SetScale(const Vector3& scale)
		{
			m00_ = scale.x_;
			m11_ = scale.y_;
			m22_ = scale.z_;
		}

		void Matrix3x4::SetScale(float scale)
		{
			m00_ = scale;
			m11_ = scale;
			m22_ = scale;
		}

		Matrix3 Matrix3x4::ToMatrix3() const
		{
			return Matrix3(
				m00_,
				m01_,
				m02_,
				m10_,
				m11_,
				m12_,
				m20_,
				m21_,
				m22_
			);
		}

		Matrix4 Matrix3x4::ToMatrix4() const
		{
			return Matrix4(
				m00_,
				m01_,
				m02_,
				m03_,
				m10_,
				m11_,
				m12_,
				m13_,
				m20_,
				m21_,
				m22_,
				m23_,
				0.0f,
				0.0f,
				0.0f,
				1.0f
			);
		}

		Matrix3 Matrix3x4::RotationMatrix() const
		{
			Vector3 invScale(
				1.0f / sqrtf(m00_ * m00_ + m10_ * m10_ + m20_ * m20_),
				1.0f / sqrtf(m01_ * m01_ + m11_ * m11_ + m21_ * m21_),
				1.0f / sqrtf(m02_ * m02_ + m12_ * m12_ + m22_ * m22_)
			);

			return ToMatrix3().Scaled(invScale);
		}

		Vector3 Matrix3x4::Translation() const
		{
			return Vector3(
				m03_,
				m13_,
				m23_
			);
		}

		Quaternion Matrix3x4::Rotation() const
		{
			return Quaternion(RotationMatrix());
		}

		Vector3 Matrix3x4::Scale() const
		{
			return Vector3(
				sqrtf(m00_ * m00_ + m10_ * m10_ + m20_ * m20_),
				sqrtf(m01_ * m01_ + m11_ * m11_ + m21_ * m21_),
				sqrtf(m02_ * m02_ + m12_ * m12_ + m22_ * m22_)
			);
		}

		Vector3 Matrix3x4::SignedScale(const Matrix3& rotation) const
		{
			return Vector3(
				rotation.m00_ * m00_ + rotation.m10_ * m10_ + rotation.m20_ * m20_,
				rotation.m01_ * m01_ + rotation.m11_ * m11_ + rotation.m21_ * m21_,
				rotation.m02_ * m02_ + rotation.m12_ * m12_ + rotation.m22_ * m22_
			);
		}

		bool Matrix3x4::Equals(const Matrix3x4& rhs) const
		{
			const float* leftData = Data();
			const float* rightData = rhs.Data();

			for (unsigned i = 0; i < 12; ++i)
			{
				if (!FlagGG::Math::Equals(leftData[i], rightData[i]))
					return false;
			}

			return true;
		}

		void Matrix3x4::Decompose(Vector3& translation, Quaternion& rotation, Vector3& scale) const
		{
			translation.x_ = m03_;
			translation.y_ = m13_;
			translation.z_ = m23_;

			scale.x_ = sqrtf(m00_ * m00_ + m10_ * m10_ + m20_ * m20_);
			scale.y_ = sqrtf(m01_ * m01_ + m11_ * m11_ + m21_ * m21_);
			scale.z_ = sqrtf(m02_ * m02_ + m12_ * m12_ + m22_ * m22_);

			Vector3 invScale(1.0f / scale.x_, 1.0f / scale.y_, 1.0f / scale.z_);
			rotation = Quaternion(ToMatrix3().Scaled(invScale));
		}

		Matrix3x4 Matrix3x4::Inverse() const
		{
			float det = m00_ * m11_ * m22_ +
				m10_ * m21_ * m02_ +
				m20_ * m01_ * m12_ -
				m20_ * m11_ * m02_ -
				m10_ * m01_ * m22_ -
				m00_ * m21_ * m12_;

			float invDet = 1.0f / det;
			Matrix3x4 ret;

			ret.m00_ = (m11_ * m22_ - m21_ * m12_) * invDet;
			ret.m01_ = -(m01_ * m22_ - m21_ * m02_) * invDet;
			ret.m02_ = (m01_ * m12_ - m11_ * m02_) * invDet;
			ret.m03_ = -(m03_ * ret.m00_ + m13_ * ret.m01_ + m23_ * ret.m02_);
			ret.m10_ = -(m10_ * m22_ - m20_ * m12_) * invDet;
			ret.m11_ = (m00_ * m22_ - m20_ * m02_) * invDet;
			ret.m12_ = -(m00_ * m12_ - m10_ * m02_) * invDet;
			ret.m13_ = -(m03_ * ret.m10_ + m13_ * ret.m11_ + m23_ * ret.m12_);
			ret.m20_ = (m10_ * m21_ - m20_ * m11_) * invDet;
			ret.m21_ = -(m00_ * m21_ - m20_ * m01_) * invDet;
			ret.m22_ = (m00_ * m11_ - m10_ * m01_) * invDet;
			ret.m23_ = -(m03_ * ret.m20_ + m13_ * ret.m21_ + m23_ * ret.m22_);

			return ret;
		}

		const float* Matrix3x4::Data() const
		{ 
			return &m00_; 
		}

		float Matrix3x4::Element(unsigned i, unsigned j) const 
		{ 
			return Data()[i * 4 + j]; 
		}

		Vector4 Matrix3x4::Row(unsigned i) const 
		{ 
			return Vector4(Element(i, 0), Element(i, 1), Element(i, 2), Element(i, 3));
		}

		Vector3 Matrix3x4::Column(unsigned j) const 
		{ 
			return Vector3(Element(0, j), Element(1, j), Element(2, j)); 
		}

		Container::String Matrix3x4::ToString() const
		{
			char tempBuffer[MATRIX_CONVERSION_BUFFER_LENGTH];
			sprintf(tempBuffer, "%g %g %g %g %g %g %g %g %g %g %g %g", m00_, m01_, m02_, m03_, m10_, m11_, m12_, m13_, m20_, m21_, m22_,
				m23_);
			return Container::String(tempBuffer);
		}
	}
}
