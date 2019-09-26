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
				master->SetPosition(master->GetPosition() + Math::Vector3(right.x_, 0.0f, right.z_) * units);
			}

			if (cameraType_ == AIRCRAFT)
			{
				Node* master = GetNode();
				Math::Vector3 right = GetRight();
				master->SetPosition(master->GetPosition() + right * units);
			}
		}

		void Camera::Fly(float units)
		{
			if (cameraType_ == LAND_OBJECT)
			{
				Node* master = GetNode();
				master->SetPosition(master->GetPosition() + Math::Vector3(0.0f, units, 0.0f));
			}

			if (cameraType_ == AIRCRAFT)
			{
				Node* master = GetNode();
				Math::Vector3 up = GetUp();
				master->SetPosition(master->GetPosition() + up * units);
			}
		}

		void Camera::Walk(float units)
		{
			if (cameraType_ == LAND_OBJECT)
			{
				Node* master = GetNode();
				Math::Vector3 look = GetLook();
				master->SetPosition(master->GetPosition() + Math::Vector3(look.x_, 0.0f, look.z_) * units);
			}

			if (cameraType_ == AIRCRAFT)
			{
				Node* master = GetNode();
				Math::Vector3 look = GetLook();
				master->SetPosition(master->GetPosition() + look * units);
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

		Math::Matrix4 Camera::GetViewMatrix()
		{
			auto matrix = GetNode()->GetRotation().RotationMatrix();

			Math::Vector3 right(matrix.m00_, matrix.m10_, matrix.m20_);
			Math::Vector3 up(matrix.m01_, matrix.m11_, matrix.m21_);
			Math::Vector3 look(matrix.m02_, matrix.m12_, matrix.m22_);

			const auto& pos = GetNode()->GetPosition();
			float x = -right.DotProduct(pos);
			float y = -up.DotProduct(pos);
			float z = -look.DotProduct(pos);

			return Math::Matrix4(
				matrix.m00_, matrix.m01_, matrix.m02_, 0.0f,
				matrix.m10_, matrix.m11_, matrix.m12_, 0.0f,
				matrix.m20_, matrix.m21_, matrix.m22_, 0.0f,
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
	}
}
