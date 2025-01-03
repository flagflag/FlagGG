#include "Scene/Camera.h"
#include "Scene/Node.h"

namespace FlagGG
{

Camera::Camera()
	: DrawableComponent()
	, listenerNode_(this)
{
	viewMask_ = F_MAX_UNSIGNED;
	reflectionMatrix_ = reflectionPlane_.ReflectionMatrix();
}

Camera::~Camera()
{
	listenerNode_.RemoveFromList();
}

void Camera::OnAddToNode(Node* node)
{
	node->AddTransformListener(listenerNode_);
}

void Camera::OnRemoveFromNode(Node* node)
{
	listenerNode_.RemoveFromList();
}

void Camera::OnTransformChange()
{
	frustumDirty_ = true;
	projectionDirty_ = true;
	viewDirty_ = true;
}

void Camera::OnPositionChange()
{
	frustumDirty_ = true;
	projectionDirty_ = true;
	viewDirty_ = true;
}

void Camera::OnRotationChange()
{
	frustumDirty_ = true;
	projectionDirty_ = true;
	viewDirty_ = true;
}

void Camera::OnScaleChange()
{
	frustumDirty_ = true;
	projectionDirty_ = true;
	viewDirty_ = true;
}

void Camera::Strafe(Real units)
{
	Node* master = GetNode();
	if (!master)
		return;

	if (cameraType_ == LAND_OBJECT)
	{
		Vector3 right = GetRight();
		master->SetWorldPosition(master->GetWorldPosition() + Vector3(right.x_, right.y_, 0.0f) * units);
	}

	if (cameraType_ == AIRCRAFT)
	{
		Vector3 right = GetRight();
		master->SetWorldPosition(master->GetWorldPosition() + right * units);
	}

	projectionDirty_ = true;
	viewDirty_ = true;
	frustumDirty_ = true;
}

void Camera::Fly(Real units)
{
	Node* master = GetNode();
	if (!master)
		return;

	if (cameraType_ == LAND_OBJECT)
	{			
		master->SetWorldPosition(master->GetWorldPosition() + Vector3(0.0f, 0.0f, units));
	}

	if (cameraType_ == AIRCRAFT)
	{
		Vector3 up = GetUp();
		master->SetWorldPosition(master->GetWorldPosition() + up * units);
	}

	projectionDirty_ = true;
	viewDirty_ = true;
	frustumDirty_ = true;
}

void Camera::Walk(Real units)
{
	Node* master = GetNode();
	if (!master)
		return;

	if (cameraType_ == LAND_OBJECT)
	{			
		Vector3 look = GetLook();
		master->SetWorldPosition(master->GetWorldPosition() + Vector3(look.x_, look.y_, 0.0f) * units);
	}

	if (cameraType_ == AIRCRAFT)
	{
		Vector3 look = GetLook();
		master->SetWorldPosition(master->GetWorldPosition() + look * units);
	}

	projectionDirty_ = true;
	viewDirty_ = true;
	frustumDirty_ = true;
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

	Vector3 look(matrix.m00_, matrix.m10_, matrix.m20_);
	Vector3 right(matrix.m01_, matrix.m11_, matrix.m21_);
	Vector3 up(matrix.m02_, matrix.m12_, matrix.m22_);

	Matrix4 T = MatrixRotationAxis(right, angle);
	up = Vector3TransformCoord(up, T);
	look = Vector3TransformCoord(look, T);

	Correct(right, up, look);
			
	master->SetRotation(Quaternion(Matrix3(
		look.x_, right.x_, up.x_,
		look.y_, right.y_, up.y_,
		look.z_, right.z_, up.z_
		)));

	projectionDirty_ = true;
	viewDirty_ = true;
	frustumDirty_ = true;
}

void Camera::Yaw(Real angle)
{
	Node* master = GetNode();
	if (!master)
		return;

	auto matrix = master->GetRotation().RotationMatrix();

	Vector3 look(matrix.m00_, matrix.m10_, matrix.m20_);
	Vector3 right(matrix.m01_, matrix.m11_, matrix.m21_);
	Vector3 up(matrix.m02_, matrix.m12_, matrix.m22_);

	Matrix4 T;

	if (cameraType_ == LAND_OBJECT)
	{
		T = MatrixRotationZ(angle);
	}

	if (cameraType_ == AIRCRAFT)
	{
		T = MatrixRotationAxis(up, angle);
	}

	right = Vector3TransformCoord(right, T);
	look = Vector3TransformCoord(look, T);

	Correct(right, up, look);

	master->SetRotation(Quaternion(Matrix3(
		look.x_, right.x_, up.x_,
		look.y_, right.y_, up.y_,
		look.z_, right.z_, up.z_
		)));

	projectionDirty_ = true;
	viewDirty_ = true;
	frustumDirty_ = true;
}

void Camera::Roll(Real angle)
{
	if (cameraType_ == AIRCRAFT)
	{
		Node* master = GetNode();
		if (!master)
			return;

		auto matrix = master->GetRotation().RotationMatrix();

		Vector3 look(matrix.m00_, matrix.m10_, matrix.m20_);
		Vector3 right(matrix.m01_, matrix.m11_, matrix.m21_);
		Vector3 up(matrix.m02_, matrix.m12_, matrix.m22_);

		Matrix4 T = MatrixRotationAxis(look, angle);
		right = Vector3TransformCoord(right, T);
		up = Vector3TransformCoord(up, T);

		Correct(right, up, look);

		master->SetRotation(Quaternion(Matrix3(
			look.x_, right.x_, up.x_,
			look.y_, right.y_, up.y_,
			look.z_, right.z_, up.z_
			)));

		projectionDirty_ = true;
		viewDirty_ = true;
		frustumDirty_ = true;
	}
}

Matrix3x4 Camera::GetEffectiveWorldTransform() const
{
	static Quaternion zUpQua = Quaternion(Vector3(0.f, 0.f, 1.f), Vector3(1.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f)).Inverse();
	Matrix3x4 transform = node_ ? Matrix3x4(node_->GetWorldPosition(), isZUp_ ? node_->GetWorldRotation() * zUpQua : node_->GetWorldRotation(), 1.0f) : Matrix3x4::IDENTITY;
	return useReflection_ ? reflectionMatrix_ * transform : transform;
}

const Matrix3x4& Camera::GetViewMatrix()
{
	if (viewDirty_)
	{
		view_ = GetEffectiveWorldTransform().Inverse();
		viewDirty_ = false;
	}
	return view_;
}

void Camera::UpdateProjection() const
{
	projection_ = Matrix4::ZERO;

	if (!orthographic_)
	{
		Real h = (1.0f / tanf(fov_ * (PI / 180.0f) * 0.5f)) * zoom_;
		Real w = h / aspect_;
		Real q = farClip_ / (farClip_ - nearClip_);
		Real r = -q * nearClip_;

		projection_.m00_ = w;
		projection_.m02_ = projOffset_.x_ * 2.0f;
		projection_.m11_ = h;
		projection_.m12_ = projOffset_.y_ * 2.0f;
		projection_.m22_ = q;
		projection_.m23_ = r;
		projection_.m32_ = 1.0f;
	}
	else
	{
		Real h = (1.0f / (orthoSize_ * 0.5f)) * zoom_;
		Real w = h / aspect_;
		Real q = 1.0f / farClip_;
		Real r = 0.0f;

		projection_.m00_ = w;
		projection_.m03_ = projOffset_.x_ * 2.0f;
		projection_.m11_ = h;
		projection_.m13_ = projOffset_.y_ * 2.0f;
		projection_.m22_ = q;
		projection_.m23_ = r;
		projection_.m33_ = 1.0f;
	}

	projectionDirty_ = false;
}

const Matrix4& Camera::GetProjectionMatrix()
{
	if (projectionDirty_)
		UpdateProjection();
	return projection_;
}

const Frustum& Camera::GetFrustum() const
{
	// Use projection_ instead of GetProjection() so that Y-flip has no effect. Update first if necessary
	if (projectionDirty_)
		UpdateProjection();

	if (frustumDirty_)
	{	
		// If not using a custom projection, prefer calculating frustum from projection parameters instead of matrix
		// for better accuracy
		if (!orthographic_)
			frustum_.Define(fov_, aspect_, zoom_, GetNearClip(), GetFarClip(), GetEffectiveWorldTransform());
		else
			frustum_.DefineOrtho(orthoSize_, aspect_, zoom_, GetNearClip(), GetFarClip(), GetEffectiveWorldTransform());

		frustumDirty_ = false;
	}

	return frustum_;
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
	if (farClip_ != farClip)
	{
		farClip_ = farClip;

		projectionDirty_ = true;
		frustumDirty_ = true;
	}
}

Real Camera::GetFarClip() const
{
	return farClip_;
}

void Camera::SetNearClip(Real nearClip)
{
	if (nearClip_ != nearClip)
	{
		nearClip_ = nearClip;

		projectionDirty_ = true;
		frustumDirty_ = true;
	}
}

Real Camera::GetNearClip() const
{
	return nearClip_;
}

void Camera::SetAspect(Real aspect)
{
	if (aspect_ != aspect)
	{
		aspect_ = aspect;

		projectionDirty_ = true;
		frustumDirty_ = true;
	}
}

Real Camera::GetAspect() const
{
	return aspect_;
}

void Camera::SetZoom(Real zoom)
{
	if (zoom_ != zoom)
	{
		zoom_ = zoom;

		projectionDirty_ = true;
		frustumDirty_ = true;
	}
}

Real Camera::GetZoom() const
{
	return zoom_;
}

void Camera::SetFov(Real fov)
{
	if (fov_ != fov)
	{
		fov_ = fov;

		projectionDirty_ = true;
		frustumDirty_ = true;
	}
}

Real Camera::GetFov() const
{
	return fov_;
}

void Camera::SetOrthographic(bool enable)
{
	if (orthographic_ != enable)
	{
		orthographic_ = enable;

		projectionDirty_ = true;
		frustumDirty_ = true;
	}
}

bool Camera::IsOrthographic() const
{
	return orthographic_;
}

void Camera::SetOrthoSize(float orthoSize)
{
	if (orthoSize_ != orthoSize)
	{
		orthoSize_ = orthoSize;

		frustumDirty_ = true;
		projectionDirty_ = true;
	}
}

void Camera::SetOrthoSize(const Vector2& orthoSize)
{
	orthoSize_ = orthoSize.y_;
	aspect_ = orthoSize.x_ / orthoSize.y_;

	frustumDirty_ = true;
	projectionDirty_ = true;
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

void Camera::GetFrustumSize(Vector3& nearVec, Vector3& farVec) const
{
	Frustum viewSpaceFrustum = GetViewSpaceFrustum();
	nearVec = viewSpaceFrustum.vertices_[0];
	farVec = viewSpaceFrustum.vertices_[4];
}

Frustum Camera::GetViewSpaceFrustum() const
{
	if (projectionDirty_)
		UpdateProjection();

	Frustum ret;

	if (!orthographic_)
		ret.Define(fov_, aspect_, zoom_, GetNearClip(), GetFarClip());
	else
		ret.DefineOrtho(orthoSize_, aspect_, zoom_, GetNearClip(), GetFarClip());

	return ret;
}

void Camera::SetUseReflection(bool useReflection)
{
	if (useReflection_ != useReflection)
	{
		useReflection_ = useReflection;

		viewDirty_ = true;
		frustumDirty_ = true;
	}
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

void Camera::SetZUp(bool isZUp)
{
	isZUp_ = isZUp;
}

}
