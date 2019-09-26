#pragma once

#include "Export.h"
#include "Math/Vector3.h"
#include "Math/Matrix4.h"
#include "Math/Quaternion.h"
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

			Math::Matrix4 GetViewMatrix();
			Math::Matrix4 GetProjectionMatrix();

			CameraType GetCameraType() const;
			void SetCameraType(CameraType cameraType);

			// Ô¶²Ã¼ôÃæ
			void SetFarClip(float farClip);
			float GetFarClip() const;

			// ½ü²Ã¼ôÃæ
			void SetNearClip(float nearClip);
			float GetNearClip() const;

			Math::Vector3 GetRight() const;
			Math::Vector3 GetUp() const;
			Math::Vector3 GetLook() const;

		private:
			CameraType cameraType_{ LAND_OBJECT };

			float farClip_{ 1.0f };
			float nearClip_{ 100.0f };
		};
	}
}
