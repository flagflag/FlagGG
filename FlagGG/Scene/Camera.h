#pragma once

#include "Export.h"
#include "Math/Vector3.h"
#include "Math/Matrix4.h"
#include "Math/Matrix3x4.h"
#include "Math/Quaternion.h"
#include "Math/Ray.h"
#include "Math/Plane.h"
#include "Scene/Component.h"

namespace FlagGG
{
	namespace Scene
	{
		enum CameraType
		{
			LAND_OBJECT = 0,
			AIRCRAFT,
		};

		class FlagGG_API Camera : public Component
		{
			OBJECT_OVERRIDE(Camera);
		public:
			Camera();

			void Strafe(Real units);
			void Fly(Real units);
			void Walk(Real units);

			void Pitch(Real angle);
			void Yaw(Real angle);
			void Roll(Real angle);

			Math::Matrix3x4 GetViewMatrix();
			Math::Matrix4 GetProjectionMatrix();

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

			Math::Vector3 GetRight() const;
			Math::Vector3 GetUp() const;
			Math::Vector3 GetLook() const;

			bool IsProjectionValid() const;

			Math::Ray GetScreenRay(Real x, Real y);
			Math::Vector3 ScreenPosToWorldPos(const Math::Vector3& screenPos);
			Math::Vector2 WorldPosToScreenPos(const Math::Vector3& worldPos);

			void SetUseReflection(bool useReflection);
			bool GetUseReflection() const;

			void SetReflectionPlane(const Math::Plane& plane);

		protected:
			void Correct(Math::Vector3& right, Math::Vector3& up, Math::Vector3& look);

		private:
			CameraType cameraType_{ LAND_OBJECT };

			Real farClip_{ 1.0f };
			Real nearClip_{ 100.0f };
			Real aspect_{ 1.0f };
			Real fov_{ 45.0f };
			Real zoom_{ 1.0f };
			Math::Vector2 projOffset_{ Math::Vector2::ZERO };

			bool useReflection_{ false };
			Math::Plane reflectionPlane_{ Math::Plane::UP };
			Math::Matrix3x4 reflectionMatrix_{ Math::Matrix3x4::IDENTITY };
		};
	}
}
