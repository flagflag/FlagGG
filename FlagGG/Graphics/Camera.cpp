#include "Camera.h"
#include "RenderEngine.h"
#include "Math/Math.h"

namespace FlagGG
{
	namespace Graphics
	{
		Camera::Camera(CameraType cameraType) :
			cameraType_(cameraType),
			pos_(0.0f, 0.0f, 0.0f),
			right_(1.0f, 0.0f, 0.0f),
			up_(0.0f, 1.0f, 0.0f),
			look_(0.0f, 0.0f, 1.0f),
			nearClip_(1.0f),
			farClip_(100.0f)
		{
		}

		void Camera::Strafe(float units)
		{
			if (cameraType_ == LAND_OBJECT)
			{
				pos_ += Math::Vector3(right_.x_, 0.0f, right_.z_) * units;
			}

			if (cameraType_ == AIRCRAFT)
			{
				pos_ += right_ * units;
			}
		}

		void Camera::Fly(float units)
		{
			if (cameraType_ == LAND_OBJECT)
			{
				pos_.y_ += units;
			}

			if (cameraType_ == AIRCRAFT)
			{
				pos_ += up_ * units;
			}
		}

		void Camera::Walk(float units)
		{
			if (cameraType_ == LAND_OBJECT)
			{
				pos_ += Math::Vector3(look_.x_, 0.0f, look_.z_) * units;
			}

			if (cameraType_ == AIRCRAFT)
			{
				pos_ += look_ * units;
			}
		}

		void Camera::Pitch(float angle)
		{
			Math::Matrix4 T = Math::MatrixRotationAxis(right_, angle);

			up_ = Math::Vector3TransformCoord(up_, T);
			look_ = Math::Vector3TransformCoord(look_, T);
		}

		void Camera::Yaw(float angle)
		{
			Math::Matrix4 T;

			if (cameraType_ == LAND_OBJECT)
			{
				T = Math::MatrixRotationY(angle);
			}

			if (cameraType_ == AIRCRAFT)
			{
				T = Math::MatrixRotationAxis(up_, angle);
			}

			right_ = Math::Vector3TransformCoord(right_, T);
			look_ = Math::Vector3TransformCoord(look_, T);
		}

		void Camera::Roll(float angle)
		{
			if (cameraType_ == AIRCRAFT)
			{
				Math::Matrix4 T = Math::MatrixRotationAxis(look_, angle);

				right_ = Math::Vector3TransformCoord(right_, T);
				up_ = Math::Vector3TransformCoord(up_, T);
			}
		}

		Math::Matrix4 Camera::GetViewMatrix()
		{
			look_ = look_.Normalized();

			up_ = look_.CrossProduct(right_);
			up_ = up_.Normalized();

			right_ = up_.CrossProduct(look_);
			right_ = right_.Normalized();

			float x = -right_.DotProduct(pos_);
			float y = -up_.DotProduct(pos_);
			float z = -look_.DotProduct(pos_);

			return Math::Matrix4(
				right_.x_, up_.x_, look_.x_, 0.0f,
				right_.y_, up_.y_, look_.y_, 0.0f,
				right_.z_, up_.z_, look_.z_, 0.0f,
				x, y, z, 1.0f
				);
		}

		Math::Matrix4 Camera::GetProjectionMatrix()
		{
			return Math::MatrixPerspectiveFovLH(
				Math::PI / 4,
				1.0f,
				nearClip_,
				farClip_
				);
		}

		CameraType Camera::GetCameraType() const
		{
			return cameraType_;
		}

		void Camera::SetCameraType(CameraType cameraType)
		{
			cameraType_ = cameraType;
		}

		const Math::Vector3& Camera::GetPosition() const
		{
			return pos_;
		}

		void Camera::SetPosition(const Math::Vector3& pos)
		{
			pos_ = pos;
		}

		void Camera::SetFarClip(float farClip)
		{
			farClip_ = farClip;
		}

		float Camera::GetFarClip() const
		{
			return farClip_;
		}

		void Camera::SetNearClip(float nearClip)
		{
			nearClip_ = nearClip;
		}

		float Camera::GetNearClip() const
		{
			return nearClip_;
		}

		const Math::Vector3& Camera::GetRight() const
		{
			return right_;
		}

		const Math::Vector3& Camera::GetUp() const
		{
			return up_;
		}

		const Math::Vector3& Camera::GetLook() const
		{
			return look_;
		}
	}
}
