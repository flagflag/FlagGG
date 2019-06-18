#ifndef __MATERIAL__
#define __MATERIAL__

#include "Export.h"
#include "Graphics/GraphicsDef.h"
#include "Resource/Resource.h"
#include "Math/Color.h"
#include "Container/Ptr.h"
#include "Core/Context.h"

namespace FlagGG
{
	namespace Graphics
	{
		class Texture;
		class Shader;

		class FlagGG_API Material : public Resource::Resource
		{
		public:
			Material(Core::Context* context);

			~Material() override = default;

			Container::SharedPtr<Texture> GetTexture();

			Container::SharedPtr<Texture> GetTexture(uint32_t index);

			Container::SharedPtr<Shader> GetVSShader();

			Container::SharedPtr<Shader> GetPSShader();

		protected:
			bool BeginLoad(IOFrame::Buffer::IOBuffer* stream) override;

			bool EndLoad() override;

		protected:
			Container::SharedPtr<Texture> textures_[MAX_TEXTURE_CLASS];

			Container::SharedPtr<Shader> vsShader_;

			Container::SharedPtr<Shader> psShader_;
		};
	}
}

#endif
