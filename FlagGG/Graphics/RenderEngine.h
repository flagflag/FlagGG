#pragma once

#include "Export.h"
#include "Graphics/GraphicsDef.h"
#include "Graphics/Viewport.h"
#include "Graphics/RenderContext.h"
#include "Graphics/ShaderParameter.h"
#include "Graphics/Batch.h"
#include "Scene/Camera.h"
#include "Resource/Image.h"
#include "Container/HashMap.h"
#include "Utility/Singleton.h"
#include "AsyncFrame/Mutex.h"
#include "Core/Context.h"

#include <stdio.h>
#include <stdint.h>

namespace FlagGG
{

class GfxDevice;

class FlagGG_API RenderEngine : public Singleton<RenderEngine, NullMutex, Context*>
{
public:
	RenderEngine(Context* context);

	void Initialize();

	void Uninitialize();

	void SetTextureQuality(MaterialQuality quality);

	MaterialQuality GetTextureQuality();

	static TextureFormat GetFormat(CompressedFormat format);

	/// Return the API-specific alpha texture format.
	static TextureFormat GetAlphaFormat();
	/// Return the API-specific luminance texture format.
	static TextureFormat GetLuminanceFormat();
	/// Return the API-specific luminance alpha texture format.
	static TextureFormat GetLuminanceAlphaFormat();
	/// Return the API-specific RGB texture format.
	static TextureFormat GetRGBFormat();
	/// Return the API-specific RGBA texture format.
	static TextureFormat GetRGBAFormat();
	/// Return the API-specific RGBA 16-bit texture format.
	static TextureFormat GetRGBA16Format();
	/// Return the API-specific RGBA 16-bit float texture format.
	static TextureFormat GetRGBAFloat16Format();
	/// Return the API-specific RGBA 32-bit float texture format.
	static TextureFormat GetRGBAFloat32Format();
	/// Return the API-specific RG 16-bit texture format.
	static TextureFormat GetRG16Format();
	/// Return the API-specific RG 16-bit float texture format.
	static TextureFormat GetRGFloat16Format();
	/// Return the API-specific RG 32-bit float texture format.
	static TextureFormat GetRGFloat32Format();
	/// Return the API-specific single channel 16-bit float texture format.
	static TextureFormat GetFloat16Format();
	/// Return the API-specific single channel 32-bit float texture format.
	static TextureFormat GetFloat32Format();
	/// Return the API-specific linear depth texture format.
	static TextureFormat GetLinearDepthFormat();
	/// Return the API-specific hardware depth-stencil texture format.
	static TextureFormat GetDepthStencilFormat();
	/// Return the API-specific readable hardware depth format, or 0 if not supported.
	static TextureFormat GetReadableDepthFormat();
	/// Return the API-specific texture format from a textual description, for example "rgb".
	static TextureFormat GetFormat(const String& formatName);

	// 最大骨骼数
	static UInt32 GetMaxBonesNum();

	ShaderParameters& GetShaderParameters();

	void PostRenderBatch(const Vector<SharedPtr<Batch>>& batches);

	void SetDefaultTextures(TextureClass index, Texture* texture);

	void SetRasterizerState(RasterizerState rasterizerState);

	void SetShaderParameter(Camera* camera, const RenderContext* renderContext);

	void SetVertexBuffers(const Vector<SharedPtr<VertexBuffer>>& vertexBuffers);

	void SetIndexBuffer(IndexBuffer* indexBuffer);

	void SetVertexShader(Shader* shader);

	void SetPixelShader(Shader* shader);

	void SetTextures(const Vector<SharedPtr<Texture>>& textures);

	void SetPrimitiveType(PrimitiveType primitiveType);

	void SetRenderTarget(Viewport* viewport, bool renderShadowMap = false);

	void PreDraw();

	void DrawCallIndexed(UInt32 indexStart, UInt32 indexCount);

	void DrawCall(UInt32 vertexStart, UInt32 vertexCount);

	void Render(Viewport* viewport);

	void RenderBatch(Viewport* viewport);

private:
	void CreateShadowRasterizerState();

	RasterizerState shadowRasterizerState_;

	const Matrix3x4* skinMatrix_{ nullptr };
	UInt32 numSkinMatrix_{ 0u };

	MaterialQuality textureQuality_{ QUALITY_HIGH };

	SharedPtr<Shader> vertexShader_{ nullptr };
	bool vertexShaderDirty_{ false };

	SharedPtr<Shader> pixelShader_{ nullptr };
	bool pixelShaderDirty_{ false };

	SharedPtr<Texture> defaultTextures_[MAX_TEXTURE_CLASS];

	SharedPtr<ShaderParameters> shaderParameters_;
	SharedPtr<ShaderParameters> inShaderParameters_;

	bool renderShadowMap_{ false };

	Vector<SharedPtr<Batch>> batches_;

	Context* context_;
	GfxDevice* gfxDevice_;
};

}
