#ifndef __MATERIAL__
#define __MATERIAL__

#include "Export.h"
#include "Graphics/GraphicsDef.h"
#include "Graphics/ShaderParameter.h"
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

		struct FlagGG_API RenderPass
		{
			Container::SharedPtr<Shader> vertexShader_;
			Container::SharedPtr<Shader> pixelShader_;
		};

		class FlagGG_API Material : public Resource::Resource
		{
		public:
			Material(Core::Context* context);

			~Material() override = default;

			Container::SharedPtr<Texture> GetTexture();

			Container::SharedPtr<Texture> GetTexture(uint32_t index);

			Container::SharedPtr<Shader> GetVertexShader();

			Container::SharedPtr<Shader> GetPixelShader();

			Container::HashMap<uint32_t, RenderPass>& GetRenderPass();

		protected:
			bool BeginLoad(IOFrame::Buffer::IOBuffer* stream) override;

			bool EndLoad() override;

		protected:
			Container::SharedPtr<Texture> textures_[MAX_TEXTURE_CLASS];

			Container::SharedPtr<Shader> vsShader_;

			Container::SharedPtr<Shader> psShader_;

			Container::SharedPtr<ShaderParameters> shaderParameters_;

			Container::HashMap<uint32_t, RenderPass> renderPass_;
		};
	}
}

#endif
