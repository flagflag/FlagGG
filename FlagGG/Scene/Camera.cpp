#include "Scene/Camera.h"
#include "Scene/Node.h"

namespace FlagGG
{
	namespace Scene
	{
		Camera::Camera() :
			Component()
		{
			viewMask_ = Math::F_MAX_UNSIGNED;
			reflectionMatrix_ = reflectionPlane_.ReflectionMatrix();
		}

		void Camera::Strafe(Real units)
		{
			Node* master = GetNode();
			if (!master)
				return;

			if (cameraType_ == LAND_OBJECT)
			{
				Math::Vector3 right = GetRight();
				master->SetWorldPosition(master->GetWorldPosition() + Math::Vector3(right.x_, 0.0f, right.z_) * units);
			}

			if (cameraType_ == AIRCRAFT)
			{
				Math::Vector3 right = GetRight();
				master->SetWorldPosition(master->GetWorldPosition() + right * units);
			}
		}

		void Camera::Fly(Real units)
		{
			Node* master = GetNode();
			if (!master)
				return;

			if (cameraType_ == LAND_OBJECT)
			{			
				master->SetWorldPosition(master->GetWorldPosition() + Math::Vector3(0.0f, units, 0.0f));
			}

			if (cameraType_ == AIRCRAFT)
			{
				Math::Vector3 up = GetUp();
				master->SetWorldPosition(master->GetWorldPosition() + up * units);
			}
		}

		void Camera::Walk(Real units)
		{
			Node* master = GetNode();
			if (!master)
				return;

			if (cameraType_ == LAND_OBJECT)
			{			
				Math::Vector3 look = GetLook();
				master->SetWorldPosition(master->GetWorldPosition() + Math::Vector3(look.x_, 0.0f, look.z_) * units);
			}

			if (cameraType_ == AIRCRAFT)
			{
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

		void Camera::Pitch(Real angle)
		{
			Node* master = GetNode();
			if (!master)
				return;

			auto matrix = master->GetRotation().RotationMatrix();

			Math::Vector3 right(matrix.m00_, matrix.m10_, matrix.m20_);
			Math::Vector3 up(matrix.m01_, matrix.m11_, matrix.m21_);
			Math::Vector3 look(matrix.m02_, matrix.m12_, matrix.m22_);

			Math::Matrix4 T = Math::MatrixRotationAxis(right, angle);
			up = Math::Vector3TransformCoord(up, T);
			look = Math::Vector3TransformCoord(look, T);

			Correct(right, up, look);
			
			master->SetRotation(Math::Quaternion(Math::Matrix3(
				right.x_, up.x_, look.x_,
				right.y_, up.y_, look.y_,
				right.z_, up.z_, look.z_
				)));
		}

		void Camera::Yaw(Real angle)
		{
			Node* master = GetNode();
			if (!master)
				return;

			auto matrix = master->GetRotation().RotationMatrix();

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

			master->SetRotation(Math::Quaternion(Math::Matrix3(
				right.x_, up.x_, look.x_,
				right.y_, up.y_, look.y_,
				right.z_, up.z_, look.z_
				)));
		}

		void Camera::Roll(Real angle)
		{
			if (cameraType_ == AIRCRAFT)
			{
				Node* master = GetNode();
				if (!master)
					return;

				auto matrix = master->GetRotation().RotationMatrix();

				Math::Vector3 right(matrix.m00_, matrix.m10_, matrix.m20_);
				Math::Vector3 up(matrix.m01_, matrix.m11_, matrix.m21_);
				Math::Vector3 look(matrix.m02_, matrix.m12_, matrix.m22_);

				Math::Matrix4 T = Math::MatrixRotationAxis(look, angle);
				right = Math::Vector3TransformCoord(right, T);
				up = Math::Vector3TransformCoord(up, T);

				Correct(right, up, look);

				master->SetRotation(Math::Quaternion(Math::Matrix3(
					right.x_, up.x_, look.x_,
					right.y_, up.y_, look.y_,
					right.z_, up.z_, look.z_
					)));
			}
		}

		Math::Matrix3x4 Camera::GetViewMatrix()
		{
			Math::Matrix3x4 transform = node_ ? Math::Matrix3x4(node_->GetWorldPosition(), node_->GetWorldRotation(), 1.0f) : Math::Matrix3x4::IDENTITY;
			return useReflection_ ? (reflectionMatrix_ * transform).Inverse() : transform.Inverse();
		}

		Math::Matrix4 Camera::GetProjectionMatrix()
		{
			Math::Matrix4 projection = Math::Matrix4::ZERO;

			if (!orthographic_)
			{
				Real h = (1.0f / tanf(fov_ * (Math::PI / 180.0f) * 0.5f)) * zoom_;
				Real w = h / aspect_;
				Real q = farClip_ / (farClip_ - nearClip_);
				Real r = -q * nearClip_;

				projection.m00_ = w;
				projection.m02_ = projOffset_.x_ * 2.0f;
				projection.m11_ = h;
				projection.m12_ = projOffset_.y_ * 2.0f;
				projection.m22_ = q;
				projection.m23_ = r;
				projection.m32_ = 1.0f;
			}
			else
			{
				Real h = (1.0f / (orthoSize_ * 0.5f)) * zoom_;
				Real w = h / aspect_;
				Real q = 1.0f / farClip_;
				Real r = 0.0f;

				projection.m00_ = w;
				projection.m03_ = projOffset_.x_ * 2.0f;
				projection.m11_ = h;
				projection.m13_ = projOffset_.y_ * 2.0f;
				projection.m22_ = q;
				projection.m23_ = r;
				projection.m33_ = 1.0f;
			}

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

		void Camera::SetFarClip(Real farClip)
		{
			farClip_ = farClip;
		}

		Real Camera::GetFarClip() const
		{
			return farClip_;
		}

		void Camera::SetNearClip(Real nearClip)
		{
			nearClip_ = nearClip;
		}

		Real Camera::GetNearClip() const
		{
			return nearClip_;
		}

		void Camera::SetAspect(Real aspect)
		{
			aspect_ = aspect;
		}

		Real Camera::GetAspect() const
		{
			return aspect_;
		}

		void Camera::SetZoom(Real zoom)
		{
			zoom_ = zoom;
		}

		Real Camera::GetZoom() const
		{
			return zoom_;
		}

		void Camera::SetFov(Real fov)
		{
			fov_ = fov;
		}

		Real Camera::GetFov() const
		{
			return fov_;
		}

		void Camera::SetOrthographic(bool enable)
		{
			orthographic_ = enable;
		}

		bool Camera::GetOrthographics() const
		{
			return orthographic_;
		}

		void Camera::SetOrthoSize(float orthoSize)
		{
			orthoSize_ = orthoSize;
		}

		float Camera::GetOrthoSize() const
		{
			return orthoSize_;
		}

		Math::Vector3 Camera::GetRight() const
		{
			Node* master = GetNode();
			return master ? master->GetWorldRotation() * Math::Vector3::RIGHT : Math::Vector3::ZERO;
		}

		Math::Vector3 Camera::GetUp() const
		{
			Node* master = GetNode();
			return master ? master->GetWorldRotation() * Math::Vector3::UP : Math::Vector3::ZERO;
		}

		Math::Vector3 Camera::GetLook() const
		{
			Node* master = GetNode();
			return master ? master->GetWorldRotation() * Math::Vector3::FORWARD : Math::Vector3::ZERO;
		}

		bool Camera::IsProjectionValid() const
		{
			return GetFarClip() > GetNearClip();
		}

		Math::Ray Camera::GetScreenRay(Real x, Real y)
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

		void Camera::SetUseReflection(bool useReflection)
		{
			useReflection_ = useReflection;
		}

		bool Camera::GetUseReflection() const
		{
			return useReflection_;
		}

		void Camera::SetReflectionPlane(const Math::Plane& plane)
		{
			reflectionPlane_ = plane;
			reflectionMatrix_ = reflectionPlane_.ReflectionMatrix();
		}
	}
}
