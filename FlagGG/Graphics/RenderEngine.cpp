#include "Graphics/RenderEngine.h"
#include "Graphics/Texture.h"
#include "Graphics/Texture2D.h"
#include "Graphics/RenderView.h"
#include "Graphics/RenderBatch.h"
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

RenderEngine::RenderEngine()
{
	shaderParameters_ = new ShaderParameters();
	shaderParameters_->AddParametersDefine<Matrix3x4>(SP_WORLD_MATRIX);
	shaderParameters_->AddParametersDefine<Matrix3x4>(SP_VIEW_MATRIX);
	shaderParameters_->AddParametersDefine<Matrix4>(SP_PROJ_MATRIX);
	shaderParameters_->AddParametersDefine<Matrix4>(SP_PROJVIEW_MATRIX);
	shaderParameters_->AddParametersDefine<Matrix3x4>(SP_INV_VIEW_MATRIX);
	shaderParameters_->AddParametersDefine<float>(SP_NEAR_CLIP);
	shaderParameters_->AddParametersDefine<float>(SP_FAR_CLIP);
	shaderParameters_->AddParametersDefine<Vector3>(SP_FRUSTUM_SIZE);
	shaderParameters_->AddParametersDefine<Vector4>(SP_DEPTH_RECONSTRUCT);
	shaderParameters_->AddParametersDefine<Vector4>(SP_DEVICEZ_TO_WORLDZ);
	shaderParameters_->AddParametersDefine<Vector4>(SP_SCREEN_POSITION_SCALE_BIAS);
	shaderParameters_->AddParametersDefine<float>(SP_DELTA_TIME);
	shaderParameters_->AddParametersDefine<float>(SP_ELAPSED_TIME);
	shaderParameters_->AddParametersDefine<UInt32>(SP_FRAME_NUMBER);
	shaderParameters_->AddParametersDefine<UInt32>(SP_FRAME_NUMBER_MOD8);
	shaderParameters_->AddParametersDefine<Vector3>(SP_CAMERA_POS);
	shaderParameters_->AddParametersDefine<Vector3>(SP_LIGHT_POS);
	shaderParameters_->AddParametersDefine<Vector3>(SP_LIGHT_DIR);
	shaderParameters_->AddParametersDefine<Color>(SP_LIGHT_COLOR);
	shaderParameters_->AddParametersDefine<Matrix3x4>(SP_LIGHT_VIEW_MATRIX);
	shaderParameters_->AddParametersDefine<Matrix4>(SP_LIGHT_PROJVIEW_MATRIX);
	shaderParameters_->AddParametersDefine<Vector2>(SP_SHADOWMAP_PIXEL_TEXELS);
	shaderParameters_->AddParametersDefine<Vector4>(SP_ENV_CUBE_ANGLE);
	shaderParameters_->AddParametersDefine<float>(SP_SH_INTENSITY);
	shaderParameters_->AddParametersDefine<float>(SP_IBL_INTENSITY);
	shaderParameters_->AddParametersDefine<float>(SP_AMBIENT_OCCLUSION_INTENSITY);
	shaderParameters_->AddParametersDefine<Vector4>(SP_SHAR);
	shaderParameters_->AddParametersDefine<Vector4>(SP_SHAG);
	shaderParameters_->AddParametersDefine<Vector4>(SP_SHAB);
	shaderParameters_->AddParametersDefine<Vector4>(SP_SHBR);
	shaderParameters_->AddParametersDefine<Vector4>(SP_SHBG);
	shaderParameters_->AddParametersDefine<Vector4>(SP_SHBB);
	shaderParameters_->AddParametersDefine<Vector4>(SP_SHC);
	shaderParameters_->AddParametersDefineImpl(SP_SKIN_MATRICES, sizeof(Matrix3x4) * GetMaxBonesNum());
}

void RenderEngine::Initialize()
{
	gfxDevice_ = GfxDevice::CreateDevice();

	const Real vertexData[] =
	{
		-1, 1, 0,
		1, 1, 0,
		1, -1, 0,
		-1, -1, 0,
	};
	SharedPtr<VertexBuffer> vb(new VertexBuffer());
	PODVector<VertexElement> elements;
	elements.Push(VertexElement(VE_VECTOR3, SEM_POSITION));
	vb->SetSize(4, elements);
	vb->SetData(vertexData);

	const UInt16 indexData[] =
	{
		0, 1, 2,
		2, 3, 0,
	};
	SharedPtr<IndexBuffer> ib(new IndexBuffer());
	ib->SetSize(sizeof(UInt16), 6u);
	ib->SetData(indexData);

	orthographicGeometry_ = new Geometry();
	orthographicGeometry_->SetVertexBuffer(0u, vb);
	orthographicGeometry_->SetIndexBuffer(ib);
	orthographicGeometry_->SetDataRange(0, ib->GetIndexCount());
	orthographicGeometry_->SetPrimitiveType(PRIMITIVE_TRIANGLE);

	fullscreenQuadRS_.scissorTest_ = false;
	fullscreenQuadRS_.fillMode_ = FILL_SOLID;
	fullscreenQuadRS_.cullMode_ = CULL_NONE;
	fullscreenQuadRS_.blendMode_ = BLEND_REPLACE;

	fullscreenDSS_.depthWrite_ = false;
	fullscreenDSS_.stencilTest_ = false;
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

	case CF_RGBA16F:
		return TEXTURE_FORMAT_RGBA16F;

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
	return TEXTURE_FORMAT_RGBA8;
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
	return TEXTURE_FORMAT_D24;
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
	return 150;
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

void RenderEngine::SetShaderParameter(Camera* camera, const RenderBatch& renderBatch, bool disableReverseZ)
{
	// 视图矩阵，投影矩阵，蒙皮矩阵等
	if (camera && renderBatch.worldTransform_ && renderBatch.numWorldTransform_)
	{
		bool reverseZ = camera->GetReverseZ();
		if (disableReverseZ && reverseZ)
			camera->SetReverseZ(false);
		shaderParameters_->SetValue(SP_WORLD_MATRIX, *renderBatch.worldTransform_);
		shaderParameters_->SetValue(SP_VIEW_MATRIX, camera->GetViewMatrix());
		shaderParameters_->SetValue(SP_PROJ_MATRIX, camera->GetProjectionMatrix());
		shaderParameters_->SetValue(SP_PROJVIEW_MATRIX, camera->GetProjectionMatrix() * camera->GetViewMatrix());
		shaderParameters_->SetValue(SP_INV_VIEW_MATRIX, camera->GetViewMatrix().Inverse());
		shaderParameters_->SetValue(SP_NEAR_CLIP, camera->GetNearClip());
		shaderParameters_->SetValue(SP_FAR_CLIP, camera->GetFarClip());
		Vector3 nearVector, farVector;
		camera->GetFrustumSize(nearVector, farVector);
		shaderParameters_->SetValue(SP_FRUSTUM_SIZE, farVector);
		if (disableReverseZ && reverseZ)
			camera->SetReverseZ(true);
		Vector4 depthReconstruct(camera->GetFarClip() / (camera->GetFarClip() - camera->GetNearClip()), -camera->GetNearClip() / (camera->GetFarClip() - camera->GetNearClip()), camera->IsOrthographic() ? 1.0f : 0.0f, camera->IsOrthographic() ? 0.0f : 1.0f);
		shaderParameters_->SetValue(SP_DEPTH_RECONSTRUCT, depthReconstruct);
		shaderParameters_->SetValue(SP_DEVICEZ_TO_WORLDZ, CreateDeviceZToWorldZTransform(camera->GetProjectionMatrix()));
		shaderParameters_->SetValue(SP_SCREEN_POSITION_SCALE_BIAS, Vector4(1, 1, 0, 0));
		shaderParameters_->SetValue(SP_CAMERA_POS, camera->GetNode()->GetWorldPosition());

		if (renderBatch.geometryType_ == GEOMETRY_SKINNED)
		{
			shaderParameters_->SetValueImpl(SP_SKIN_MATRICES, renderBatch.worldTransform_, renderBatch.numWorldTransform_ * sizeof(Matrix3x4));
		}
	}

	gfxDevice_->SetEngineShaderParameters(shaderParameters_);
	if (renderBatch.material_)
		gfxDevice_->SetMaterialShaderParameters(renderBatch.material_->GetShaderParameters());
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

void RenderEngine::SetShaders(Shader* vertexShader, Shader* pixelShader)
{
	if (vertexShader && pixelShader)
	{
		gfxDevice_->SetShaders(vertexShader->GetGfxRef(), pixelShader->GetGfxRef());
	}
}

void RenderEngine::SetTextures(const Vector<SharedPtr<Texture>>& textures)
{
	for (UInt32 i = 0; i < MAX_TEXTURE_CLASS; ++i)
	{
		auto texture = i < textures.Size() && textures[i] ? textures[i] : defaultTextures_[i];
		if (texture)
		{
			gfxDevice_->SetTexture(i, texture->GetGfxTextureRef());
			gfxDevice_->SetSampler(i, texture->GetGfxSamplerRef());
		}
	}
}

void RenderEngine::SetMaterialTextures(Material* material)
{
	if (!material)
		return;

	for (UInt32 i = 0; i < MAX_TEXTURE_CLASS; ++i)
	{
		auto texture = material->GetTexture(i) ? material->GetTexture(i) : defaultTextures_[i];
		if (texture)
		{
			gfxDevice_->SetTexture(i, texture->GetGfxTextureRef());
			gfxDevice_->SetSampler(i, texture->GetGfxSamplerRef());
		}
	}
}

void RenderEngine::SetDefaultTexture(TextureClass index, Texture* texture)
{
	defaultTextures_[index] = texture;
}

void RenderEngine::SetRasterizerState(const RasterizerState& rasterizerState)
{
	gfxDevice_->SetBlendMode(rasterizerState.blendMode_);
	gfxDevice_->SetCullMode(rasterizerState.cullMode_);
	gfxDevice_->SetFillMode(rasterizerState.fillMode_);
	gfxDevice_->SetDepthBias(0.0f, 0.0f);
}

void RenderEngine::SetDepthStencilState(const DepthStencilState& depthStencilState, bool reverseZ)
{
	gfxDevice_->SetDepthWrite(depthStencilState.depthWrite_);
	gfxDevice_->SetDepthTestMode(reverseZ ? REVERSE_Z_MAPPING[depthStencilState.depthTestMode_] : depthStencilState.depthTestMode_);
	gfxDevice_->SetStencilTest(
		depthStencilState.stencilTest_,
		depthStencilState.stencilTestMode_,
		depthStencilState.stencilRef_,
		depthStencilState.stencilReadMask_,
		depthStencilState.stencilWriteMask_);
}

void RenderEngine::SetPrimitiveType(PrimitiveType primitiveType)
{
	gfxDevice_->SetPrimitiveType(primitiveType);
}

void RenderEngine::DrawCallIndexed(UInt32 indexStart, UInt32 indexCount)
{
	gfxDevice_->DrawIndexed(indexStart, indexCount, 0);
}

void RenderEngine::DrawCall(UInt32 vertexStart, UInt32 vertexCount)
{
	gfxDevice_->Draw(vertexStart, vertexCount);
}

void RenderEngine::DrawBatch(Camera* camera, const RenderBatch& renderBatch)
{
	SetRasterizerState(renderBatch.renderPassInfo_->GetRasterizerState());
	SetDepthStencilState(renderBatch.renderPassInfo_->GetDepthStencilState(), camera->GetReverseZ());
	SetShaderParameter(camera, renderBatch);
	if (renderBatch.vertexShader_ && renderBatch.pixelShader_)
		SetShaders(renderBatch.vertexShader_, renderBatch.pixelShader_);
	else
		SetShaders(renderBatch.material_->GetVertexShader(), renderBatch.material_->GetPixelShader());
	SetMaterialTextures(renderBatch.material_);
	SetVertexBuffers(renderBatch.geometry_->GetVertexBuffers());
	if (renderBatch.vertexDesc_)
		gfxDevice_->SetVertexDescription(renderBatch.vertexDesc_);
	SetIndexBuffer(renderBatch.geometry_->GetIndexBuffer());
	SetPrimitiveType(renderBatch.geometry_->GetPrimitiveType());
	DrawCallIndexed(renderBatch.geometry_->GetIndexStart(), renderBatch.geometry_->GetIndexCount());
}

void RenderEngine::DrawBatch(Camera* camera, const RenderInstanceBatch& renderBatch, VertexBuffer* instanceBuffer)
{
	SetRasterizerState(renderBatch.renderPassInfo_->GetRasterizerState());
	SetDepthStencilState(renderBatch.renderPassInfo_->GetDepthStencilState(), camera->GetReverseZ());
	SetShaderParameter(camera, renderBatch);
	if (renderBatch.vertexShader_ && renderBatch.pixelShader_)
		SetShaders(renderBatch.vertexShader_, renderBatch.pixelShader_);
	else
		SetShaders(renderBatch.material_->GetVertexShader(), renderBatch.material_->GetPixelShader());
	SetMaterialTextures(renderBatch.material_);
	SetVertexBuffers(renderBatch.geometry_->GetVertexBuffers());
	if (renderBatch.vertexDesc_)
		gfxDevice_->SetVertexDescription(renderBatch.vertexDesc_);
	SetIndexBuffer(renderBatch.geometry_->GetIndexBuffer());
	SetPrimitiveType(renderBatch.geometry_->GetPrimitiveType());
	gfxDevice_->SetInstanceBuffer(instanceBuffer->GetGfxRef());
	gfxDevice_->SetInstanceDescription(instanceBuffer->GetVertexDescription());
	gfxDevice_->DrawIndexedInstanced(renderBatch.geometry_->GetIndexStart(), renderBatch.geometry_->GetIndexCount(), 0, renderBatch.instanceStart_, renderBatch.instanceCount_);
}

Matrix3x4 RenderEngine::GetFullscreenQuadTransform(Camera* camera)
{
	Matrix3x4 quadTransform;
	Vector3 nearVec, farVec;
	// Position the directional light quad in halfway between far & near planes to prevent depth clipping
	camera->GetFrustumSize(nearVec, farVec);
	quadTransform.SetTranslation(Vector3(0.0f, 0.0f, (camera->GetNearClip() + camera->GetFarClip()) * 0.5f));
	quadTransform.SetScale(Vector3(farVec.x_, farVec.y_, 1.0f)); // Will be oversized, but doesn't matter (gets frustum clipped)
	return camera->GetEffectiveWorldTransform() * quadTransform;
}

void RenderEngine::DrawQuad(Camera* camera, bool disableReverseZ)
{
	Matrix3x4 worldTransform = GetFullscreenQuadTransform(camera);

	RenderBatch quadRenderBatch;
	quadRenderBatch.geometryType_ = GEOMETRY_STATIC;
	quadRenderBatch.worldTransform_ = &worldTransform;
	quadRenderBatch.numWorldTransform_ = 1u;

	SetRasterizerState(fullscreenQuadRS_);
	SetDepthStencilState(fullscreenDSS_, camera->GetReverseZ());
	SetShaderParameter(camera, quadRenderBatch, disableReverseZ);
	SetVertexBuffers(orthographicGeometry_->GetVertexBuffers());
	SetIndexBuffer(orthographicGeometry_->GetIndexBuffer());
	SetPrimitiveType(orthographicGeometry_->GetPrimitiveType());
	DrawCallIndexed(orthographicGeometry_->GetIndexStart(), orthographicGeometry_->GetIndexCount());
}

void RenderEngine::DrawQuad()
{
	SetRasterizerState(fullscreenQuadRS_);
	SetDepthStencilState(fullscreenDSS_, false);
	SetVertexBuffers(orthographicGeometry_->GetVertexBuffers());
	SetIndexBuffer(orthographicGeometry_->GetIndexBuffer());
	SetPrimitiveType(orthographicGeometry_->GetPrimitiveType());
	DrawCallIndexed(orthographicGeometry_->GetIndexStart(), orthographicGeometry_->GetIndexCount());
}

void RenderEngine::RenderUpdate(Viewport* viewport)
{
	RenderView* renderView = viewport->GetOrCreateRenderView();
	ASSERT(renderView);

	renderView->Define(viewport);

	renderView->RenderUpdate();
}

void RenderEngine::Render(Viewport* viewport)
{
	RenderView* renderView = viewport->GetOrCreateRenderView();
	ASSERT(renderView);

	renderView->Render();
}

}