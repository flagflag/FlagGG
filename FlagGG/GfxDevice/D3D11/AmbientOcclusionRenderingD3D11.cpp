#include "AmbientOcclusionRenderingD3D11.h"
#include "GfxDeviceD3D11.h"
#include "GfxTextureD3D11.h"
#include "GfxRenderSurfaceD3D11.h"
#include "Graphics/Texture2D.h"
#include "Scene/Camera.h"

namespace FlagGG
{

AmbientOcclusionRenderingD3D11::AmbientOcclusionRenderingD3D11()
{
#if GFSDK_SSAO
	aoParameters_.Radius = 2.f;
	aoParameters_.Bias = 0.2f;
	aoParameters_.PowerExponent = 2.f;
	aoParameters_.Blur.Enable = true;
	aoParameters_.Blur.Sharpness = 32.f;
	aoParameters_.Blur.Radius = GFSDK_SSAO_BLUR_RADIUS_4;
	aoParameters_.DepthStorage = GFSDK_SSAO_FP32_VIEW_DEPTHS;
	aoParameters_.EnableDualLayerAO = /*true*/false;

	GFSDK_SSAO_CustomHeap customHeap;
	customHeap.new_ = ::operator new;
	customHeap.delete_ = ::operator delete;

	GFSDK_SSAO_Status status = GFSDK_SSAO_CreateContext_D3D11(GetSubsystem<GfxDeviceD3D11>()->GetD3D11Device(), &ssaoContext_, &customHeap);
	ASSERT(status == GFSDK_SSAO_OK);
#endif
}

AmbientOcclusionRenderingD3D11::~AmbientOcclusionRenderingD3D11()
{

}

void AmbientOcclusionRenderingD3D11::AllocAOTexture(const IntVector2& renderSolution)
{
	if (!aoTexture_)
	{
		aoTexture_ = new Texture2D();
		aoTexture_->SetNumLevels(1);
	}

	if (aoTexture_->GetWidth() != renderSolution.x_ ||
		aoTexture_->GetHeight() != renderSolution.y_)
	{
		aoTexture_->SetSize(renderSolution.x_, renderSolution.y_, TEXTURE_FORMAT_R8, TEXTURE_RENDERTARGET);
	}
}

void AmbientOcclusionRenderingD3D11::RenderAO(const AmbientOcclusionInputData& inputData)
{
#if GFSDK_SSAO
	AllocAOTexture(inputData.renderSolution_);

	auto* depthTextureD3D11 = RTTICast<GfxTextureD3D11>(inputData.screenDepthTexture_->GetGfxTextureRef());
	auto* normalTextureD3D11 = RTTICast<GfxTextureD3D11>(inputData.screenNormalTexture_->GetGfxTextureRef());
	auto* aoSurfaceD3D11 = RTTICast<GfxRenderSurfaceD3D11>(aoTexture_->GetRenderSurface());

	Matrix4 projMatrix = inputData.camera_->GetProjectionMatrix();
	Matrix4 viewMatrix = inputData.camera_->GetViewMatrix().ToMatrix4();

	GFSDK_SSAO_InputData_D3D11 inputDataD3D11 = {};
	inputDataD3D11.DepthData.DepthTextureType = GFSDK_SSAO_HARDWARE_DEPTHS;
	inputDataD3D11.DepthData.pFullResDepthTextureSRV = depthTextureD3D11->GetD3D11ShaderResourceView();
	inputDataD3D11.DepthData.pFullResDepthTexture2ndLayerSRV = nullptr;
	inputDataD3D11.DepthData.ProjectionMatrix.Data = GFSDK_SSAO_Float4x4((const GFSDK_SSAO_FLOAT*)projMatrix.Data());
	inputDataD3D11.DepthData.ProjectionMatrix.Layout = GFSDK_SSAO_COLUMN_MAJOR_ORDER;
	inputDataD3D11.DepthData.MetersToViewSpaceUnits = 1.0f;

	if (normalTextureD3D11)
	{
		inputDataD3D11.NormalData.Enable = true;
		inputDataD3D11.NormalData.WorldToViewMatrix.Data = GFSDK_SSAO_Float4x4((const GFSDK_SSAO_FLOAT*)viewMatrix.Data());
		inputDataD3D11.NormalData.WorldToViewMatrix.Layout = GFSDK_SSAO_COLUMN_MAJOR_ORDER;
		inputDataD3D11.NormalData.DecodeScale = 1.0f;
		inputDataD3D11.NormalData.DecodeBias = 0.0f;
		inputDataD3D11.NormalData.pFullResNormalTextureSRV = normalTextureD3D11->GetD3D11ShaderResourceView();
	}

	GFSDK_SSAO_Output_D3D11 outputD3D11;
	outputD3D11.pRenderTargetView = aoSurfaceD3D11->GetRenderTargetView();

	GFSDK_SSAO_Status status = ssaoContext_->RenderAO(GetSubsystem<GfxDeviceD3D11>()->GetD3D11DeviceContext(), inputDataD3D11, aoParameters_, outputD3D11, GFSDK_SSAO_RENDER_AO);
	ASSERT(status == GFSDK_SSAO_OK);
#endif
}

Texture2D* AmbientOcclusionRenderingD3D11::GetAmbientOcclusionTexture() const
{
	return aoTexture_;
}

}
