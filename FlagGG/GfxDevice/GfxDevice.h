#pragma once

#include "Graphics/GraphicsDef.h"
#include "Graphics/ShaderParameter.h"
#include "Container/Vector.h"
#include "Container/Ptr.h"
#include "Math/Rect.h"

namespace FlagGG
{

class GfxBuffer;
class GfxTexture;
class GfxShader;
class GfxProgram;
class GfxRenderSurface;
class VertexDescription;

class GfxDevice
{
public:
	explicit GfxDevice();

	virtual ~GfxDevice();

	/**********************************************************/
	/*                        渲染指令                        */
	/**********************************************************/

	// 设置RenderSurface
	virtual void SetRenderSurface(GfxRenderSurface* gfxRenderSuraface);

	// 设置VertexBuffer
	virtual void SetVertexBuffer(GfxBuffer* gfxVertexBuffer);

	// 清空顶点buffer设置
	virtual void ClearVertexBuffer();

	// 增加VertexBuffer
	virtual void AddVertexBuffer(GfxBuffer* gfxVertexBuffer);

	// 设置IndexBuffer
	virtual void SetIndexBuffer(GfxBuffer* gfxIndexBuffer);

	// 设置顶点描述
	virtual void SetVertexDescription(VertexDescription* vertexDesc);

	// 在slotID通道绑定纹理
	virtual void SetTexture(UInt32 slotID, GfxTexture* gfxTexture);

	// 设置shaders
	virtual void SetShaders(GfxShader* vertexShader, GfxShader* pixelShader);

	// 设置有引擎shader参数
	virtual void SetEngineShaderParameters(ShaderParameters* engineShaderParameters);

	// 设置材质shader参数
	virtual void SetMaterialShaderParameters(ShaderParameters* materialShaderParameters);

	// 设置图元类型
	virtual void SetPrimitiveType(PrimitiveType primitiveType);

	// 设置混合模式
	virtual void SetBlendMode(BlendMode blendMode);

	// 设置裁剪模式
	virtual void SetCullMode(CullMode cullMode);

	// 设置填充模式
	virtual void SetFillMode(FillMode fillMode);

	// 设置深度写
	virtual void SetDepthWrite(bool depthWrite);

	// 设置裁剪测试
	virtual void SetScissorTest(bool scissorTest, const IntRect& rect);

	// 提交渲染指令
	virtual void Draw(UInt32 vertexStart, UInt32 vertexCount);

	// 提交渲染指令
	virtual void DrawIndexed(UInt32 indexStart, UInt32 indexCount, UInt32 vertexStart = 0u);


	/**********************************************************/
	/*                     创建Gfx对象                        */
	/**********************************************************/

	// 创建纹理
	virtual GfxTexture* CreateTexture();

	// 创建buffer
	virtual GfxBuffer* CreateBuffer();

	// 创建shader
	virtual GfxShader* CreateShader();

	// 创建gpu program
	virtual GfxProgram* CreateProgram();

public:
	// 获取d3d11设备
	static GfxDevice* GetDevice();

protected:
	// 准备提交的vbs
	Vector<SharedPtr<GfxBuffer>> vertexBuffers_;
	bool vertexBufferDirty_{};

	// 准备提交的ib
	SharedPtr<GfxBuffer> indexBuffer_;
	bool indexBufferDirty_{};

	// vbs desc
	SharedPtr<VertexDescription> vertexDesc_;
	bool vertexDescDirty_{};

	// 准备提交的shaders
	SharedPtr<GfxShader> vertexShader_;
	SharedPtr<GfxShader> pixelShader_;
	bool shaderDirty_{};

	// 准备提交的texture
	SharedPtr<GfxTexture> textures_[MAX_TEXTURE_CLASS];
	bool texturesDirty_{};

	// 光栅化状态
	RasterizerState rasterizerState_;
	// 光珊化状态dirty
	bool rasterizerStateDirty_{};

	// 图元类型
	PrimitiveType primitiveType_{ PRIMITIVE_TRIANGLE };

	// shader参数
	SharedPtr<ShaderParameters> engineShaderParameters_;
	SharedPtr<ShaderParameters> materialShaderParameters_;
};

}
