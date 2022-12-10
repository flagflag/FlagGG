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

class Texture;
class Shader;

struct FlagGG_API RenderPass
{
	SharedPtr<Shader> vertexShader_;
	SharedPtr<Shader> pixelShader_;
};

class FlagGG_API Material : public Resource
{
public:
	Material(Context* context);

	~Material() override = default;

	void SetTexture(Texture* texture);

	void SetTexture(TextureClass textureClass, Texture* texture);

	void SetVertexShader(Shader* vertexShader);

	void SetPixelShader(Shader* pixelShader);

	void SetRenderPass(RenderPassType type, const RenderPass& renderPass);

	void SetFillMode(FillMode fillMode);

	void SetCullMode(CullMode cullMode);

	void SetDepthWrite(bool depthWrite);


	SharedPtr<Texture> GetTexture();

	SharedPtr<Texture> GetTexture(UInt32 index);

	SharedPtr<Shader> GetVertexShader();

	SharedPtr<Shader> GetPixelShader();

	HashMap<UInt32, RenderPass>& GetRenderPass();

	SharedPtr<ShaderParameters> GetShaderParameters();

	FillMode GetFillMode() const;

	CullMode GetCullMode() const;

	bool GetDepthWrite() const;

	const RasterizerState GetRasterizerState() const;

	void CreateShaderParameters();

protected:
	bool BeginLoad(IOFrame::Buffer::IOBuffer* stream) override;

	bool EndLoad() override;

protected:
	SharedPtr<Texture> textures_[MAX_TEXTURE_CLASS];

	SharedPtr<Shader> vsShader_;

	SharedPtr<Shader> psShader_;

	SharedPtr<ShaderParameters> shaderParameters_;

	HashMap<UInt32, RenderPass> renderPass_;

	RasterizerState rasterizerState_;
};

}

#endif
