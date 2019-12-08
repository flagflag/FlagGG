#pragma once

#include "Export.h"
#include "Math/Vector3.h"
#include "Math/Matrix4.h"
#include "Math/Matrix3x4.h"
#include "Math/Quaternion.h"
#include "Math/Ray.h"
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
			void Strafe(float units);
			void Fly(float units);
			void Walk(float units);

			void Pitch(float angle);
			void Yaw(float angle);
			void Roll(float angle);

			Math::Matrix3x4 GetViewMatrix();
			Math::Matrix4 GetProjectionMatrix();

			CameraType GetCameraType() const;
			void SetCameraType(CameraType cameraType);

			// 远裁剪面
			void SetFarClip(float farClip);
			float GetFarClip() const;

			// 近裁剪面
			void SetNearClip(float nearClip);
			float GetNearClip() const;

			// 设置横纵比例
			void SetAspect(float aspect);
			float GetAspect() const;

			// 设置透视范围基础距离
			void SetZoom(float zoom);
			float GetZoom() const;

			// 设置透视范围角度
			void SetFov(float fov);
			float GetFov() const;

			Math::Vector3 GetRight() const;
			Math::Vector3 GetUp() const;
			Math::Vector3 GetLook() const;

			bool IsProjectionValid() const;

			Math::Ray GetScreenRay(float x, float y);
			Math::Vector3 ScreenPosToWorldPos(const Math::Vector3& screenPos);
			Math::Vector2 WorldPosToScreenPos(const Math::Vector3& worldPos);

		protected:
			void Correct(Math::Vector3& right, Math::Vector3& up, Math::Vector3& look);

		private:
			CameraType cameraType_{ LAND_OBJECT };

			float farClip_{ 1.0f };
			float nearClip_{ 100.0f };
			float aspect_{ 1.0f };
			float fov_{ 45.0f };
			float zoom_{ 1.0f };
			Math::Vector2 projOffset_{ Math::Vector2::ZERO };
		};
	}
}
