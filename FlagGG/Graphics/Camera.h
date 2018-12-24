#ifndef __CAMERA__
#define __CAMERA__

#include <d3dx11.h>
#include <d3dx9.h>

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
			Camera() = default;

			Camera(CameraType cameraType);

			virtual ~Camera() = default;

			void Strafe(float units);
			void fly(float units);
			void walk(float units);

			void pitch(float angle);
			void yaw(float angle);
			void roll(float angle);

			void GetViewMatrix(D3DXMATRIX* V);
			void SetCameraType(CameraType cameraType);
			void GetPosition(D3DXVECTOR3* pos);
			void SetPosition(D3DXVECTOR3* pos);

			void GetRight(D3DXVECTOR3* right);
			void GetUp(D3DXVECTOR3* up);
			void GetLook(D3DXVECTOR3* look);

		private:
			CameraType cameraType_;

			D3DXVECTOR3 right_;
			D3DXVECTOR3 up_;
			D3DXVECTOR3 look_;
			D3DXVECTOR3 pos_;
		};
	}
}

#endif