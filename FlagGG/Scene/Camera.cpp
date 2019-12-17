#include "Scene/Camera.h"
#include "Scene/Node.h"

namespace FlagGG
{
	namespace Scene
	{
		void Camera::Strafe(float units)
		{
			if (cameraType_ == LAND_OBJECT)
			{
				Node* master = GetNode();
				Math::Vector3 right = GetRight();
				master->SetWorldPosition(master->GetWorldPosition() + Math::Vector3(right.x_, 0.0f, right.z_) * units);
			}

			if (cameraType_ == AIRCRAFT)
			{
				Node* master = GetNode();
				Math::Vector3 right = GetRight();
				master->SetWorldPosition(master->GetWorldPosition() + right * units);
			}
		}

		void Camera::Fly(float units)
		{
			if (cameraType_ == LAND_OBJECT)
			{
				Node* master = GetNode();
				master->SetWorldPosition(master->GetWorldPosition() + Math::Vector3(0.0f, units, 0.0f));
			}

			if (cameraType_ == AIRCRAFT)
			{
				Node* master = GetNode();
				Math::Vector3 up = GetUp();
				master->SetWorldPosition(master->GetWorldPosition() + up * units);
			}
		}

		void Camera::Walk(float units)
		{
			if (cameraType_ == LAND_OBJECT)
			{
				Node* master = GetNode();
				Math::Vector3 look = GetLook();
				master->SetWorldPosition(master->GetWorldPosition() + Math::Vector3(look.x_, 0.0f, look.z_) * units);
			}

			if (cameraType_ == AIRCRAFT)
			{
				Node* master = GetNode();
				Math::Vector3 look = GetLook();
				master->SetWorldPosition(master->GetWorldPosition() + look * units);
			}
		}

		void Camera::Correct(Math::Vector3& right, Math::Vector3& up, Math::Vector3& look)
		{
			look.Normalize();

			up = look.CrossProduct(right);
			up.Normalize();

			right = up.CrossProduct(look);
			right.Normalize();
		}

		void Camera::Pitch(float angle)
		{
			auto matrix = GetNode()->GetRotation().RotationMatrix();

			Math::Vector3 right(matrix.m00_, matrix.m10_, matrix.m20_);
			Math::Vector3 up(matrix.m01_, matrix.m11_, matrix.m21_);
			Math::Vector3 look(matrix.m02_, matrix.m12_, matrix.m22_);

			Math::Matrix4 T = Math::MatrixRotationAxis(right, angle);
			up = Math::Vector3TransformCoord(up, T);
			look = Math::Vector3TransformCoord(look, T);

			Correct(right, up, look);
			
			GetNode()->SetRotation(Math::Quaternion(Math::Matrix3(
				right.x_, up.x_, look.x_,
				right.y_, up.y_, look.y_,
				right.z_, up.z_, look.z_
				)));
		}

		void Camera::Yaw(float angle)
		{
			auto matrix = GetNode()->GetRotation().RotationMatrix();

			Math::Vector3 right(matrix.m00_, matrix.m10_, matrix.m20_);
			Math::Vector3 up(matrix.m01_, matrix.m11_, matrix.m21_);
			Math::Vector3 look(matrix.m02_, matrix.m12_, matrix.m22_);

			Math::Matrix4 T;

			if (cameraType_ == LAND_OBJECT)
			{
				T = Math::MatrixRotationY(angle);
			}

			if (cameraType_ == AIRCRAFT)
			{
				T = Math::MatrixRotationAxis(up, angle);
			}

			right = Math::Vector3TransformCoord(right, T);
			look = Math::Vector3TransformCoord(look, T);

			Correct(right, up, look);

			GetNode()->SetRotation(Math::Quaternion(Math::Matrix3(
				right.x_, up.x_, look.x_,
				right.y_, up.y_, look.y_,
				right.z_, up.z_, look.z_
				)));
		}

		void Camera::Roll(float angle)
		{
			if (cameraType_ == AIRCRAFT)
			{
				auto matrix = GetNode()->GetRotation().RotationMatrix();

				Math::Vector3 right(matrix.m00_, matrix.m10_, matrix.m20_);
				Math::Vector3 up(matrix.m01_, matrix.m11_, matrix.m21_);
				Math::Vector3 look(matrix.m02_, matrix.m12_, matrix.m22_);

				Math::Matrix4 T = Math::MatrixRotationAxis(look, angle);
				right = Math::Vector3TransformCoord(right, T);
				up = Math::Vector3TransformCoord(up, T);

				Correct(right, up, look);

				GetNode()->SetRotation(Math::Quaternion(Math::Matrix3(
					right.x_, up.x_, look.x_,
					right.y_, up.y_, look.y_,
					right.z_, up.z_, look.z_
					)));
			}
		}

		Math::Matrix3x4 Camera::GetViewMatrix()
		{
			Math::Matrix3x4 transform = node_ ? node_->GetWorldTransform() : Math::Matrix3x4::IDENTITY;
			return transform.Inverse();
		}

		Math::Matrix4 Camera::GetProjectionMatrix()
		{
			Math::Matrix4 projection = Math::Matrix4::ZERO;

			float h = (1.0f / tanf(fov_ * (Math::PI / 180.0f) * 0.5f)) * zoom_;
			float w = h / aspect_;
			float q = farClip_ / (farClip_ - nearClip_);
			float r = -q * nearClip_;

			projection.m00_ = w;
			projection.m02_ = projOffset_.x_ * 2.0f;
			projection.m11_ = h;
			projection.m12_ = projOffset_.y_ * 2.0f;
			projection.m22_ = q;
			projection.m23_ = r;
			projection.m32_ = 1.0f;

			return projection;
		}

		CameraType Camera::GetCameraType() const
		{
			return cameraType_;
		}

		void Camera::SetCameraType(CameraType cameraType)
		{
			cameraType_ = cameraType;
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

		void Camera::SetAspect(float aspect)
		{
			aspect_ = aspect;
		}

		float Camera::GetAspect() const
		{
			return aspect_;
		}

		void Camera::SetZoom(float zoom)
		{
			zoom_ = zoom;
		}

		float Camera::GetZoom() const
		{
			return zoom_;
		}

		void Camera::SetFov(float fov)
		{
			fov_ = fov;
		}

		float Camera::GetFov() const
		{
			return fov_;
		}

		Math::Vector3 Camera::GetRight() const
		{
			return GetNode()->GetWorldRotation() * Math::Vector3::RIGHT;
		}

		Math::Vector3 Camera::GetUp() const
		{
			return GetNode()->GetWorldRotation() * Math::Vector3::UP;
		}

		Math::Vector3 Camera::GetLook() const
		{
			return GetNode()->GetWorldRotation() * Math::Vector3::FORWARD;
		}

		bool Camera::IsProjectionValid() const
		{
			return GetFarClip() > GetNearClip();
		}

		Math::Ray Camera::GetScreenRay(float x, float y)
		{
			Math::Ray ret;
			if (!IsProjectionValid())
			{
				ret.origin_ = node_ ? node_->GetWorldPosition() : Math::Vector3::ZERO;
				ret.direction_ = node_ ? node_->GetWorldDirection() : Math::Vector3::FORWARD;
				return ret;
			}

			Math::Matrix4 viewProjInverse = (GetProjectionMatrix() * GetViewMatrix()).Inverse();

			x = 2.0f * x - 1.0f;
			y = 1.0f - 2.0f * y;
			Math::Vector3 nearPos(x, y, 0.0f);
			Math::Vector3 farPos(x, y, 0.01f);

			ret.origin_ = viewProjInverse * nearPos;
			ret.direction_ = ((viewProjInverse * farPos) - ret.origin_).Normalized();
			return ret;
		}

		Math::Vector3 Camera::ScreenPosToWorldPos(const Math::Vector3& screenPos)
		{
			return Math::Vector3::ZERO;
		}

		Math::Vector2 Camera::WorldPosToScreenPos(const Math::Vector3& worldPos)
		{
			return Math::Vector2::ZERO;
		}
	}
}
