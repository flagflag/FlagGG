#pragma once

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

struct FlagGG_API RenderPassInfo
{
	~RenderPassInfo();

	SharedPtr<Shader> vertexShader_;
	SharedPtr<Shader> pixelShader_;
};

class FlagGG_API Material : public Resource
{
	OBJECT_OVERRIDE(Material, Resource);
public:
	Material();

	~Material() override;

	// 设置纹理
	void SetTexture(Texture* texture);

	// 设置某个通道的纹理
	void SetTexture(TextureClass textureClass, Texture* texture);

	// 设置顶点shader
	void SetVertexShader(Shader* vertexShader);

	// 设置像素shader
	void SetPixelShader(Shader* pixelShader);

	// 设置render pass
	void SetRenderPass(RenderPassType type, const RenderPassInfo& renderPass);

	// 设置填充模式
	void SetFillMode(FillMode fillMode);

	// 设置裁剪模式
	void SetCullMode(CullMode cullMode);

	// 设置深度写
	void SetDepthWrite(bool depthWrite);


	// 获取纹理
	SharedPtr<Texture> GetTexture();

	// 获取通道的纹理
	SharedPtr<Texture> GetTexture(UInt32 index);

	// 获取顶点shader
	SharedPtr<Shader> GetVertexShader();

	// 获取像素shader
	SharedPtr<Shader> GetPixelShader();

	// 获取所有render pass
	HashMap<UInt32, RenderPassInfo>& GetRenderPass();

	// 获取材质参数
	SharedPtr<ShaderParameters> GetShaderParameters();

	// 获取填充模式
	FillMode GetFillMode() const;

	// 获取裁剪模式
	CullMode GetCullMode() const;

	// 获取深度写
	bool GetDepthWrite() const;

	// 获取光栅化状态
	const RasterizerState& GetRasterizerState() const { return rasterizerState_; }

	// 获取深度模板状态
	const DepthStencilState& GetDepthStencilState() const { return depthStencilState_; }

	// 创建材质参数集
	void CreateShaderParameters();

protected:
	bool BeginLoad(IOFrame::Buffer::IOBuffer* stream) override;

	bool EndLoad() override;

protected:
	SharedPtr<Texture> textures_[MAX_TEXTURE_CLASS];

	SharedPtr<Shader> vsShader_;

	SharedPtr<Shader> psShader_;

	SharedPtr<ShaderParameters> shaderParameters_;

	HashMap<UInt32, RenderPassInfo> renderPass_;

	RasterizerState rasterizerState_;

	DepthStencilState depthStencilState_;
};

}

