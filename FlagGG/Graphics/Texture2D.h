#ifndef __TEXTURE2D__
#define __TEXTURE2D__

#include "Graphics/Texture.h"

#include <string>

namespace FlagGG
{
	namespace Graphics
	{
#define USE_DDS

		class FlagGG_API Texture2D : public Texture
		{
		public:
			Texture2D(Core::Context* context);

			~Texture2D() override = default;

			void SetData(int x, int y, unsigned width, unsigned height, const void* data) override;

		private:
			bool Create(ID3D11Resource*& resource, ID3D11ShaderResourceView*& resourceView) override;

			std::wstring texturePath_;
		};
	}
}

#endif