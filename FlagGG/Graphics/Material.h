#pragma once

#include "Export.h"
#include "Graphics/GraphicsDef.h"
#include "Graphics/ShaderParameter.h"
#include "Resource/Resource.h"
#include "Math/Color.h"
#include "Container/Ptr.h"
#include "Core/Context.h"
// 依赖太复杂了，前向声明导致构建错误，以后再查暂时include头文件
#include "Graphics/Shader.h"

namespace FlagGG
{

class Texture;
class Shader;
class LJSONValue;

class FlagGG_API RenderPassInfo
{
public:
	RenderPassInfo();

	~RenderPassInfo();

	// 设置填充模式
	void SetFillMode(FillMode fillMode);

	// 设置裁剪模式
	void SetCullMode(CullMode cullMode);

	// 设置深度写
	void SetDepthWrite(bool depthWrite);

	// 设置深度偏移
	void SetSlopeScaledDepthBias(float slopeScaledDepthBias);

	// 设置顶点着色器
	void SetVertexShader(Shader* shader);

	// 设置像素着色器
	void SetPixelShader(Shader* shader);

	// 设置光栅化状态
	void SetRasterizerState(const RasterizerState& rasterizerState);

	// 设置深度模板状态
	void SetDepthStencilState(const DepthStencilState& depthStencilState);


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

	// 获取顶点着色器
	Shader* GetVertexShader() const { return vertexShader_; }

	// 获取像素着色器
	Shader* GetPixelShader() const { return pixelShader_; }

private:
	RasterizerState rasterizerState_;
	DepthStencilState depthStencilState_;

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

	// 创建render pass
	RenderPassInfo& CreateOrGetRenderPass(RenderPassType type);


	// 获取纹理
	Texture* GetTexture();

	// 获取通道的纹理
	Texture* GetTexture(UInt32 index);

	// 获取顶点shader
	Shader* GetVertexShader();

	// 获取像素shader
	Shader* GetPixelShader();

	// 获取所有render pass
	HashMap<UInt32, RenderPassInfo>& GetRenderPass();

	// 获取材质参数
	SharedPtr<ShaderParameters> GetShaderParameters();

	// 创建材质参数集
	void CreateShaderParameters();

	// 拷贝一份材质
	SharedPtr<Material> Clone();

protected:
	bool BeginLoad(IOFrame::Buffer::IOBuffer* stream) override;

	bool EndLoad() override;

	void LoadRasterizerState(const LJSONValue& root, RasterizerState& state);

	void LoadDepthStencilState(const LJSONValue& root, DepthStencilState& state);

protected:
	SharedPtr<Texture> textures_[MAX_TEXTURE_CLASS];

	SharedPtr<Shader> vsShader_;

	SharedPtr<Shader> psShader_;

	SharedPtr<ShaderParameters> shaderParameters_;

	HashMap<UInt32, RenderPassInfo> renderPass_;
};

}

