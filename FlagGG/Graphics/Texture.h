#ifndef __TEXTURE__
#define __TEXTURE__

#include "GPUObject.h"

namespace FlagGG
{
	namespace Graphics
	{
		//Base Class, ID3D11Texture2D or ID3D11Texture3D
		class Texture : public GPUObject
		{
		public:
			~Texture() override;

			void Initialize() override;

			virtual void SetData(int x, int y, unsigned width, unsigned height, const void* data) = 0;

			friend class WinViewport;

		protected:
			virtual bool Create(ID3D11Resource*& resource, ID3D11ShaderResourceView*& resourceView) = 0;


		private:
			ID3D11ShaderResourceView* shaderResourceView_{ nullptr };

			ID3D11SamplerState* sampler_{ nullptr };
		};
	}
}

#endif