#include "Graphics/RenderEngine.h"
#include "Graphics/Texture.h"
#include "Graphics/Texture2D.h"
#include "Scene/Node.h"
#include "Scene/Component.h"
#include "Scene/Light.h"
#include "Math/Math.h"
#include "Log.h"
#include "IOFrame/Buffer/IOBufferAux.h"
#include "Resource/ResourceCache.h"
#include "GfxDevice/GfxDevice.h"

#include <assert.h>

namespace FlagGG
{

RenderEngine::RenderEngine(Context* context) :
	context_(context)
{
	shaderParameters_ = new ShaderParameters();
	shaderParameters_->AddParametersDefine<Matrix3x4>(SP_WORLD_MATRIX);
	shaderParameters_->AddParametersDefine<Matrix3x4>(SP_VIEW_MATRIX);
	shaderParameters_->AddParametersDefine<Matrix4>(SP_PROJVIEW_MATRIX);
	shaderParameters_->AddParametersDefine<float>(SP_DELTA_TIME);
	shaderParameters_->AddParametersDefine<float>(SP_ELAPSED_TIME);
	shaderParameters_->AddParametersDefine<Vector3>(SP_CAMERA_POS);
	shaderParameters_->AddParametersDefine<Vector3>(SP_LIGHT_POS);
	shaderParameters_->AddParametersDefine<Vector3>(SP_LIGHT_DIR);
	shaderParameters_->AddParametersDefine<Matrix3x4>(SP_LIGHT_VIEW_MATRIX);
	shaderParameters_->AddParametersDefine<Matrix4>(SP_LIGHT_PROJVIEW_MATRIX);
}

void RenderEngine::CreateShadowRasterizerState()
{
	shadowRasterizerState_.depthWrite_ = true;
	shadowRasterizerState_.scissorTest_ = false;
	shadowRasterizerState_.fillMode_ = FILL_SOLID;
	shadowRasterizerState_.cullMode_ = CULL_FRONT;
}

void RenderEngine::Initialize()
{
	gfxDevice_ = GfxDevice::CreateDevice();

	CreateShadowRasterizerState();
}

void RenderEngine::Uninitialize()
{
	GfxDevice::DestroyDevice();
}

void RenderEngine::SetTextureQuality(MaterialQuality quality)
{
	textureQuality_ = quality;
}

MaterialQuality RenderEngine::GetTextureQuality()
{
	return textureQuality_;
}

TextureFormat RenderEngine::GetFormat(CompressedFormat format)
{
	//switch (format)
	//{
	//case CF_RGBA:
	//	return DXGI_FORMAT_R8G8B8A8_UNORM;

	//case CF_DXT1:
	//	return DXGI_FORMAT_BC1_UNORM;

	//case CF_DXT3:
	//	return DXGI_FORMAT_BC2_UNORM;

	//case CF_DXT5:
	//	return DXGI_FORMAT_BC3_UNORM;

	//default:
	//	return 0;
	//}

	switch (format)
	{
	case CF_RGBA:
		return TEXTURE_FORMAT_RGBA8;

	case CF_DXT1:
		return TEXTURE_FORMAT_BC1;

	case CF_DXT3:
		return TEXTURE_FORMAT_BC2;

	case CF_DXT5:
		return TEXTURE_FORMAT_BC3;

	default:
		return TEXTURE_FORMAT_UNKNOWN;
	}
}

TextureFormat RenderEngine::GetAlphaFormat()
{
	// return DXGI_FORMAT_A8_UNORM;
	return TEXTURE_FORMAT_A8;
}

TextureFormat RenderEngine::GetLuminanceFormat()
{
	// return DXGI_FORMAT_R8_UNORM;
	return TEXTURE_FORMAT_R8;
}

TextureFormat RenderEngine::GetLuminanceAlphaFormat()
{
	// return DXGI_FORMAT_R8G8_UNORM;
	return TEXTURE_FORMAT_RG8;
}

TextureFormat RenderEngine::GetRGBFormat()
{
	// return DXGI_FORMAT_R8G8B8A8_UNORM;
	return TEXTURE_FORMAT_RGB8;
}

TextureFormat RenderEngine::GetRGBAFormat()
{
	// return DXGI_FORMAT_R8G8B8A8_UNORM;
	return TEXTURE_FORMAT_RGBA8;
}

TextureFormat RenderEngine::GetRGBA16Format()
{
	// return DXGI_FORMAT_R16G16B16A16_UNORM;
	return TEXTURE_FORMAT_RGBA16;
}

TextureFormat RenderEngine::GetRGBAFloat16Format()
{
	// return DXGI_FORMAT_R16G16B16A16_FLOAT;
	return TEXTURE_FORMAT_RGBA16F;
}

TextureFormat RenderEngine::GetRGBAFloat32Format()
{
	// return DXGI_FORMAT_R32G32B32A32_FLOAT;
	return TEXTURE_FORMAT_RGBA32F;
}

TextureFormat RenderEngine::GetRG16Format()
{
	// return DXGI_FORMAT_R16G16_UNORM;
	return TEXTURE_FORMAT_RG16;
}

TextureFormat RenderEngine::GetRGFloat16Format()
{
	// return DXGI_FORMAT_R16G16_FLOAT;
	return TEXTURE_FORMAT_RG16F;
}

TextureFormat RenderEngine::GetRGFloat32Format()
{
	// return DXGI_FORMAT_R32G32_FLOAT;
	return TEXTURE_FORMAT_RG32F;
}

TextureFormat RenderEngine::GetFloat16Format()
{
	// return DXGI_FORMAT_R16_FLOAT;
	return TEXTURE_FORMAT_R16F;
}

TextureFormat RenderEngine::GetFloat32Format()
{
	// return DXGI_FORMAT_R32_FLOAT;
	return TEXTURE_FORMAT_R32F;
}

TextureFormat RenderEngine::GetLinearDepthFormat()
{
	// return DXGI_FORMAT_R32_FLOAT;
	return TEXTURE_FORMAT_R32F;
}

TextureFormat RenderEngine::GetDepthStencilFormat()
{
	// return DXGI_FORMAT_R24G8_TYPELESS;
	return TEXTURE_FORMAT_D24S8;
}

TextureFormat RenderEngine::GetReadableDepthFormat()
{
	// return DXGI_FORMAT_R24G8_TYPELESS;
	return TEXTURE_FORMAT_D24S8;
}

TextureFormat RenderEngine::GetFormat(const String& formatName)
{
	String nameLower = formatName.ToLower().Trimmed();

	if (nameLower == "a")
		return GetAlphaFormat();
	if (nameLower == "l")
		return GetLuminanceFormat();
	if (nameLower == "la")
		return GetLuminanceAlphaFormat();
	if (nameLower == "rgb")
		return GetRGBFormat();
	if (nameLower == "rgba")
		return GetRGBAFormat();
	if (nameLower == "rgba16")
		return GetRGBA16Format();
	if (nameLower == "rgba16f")
		return GetRGBAFloat16Format();
	if (nameLower == "rgba32f")
		return GetRGBAFloat32Format();
	if (nameLower == "rg16")
		return GetRG16Format();
	if (nameLower == "rg16f")
		return GetRGFloat16Format();
	if (nameLower == "rg32f")
		return GetRGFloat32Format();
	if (nameLower == "r16f")
		return GetFloat16Format();
	if (nameLower == "r32f" || nameLower == "float")
		return GetFloat32Format();
	if (nameLower == "lineardepth" || nameLower == "depth")
		return GetLinearDepthFormat();
	if (nameLower == "d24s8")
		return GetDepthStencilFormat();
	if (nameLower == "readabledepth" || nameLower == "hwdepth")
		return GetReadableDepthFormat();

	return GetRGBFormat();
}

UInt32 RenderEngine::GetMaxBonesNum()
{
	return 64;
}

ShaderParameters& RenderEngine::GetShaderParameters()
{
	return *shaderParameters_;
}

void RenderEngine::PostRenderBatch(const Vector<SharedPtr<Batch>>& batches)
{
	batches_.Clear();
	for (const auto& batch : batches)
	{
		batches_.Push(batch);
	}
}

void RenderEngine::SetShaderParameter(Camera* camera, const RenderContext* renderContext)
{
	if (!renderContext)
		return;

	// 视图矩阵，投影矩阵，蒙皮矩阵等
	if (camera && renderContext->worldTransform_ && renderContext->numWorldTransform_)
	{
		shaderParameters_->SetValue(SP_WORLD_MATRIX, *renderContext->worldTransform_);
		shaderParameters_->SetValue(SP_VIEW_MATRIX, camera->GetViewMatrix());
		shaderParameters_->SetValue(SP_PROJVIEW_MATRIX, camera->GetProjectionMatrix() * camera->GetViewMatrix());
		shaderParameters_->SetValue(SP_CAMERA_POS, camera->GetNode()->GetWorldPosition());

		if (renderContext->geometryType_ == GEOMETRY_SKINNED)
		{
			skinMatrix_ = renderContext->worldTransform_;
			numSkinMatrix_ = renderContext->numWorldTransform_;
		}
	}

	inShaderParameters_ = renderContext->shaderParameters_;
}

void RenderEngine::SetVertexBuffers(const Vector<SharedPtr<VertexBuffer>>& vertexBuffers)
{
	gfxDevice_->ClearVertexBuffer();

	for (UInt32 i = 0; i < vertexBuffers.Size(); ++i)
	{
		gfxDevice_->AddVertexBuffer(vertexBuffers[i]->GetGfxRef());
	}

	gfxDevice_->SetVertexDescription(vertexBuffers[0]->GetVertexDescription());
}

void RenderEngine::SetIndexBuffer(IndexBuffer* indexBuffer)
{
	if (indexBuffer)
		gfxDevice_->SetIndexBuffer(indexBuffer->GetGfxRef());
}

void RenderEngine::SetVertexShader(Shader* shader)
{
	vertexShader_ = shader;
	vertexShaderDirty_ = true;
}

void RenderEngine::SetPixelShader(Shader* shader)
{
	pixelShader_ = shader;
	pixelShaderDirty_ = true;
}

void RenderEngine::SetTextures(const Vector<SharedPtr<Texture>>& textures)
{
	for (UInt32 i = 0; i < MAX_TEXTURE_CLASS; ++i)
	{
		auto texture = i < textures.Size() && textures[i] ? textures[i] : defaultTextures_[i];
		gfxDevice_->SetTexture(i, texture->GetGfxRef());
	}
}

void RenderEngine::SetDefaultTextures(TextureClass index, Texture* texture)
{
	defaultTextures_[index] = texture;
}

void RenderEngine::SetRasterizerState(RasterizerState rasterizerState)
{
	gfxDevice_->SetBlendMode(rasterizerState.blendMode_);
	gfxDevice_->SetCullMode(rasterizerState.cullMode_);
	gfxDevice_->SetFillMode(rasterizerState.fillMode_);
	gfxDevice_->SetDepthWrite(rasterizerState.depthWrite_);
}

void RenderEngine::SetPrimitiveType(PrimitiveType primitiveType)
{
	gfxDevice_->SetPrimitiveType(primitiveType);
}

void RenderEngine::PreDraw()
{
	if (vertexShaderDirty_ || pixelShaderDirty_)
	{
		gfxDevice_->SetShaders(vertexShader_->GetGfxRef(), pixelShader_->GetGfxRef());

		vertexShaderDirty_ = false;
		pixelShaderDirty_ = false;
	}

	gfxDevice_->SetEngineShaderParameters(shaderParameters_);
	gfxDevice_->SetMaterialShaderParameters(inShaderParameters_);
}

void RenderEngine::DrawCallIndexed(UInt32 indexStart, UInt32 indexCount)
{
	PreDraw();

	// draw call index
	gfxDevice_->DrawIndexed(indexCount, indexStart, 0);
}

void RenderEngine::DrawCall(UInt32 vertexStart, UInt32 vertexCount)
{
	PreDraw();

	// draw call
	gfxDevice_->Draw(vertexCount, vertexStart);
}

void RenderEngine::SetRenderTarget(Viewport* viewport, bool renderShadowMap)
{
	renderShadowMap_ = renderShadowMap;

	gfxDevice_->SetRenderTarget(viewport->GetRenderTarget());
	gfxDevice_->SetDepthStencil(viewport->GetDepthStencil());
}

void RenderEngine::Render(Viewport* viewport)
{
	if (!viewport) return;

	Camera* camera = viewport->GetCamera();
	if (!camera)
		return;

	Scene* scene = viewport->GetScene();
	if (!scene)
		return;
	PODVector<RenderContext*> renderContexts;
	scene->Render(renderContexts);

	viewport->SetViewport();

	float aspect = (float)viewport->GetWidth() / viewport->GetHeight();

	PODVector<Light*> lights;
	scene->GetLights(lights);
			
	SetRenderTarget(viewport, true);

	// 放射相机不处理阴影
	if (!camera->GetUseReflection())
	{
		for (auto light : lights)
		{
			for (const auto& renderContext : renderContexts)
			{
				if ((renderContext->viewMask_ & camera->GetViewMask()) != renderContext->viewMask_)
					continue;
				if (!renderContext->renderPass_)
					continue;
				auto it = renderContext->renderPass_->Find(RENDER_PASS_TYPE_SHADOW);
				if (it != renderContext->renderPass_->End())
				{
					light->SetAspect(aspect);
					SetRasterizerState(shadowRasterizerState_);
					SetShaderParameter(light, renderContext);
					SetVertexShader(it->second_.vertexShader_);
					SetPixelShader(it->second_.pixelShader_);
					for (const auto& geometry : renderContext->geometries_)
					{
						SetVertexBuffers(geometry->GetVertexBuffers());
						SetIndexBuffer(geometry->GetIndexBuffer());
						SetPrimitiveType(geometry->GetPrimitiveType());
						DrawCallIndexed(geometry->GetIndexStart(), geometry->GetIndexCount());
					}
				}
			}
		}
	}
	else
	{
		// 这步操作用来清理阴影贴图
		PreDraw();
	}

	SetRenderTarget(viewport, false);
	for (const auto& renderContext : renderContexts)
	{
		if ((renderContext->viewMask_ & camera->GetViewMask()) != renderContext->viewMask_)
			continue;

		if (lights.Size() > 0
			// 要不要传灯光的参数，与是否加了阴影通道没关系
			/* &&
			renderContext->renderPass_ &&
			renderContext->renderPass_->Contains(RENDER_PASS_TYPE_SHADOW)*/)
		{
			Node* lightNode = lights[0]->GetNode();
			lights[0]->SetAspect(aspect);
			shaderParameters_->SetValue(SP_LIGHT_POS, lightNode->GetWorldPosition());
			shaderParameters_->SetValue(SP_LIGHT_DIR, lightNode->GetWorldRotation() * Vector3::FORWARD);
			shaderParameters_->SetValue(SP_LIGHT_VIEW_MATRIX, lights[0]->GetViewMatrix());
			shaderParameters_->SetValue(SP_LIGHT_PROJVIEW_MATRIX, lights[0]->GetProjectionMatrix() * lights[0]->GetViewMatrix());
		}

		camera->SetAspect(aspect);
		SetRasterizerState(renderContext->rasterizerState_);
		SetShaderParameter(camera, renderContext);
		SetVertexShader(renderContext->vertexShader_);
		SetPixelShader(renderContext->pixelShader_);
		SetTextures(renderContext->textures_);

		for (const auto& geometry : renderContext->geometries_)
		{
			SetVertexBuffers(geometry->GetVertexBuffers());
			SetIndexBuffer(geometry->GetIndexBuffer());
			SetPrimitiveType(geometry->GetPrimitiveType());
			DrawCallIndexed(geometry->GetIndexStart(), geometry->GetIndexCount());
		}
	}
}

void RenderEngine::RenderBatch(Viewport* viewport)
{
	static PODVector<VertexElement> vertexElement =
	{
		VertexElement(VE_VECTOR3, SEM_POSITION, 0),
		VertexElement(VE_VECTOR2, SEM_TEXCOORD, 0),
		VertexElement(VE_UBYTE4, SEM_COLOR, 0)
	};
	static SharedPtr<VertexBuffer> vertexBuffer;
	static Vector<SharedPtr<VertexBuffer>> vertexBuffers;
	static SharedPtr<Shader> vertexShader;
	static SharedPtr<Shader> pixelShader;
	static SharedPtr<ShaderCode> vertexShaderCode;
	static SharedPtr<ShaderCode> pixelShaderCode;
	static Vector<SharedPtr<Texture>> textures;

	if (!viewport)
		return;

	viewport->SetViewport();

	SetRenderTarget(viewport, false);

	if (!vertexBuffer)
	{
		vertexBuffer = new VertexBuffer();
	}

	if (!vertexShaderCode && !pixelShaderCode)
	{
		auto* cache = context_->GetVariable<ResourceCache>("ResourceCache");

		vertexShaderCode = cache->GetResource<ShaderCode>("Shader/UI.hlsl");
		vertexShader = vertexShaderCode->GetShader(VS, {});

		pixelShaderCode = cache->GetResource<ShaderCode>("Shader/UI.hlsl");
		pixelShader = pixelShaderCode->GetShader(PS, {});
	}

	for (const auto& batch : batches_)
	{
		vertexBuffer->SetSize(batch->GetVertexCount(), vertexElement);
		char* data = (char*)vertexBuffer->Lock(0, vertexBuffer->GetVertexCount());
		memcpy(data, batch->GetVertexs(), batch->GetVertexCount() * batch->GetVertexSize());
		vertexBuffer->Unlock();

		vertexBuffers.Clear();
		vertexBuffers.Push(vertexBuffer);

		SetVertexBuffers(vertexBuffers);
		SetVertexShader(vertexShader);
		SetPixelShader(pixelShader);

		textures.Clear();
		if (batch->GetTexture())
		{
			textures.Push(SharedPtr<Texture>(batch->GetTexture()));
			SetTextures(textures);
		}

		switch (batch->GetType())
		{
		case DRAW_LINE:
			SetPrimitiveType(PRIMITIVE_LINE);
			break;

		case DRAW_TRIANGLE:
			SetPrimitiveType(PRIMITIVE_TRIANGLE);
			break;
		}

		DrawCall(0, batch->GetVertexCount());
	}
}

}