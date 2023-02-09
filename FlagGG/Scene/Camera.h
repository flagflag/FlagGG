//
// 相机
//

#pragma once

#include "Export.h"
#include "Math/Vector3.h"
#include "Math/Matrix4.h"
#include "Math/Matrix3x4.h"
#include "Math/Quaternion.h"
#include "Math/Ray.h"
#include "Math/Plane.h"
#include "Math/Frustum.h"
#include "Scene/DrawableComponent.h"

namespace FlagGG
{

enum CameraType
{
	LAND_OBJECT = 0,
	AIRCRAFT,
};

class FlagGG_API Camera : public DrawableComponent
{
	OBJECT_OVERRIDE(Camera, DrawableComponent);
public:
	Camera();

	void Strafe(Real units);
	void Fly(Real units);
	void Walk(Real units);

	void Pitch(Real angle);
	void Yaw(Real angle);
	void Roll(Real angle);

	Matrix3x4 GetViewMatrix();
	Matrix4 GetProjectionMatrix();

	// 获取视椎体
	const Frustum& GetFrustum() const;

	Matrix3x4 GetEffectiveWorldTransform() const;

	CameraType GetCameraType() const;
	void SetCameraType(CameraType cameraType);

	// 远裁剪面
	void SetFarClip(Real farClip);
	Real GetFarClip() const;

	// 近裁剪面
	void SetNearClip(Real nearClip);
	Real GetNearClip() const;

	// 设置横纵比例
	void SetAspect(Real aspect);
	Real GetAspect() const;

	// 设置透视范围基础距离
	void SetZoom(Real zoom);
	Real GetZoom() const;

	// 设置透视范围角度
	void SetFov(Real fov);
	Real GetFov() const;

	// 设置正交模式
	void SetOrthographic(bool enable);
	bool GetOrthographics() const;

	void SetOrthoSize(float orthoSize);
	float GetOrthoSize() const;

	Vector3 GetRight() const;
	Vector3 GetUp() const;
	Vector3 GetLook() const;

	bool IsProjectionValid() const;

	Ray GetScreenRay(Real x, Real y);
	Vector3 ScreenPosToWorldPos(const Vector3& screenPos);
	Vector2 WorldPosToScreenPos(const Vector3& worldPos);

	void SetUseReflection(bool useReflection);
	bool GetUseReflection() const;

	void SetReflectionPlane(const Plane& plane);

protected:
	void Correct(Vector3& right, Vector3& up, Vector3& look);

	void UpdateProjection() const;

private:
	CameraType cameraType_{ LAND_OBJECT };

	mutable Frustum frustum_;
	mutable bool frustumDirty_{};

	mutable Matrix4 projection_;
	mutable bool projectionDirty_{};

	mutable Matrix3x4 view_;
	mutable bool viewDirty_{};

	Real farClip_{ 1.0f };
	Real nearClip_{ 100.0f };
	Real aspect_{ 1.0f };
	Real fov_{ 45.0f };
	Real zoom_{ 1.0f };
	Vector2 projOffset_{ Vector2::ZERO };

	bool orthographic_{ false };
	float orthoSize_{ 20.0f };

	bool useReflection_{ false };
	Plane reflectionPlane_{ Plane::UP };
	Matrix3x4 reflectionMatrix_{ Matrix3x4::IDENTITY };
};

}
