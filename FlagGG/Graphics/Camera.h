#ifndef __CAMERA__
#define __CAMERA__

#include "Math/Vector3.h"
#include "Math/Matrix4.h"

namespace FlagGG
{
	namespace Graphics
	{
		enum CameraType
		{
			LAND_OBJECT = 0,
			AIRCRAFT,
		};

		class Camera
		{
		public:
			Camera(CameraType cameraType);

			virtual ~Camera() = default;

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

			const Math::Vector3& GetPosition() const;
			void SetPosition(const Math::Vector3& pos);

			const Math::Vector3& GetRight() const;
			const Math::Vector3& GetUp() const;
			const Math::Vector3& GetLook() const;

		private:
			CameraType cameraType_;

			Math::Vector3 pos_;
			Math::Vector3 right_;
			Math::Vector3 up_;
			Math::Vector3 look_;		
		};
	}
}

#endif