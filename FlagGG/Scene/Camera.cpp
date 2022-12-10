#include "Scene/Camera.h"
#include "Scene/Node.h"

namespace FlagGG
{

Camera::Camera() :
	Component()
{
	viewMask_ = F_MAX_UNSIGNED;
	reflectionMatrix_ = reflectionPlane_.ReflectionMatrix();
}

void Camera::Strafe(Real units)
{
	Node* master = GetNode();
	if (!master)
		return;

	if (cameraType_ == LAND_OBJECT)
	{
		Vector3 right = GetRight();
		master->SetWorldPosition(master->GetWorldPosition() + Vector3(right.x_, 0.0f, right.z_) * units);
	}

	if (cameraType_ == AIRCRAFT)
	{
		Vector3 right = GetRight();
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
		master->SetWorldPosition(master->GetWorldPosition() + Vector3(0.0f, units, 0.0f));
	}

	if (cameraType_ == AIRCRAFT)
	{
		Vector3 up = GetUp();
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
		Vector3 look = GetLook();
		master->SetWorldPosition(master->GetWorldPosition() + Vector3(look.x_, 0.0f, look.z_) * units);
	}

	if (cameraType_ == AIRCRAFT)
	{
		Vector3 look = GetLook();
		master->SetWorldPosition(master->GetWorldPosition() + look * units);
	}
}

void Camera::Correct(Vector3& right, Vector3& up, Vector3& look)
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

	Vector3 right(matrix.m00_, matrix.m10_, matrix.m20_);
	Vector3 up(matrix.m01_, matrix.m11_, matrix.m21_);
	Vector3 look(matrix.m02_, matrix.m12_, matrix.m22_);

	Matrix4 T = MatrixRotationAxis(right, angle);
	up = Vector3TransformCoord(up, T);
	look = Vector3TransformCoord(look, T);

	Correct(right, up, look);
			
	master->SetRotation(Quaternion(Matrix3(
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

	Vector3 right(matrix.m00_, matrix.m10_, matrix.m20_);
	Vector3 up(matrix.m01_, matrix.m11_, matrix.m21_);
	Vector3 look(matrix.m02_, matrix.m12_, matrix.m22_);

	Matrix4 T;

	if (cameraType_ == LAND_OBJECT)
	{
		T = MatrixRotationY(angle);
	}

	if (cameraType_ == AIRCRAFT)
	{
		T = MatrixRotationAxis(up, angle);
	}

	right = Vector3TransformCoord(right, T);
	look = Vector3TransformCoord(look, T);

	Correct(right, up, look);

	master->SetRotation(Quaternion(Matrix3(
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

		Vector3 right(matrix.m00_, matrix.m10_, matrix.m20_);
		Vector3 up(matrix.m01_, matrix.m11_, matrix.m21_);
		Vector3 look(matrix.m02_, matrix.m12_, matrix.m22_);

		Matrix4 T = MatrixRotationAxis(look, angle);
		right = Vector3TransformCoord(right, T);
		up = Vector3TransformCoord(up, T);

		Correct(right, up, look);

		master->SetRotation(Quaternion(Matrix3(
			right.x_, up.x_, look.x_,
			right.y_, up.y_, look.y_,
			right.z_, up.z_, look.z_
			)));
	}
}

Matrix3x4 Camera::GetViewMatrix()
{
	Matrix3x4 transform = node_ ? Matrix3x4(node_->GetWorldPosition(), node_->GetWorldRotation(), 1.0f) : Matrix3x4::IDENTITY;
	return useReflection_ ? (reflectionMatrix_ * transform).Inverse() : transform.Inverse();
}

Matrix4 Camera::GetProjectionMatrix()
{
	Matrix4 projection = Matrix4::ZERO;

	if (!orthographic_)
	{
		Real h = (1.0f / tanf(fov_ * (PI / 180.0f) * 0.5f)) * zoom_;
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

Vector3 Camera::GetRight() const
{
	Node* master = GetNode();
	return master ? master->GetWorldRotation() * Vector3::RIGHT : Vector3::ZERO;
}

Vector3 Camera::GetUp() const
{
	Node* master = GetNode();
	return master ? master->GetWorldRotation() * Vector3::UP : Vector3::ZERO;
}

Vector3 Camera::GetLook() const
{
	Node* master = GetNode();
	return master ? master->GetWorldRotation() * Vector3::FORWARD : Vector3::ZERO;
}

bool Camera::IsProjectionValid() const
{
	return GetFarClip() > GetNearClip();
}

Ray Camera::GetScreenRay(Real x, Real y)
{
	Ray ret;
	if (!IsProjectionValid())
	{
		ret.origin_ = node_ ? node_->GetWorldPosition() : Vector3::ZERO;
		ret.direction_ = node_ ? node_->GetWorldDirection() : Vector3::FORWARD;
		return ret;
	}

	Matrix4 viewProjInverse = (GetProjectionMatrix() * GetViewMatrix()).Inverse();

	x = 2.0f * x - 1.0f;
	y = 1.0f - 2.0f * y;
	Vector3 nearPos(x, y, 0.0f);
	Vector3 farPos(x, y, 0.01f);

	ret.origin_ = viewProjInverse * nearPos;
	ret.direction_ = ((viewProjInverse * farPos) - ret.origin_).Normalized();
	return ret;
}

Vector3 Camera::ScreenPosToWorldPos(const Vector3& screenPos)
{
	return Vector3::ZERO;
}

Vector2 Camera::WorldPosToScreenPos(const Vector3& worldPos)
{
	return Vector2::ZERO;
}

void Camera::SetUseReflection(bool useReflection)
{
	useReflection_ = useReflection;
}

bool Camera::GetUseReflection() const
{
	return useReflection_;
}

void Camera::SetReflectionPlane(const Plane& plane)
{
	reflectionPlane_ = plane;
	reflectionMatrix_ = reflectionPlane_.ReflectionMatrix();
}

}
