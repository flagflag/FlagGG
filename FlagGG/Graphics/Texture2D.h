#ifndef __TEXTURE2D__
#define __TEXTURE2D__

#include "Texture.h"

#include <string>

namespace FlagGG
{
	namespace Graphics
	{
#define USE_DDS

		class Texture2D : public Texture
		{
		public:
			Texture2D() = default;

			Texture2D(const std::wstring& texturePath);

			~Texture2D() override = default;

			void SetData(int x, int y, unsigned width, unsigned height, const void* data) override;

		private:
			bool Create(ID3D11Resource*& resource, ID3D11ShaderResourceView*& resourceView) override;

			std::wstring texturePath_;
		};
	}
}

#endif