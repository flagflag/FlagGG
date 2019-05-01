#ifndef __TEXTURE__
#define __TEXTURE__

#include "Export.h"

#include "Graphics/GPUObject.h"
#include "Resource/Resource.h"

namespace FlagGG
{
	namespace Graphics
	{
		//Base Class, ID3D11Texture2D or ID3D11Texture3D
		class FlagGG_API Texture : public GPUObject, public Resource::Resource
		{
		public:
			Texture(Core::Context* context);

			~Texture() override;

			bool IsValid() override;

			virtual void SetData(int x, int y, unsigned width, unsigned height, const void* data) = 0;

			friend class WinViewport;

		protected:
			void Initialize() override;

			bool BeginLoad(IOFrame::Buffer::IOBuffer* stream) override;

			bool EndLoad() override;

			virtual bool Create(ID3D11Resource*& resource, ID3D11ShaderResourceView*& resourceView) = 0;

		private:
			ID3D11ShaderResourceView* shaderResourceView_{ nullptr };

			ID3D11SamplerState* sampler_{ nullptr };
		};
	}
}

#endif