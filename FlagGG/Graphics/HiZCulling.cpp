#include "HiZCulling.h"
#include "Resource/ResourceCache.h"
#include "Scene/DrawableComponent.h"
#include "Scene/Camera.h"
#include "Core/Context.h"
#include "Graphics/ShaderHelper.h"
#include "Graphics/ShaderParameter.h"
#include "Graphics/Texture2D.h"
#include "Graphics/RenderEngine.h"
#include "GfxDevice/GfxDevice.h"
#include "GfxDevice/GfxTexture.h"
#include "GfxDevice/GfxSampler.h"
#include "Memory/Memory.h"
#include "Core/Profiler.h"

namespace FlagGG
{

static const UInt32 HiZMapWidth = 256;
static const UInt32 HiZMapHeight = 256;

HiZCulling::HiZCulling()
{
	buildHiZPrams_ = new ShaderParameters();
	buildHiZPrams_->AddParametersDefine<Vector2>("invSize");

	calcVisibilityParams_ = new ShaderParameters();
	calcVisibilityParams_->AddParametersDefine<Matrix4>("projviewMatrix");
	calcVisibilityParams_->AddParametersDefine<Vector2>("uvFactor");
	calcVisibilityParams_->AddParametersDefine<Real>("mipBias");
	calcVisibilityParams_->AddParametersDefine<Vector2>("HiZTextureSize");
}

HiZCulling::~HiZCulling()
{

}

void HiZCulling::InitializeFrame(Camera* camera, bool genClosestHiZ)
{
	if (!inited_ || reverseZ_ != camera->GetReverseZ() || genClosestHiZ_ != genClosestHiZ)
	{
		reverseZ_ = camera->GetReverseZ();
		genClosestHiZ_ = genClosestHiZ;
		inited_ = true;

		const String REVERSE_Z = reverseZ_ ? "REVERSE_Z" : "";
		const String CLOSEST_HIZ = genClosestHiZ_ ? "CLOSEST_HIZ" : "";

		INIT_SHADER_VARIATION(buildHiZVS_, "Shader/HiZ/HiZBuilder.hlsl", VS, {});
		INIT_SHADER_VARIATION(buildHiZPS_, "Shader/HiZ/HiZBuilder.hlsl", PS, Vector<String>({ "COPY_DEPTH", CLOSEST_HIZ }));
		INIT_SHADER_VARIATION(buildHiZMipsPS_, "Shader/HiZ/HiZBuilder.hlsl", PS, Vector<String>({ REVERSE_Z, CLOSEST_HIZ }));

		INIT_SHADER_VARIATION(calcVisibilityVS_, "Shader/HiZ/HiZVisibility.hlsl", VS, {});
		INIT_SHADER_VARIATION(calcVisibilityPS_, "Shader/HiZ/HiZVisibility.hlsl", PS, { REVERSE_Z });
	}

	viewProjectMatrix_ = camera->GetProjectionMatrix() * camera->GetViewMatrix();
}

void HiZCulling::BuildHiZMap(Texture2D* depthTexture)
{
	auto* gfxDevice = GfxDevice::GetDevice();
	auto* renderEngine = GetSubsystem<RenderEngine>();

	IntVector2 HiZMapSize;
	HiZMapSize.x_ = Max(NextPowerOfTwo(depthTexture->GetWidth() >> 1), 1u);
	HiZMapSize.y_ = Max(NextPowerOfTwo(depthTexture->GetHeight() >> 1), 1u);

	if (!HiZMap_)
		HiZMap_ = new Texture2D();

	if (HiZMap_->GetWidth() != HiZMapSize.x_ ||
		HiZMap_->GetHeight() != HiZMapSize.y_)
	{
		HiZMap_->SetNumLevels(0); // max mips
		HiZMap_->SetSubResourceViewEnabled(true);
		HiZMap_->SetFilterMode(TEXTURE_FILTER_NEAREST);
		HiZMap_->SetSize(HiZMapSize.x_, HiZMapSize.y_, TEXTURE_FORMAT_R32F, TEXTURE_RENDERTARGET);
		HiZMap_->SetGpuTag("FurthestHiZ");
	}

	if (genClosestHiZ_)
	{
		if (!closestHiZMap_)
			closestHiZMap_ = new Texture2D();

		if (closestHiZMap_->GetWidth() != HiZMapSize.x_ ||
			closestHiZMap_->GetHeight() != HiZMapSize.y_)
		{
			closestHiZMap_->SetNumLevels(0); // max mips
			closestHiZMap_->SetSubResourceViewEnabled(true);
			closestHiZMap_->SetFilterMode(TEXTURE_FILTER_NEAREST);
			closestHiZMap_->SetSize(HiZMapSize.x_, HiZMapSize.y_, TEXTURE_FORMAT_R32F, TEXTURE_RENDERTARGET);
			closestHiZMap_->SetGpuTag("ClosestHiZ");
		}
	}

	if (!depthSampler_)
	{
		depthSampler_ = gfxDevice->CreateSampler();
		depthSampler_->SetFilterMode(TEXTURE_FILTER_TRILINEAR);
		depthSampler_->SetAddressMode(TEXTURE_COORDINATE_U, TEXTURE_ADDRESS_CLAMP);
		depthSampler_->SetAddressMode(TEXTURE_COORDINATE_V, TEXTURE_ADDRESS_CLAMP);
		depthSampler_->SetAddressMode(TEXTURE_COORDINATE_W, TEXTURE_ADDRESS_CLAMP);
	}

	gfxDevice->SetViewport(IntRect(0, 0, HiZMap_->GetWidth(), HiZMap_->GetHeight()));
	
	gfxDevice->ResetRenderTargets();
	gfxDevice->SetDepthStencil(nullptr);
	gfxDevice->ResetTextures();
	gfxDevice->ResetSamplers();

	gfxDevice->SetRenderTarget(0, HiZMap_->GetRenderSurface(0, 0));
	if (genClosestHiZ_)
		gfxDevice->SetRenderTarget(1, closestHiZMap_->GetRenderSurface(0, 0));
	gfxDevice->SetTexture(0, depthTexture->GetGfxTextureRef());
	gfxDevice->SetSampler(0, depthSampler_);
	gfxDevice->SetShaders(buildHiZVS_->GetGfxRef(), buildHiZPS_->GetGfxRef());
	gfxDevice->SetMaterialShaderParameters(buildHiZPrams_);
	renderEngine->DrawQuad();

	for (UInt32 level = 1; level < HiZMap_->GetNumLevels(); ++level)
	{
		buildHiZPrams_->SetValue<Vector2>("invSize", Vector2(1.0f / (HiZMap_->GetWidth() >> (level - 1)), 1.0f / (HiZMap_->GetHeight() >> (level - 1))));

		gfxDevice->SetViewport(IntRect(0, 0, HiZMap_->GetWidth() >> level, HiZMap_->GetHeight() >> level));
		gfxDevice->SetRenderTarget(0, HiZMap_->GetRenderSurface(0, level));
		gfxDevice->SetTextureView(0, HiZMap_->GetGfxTextureRef()->GetSubResourceView(0, level - 1));
		gfxDevice->SetSampler(0, HiZMap_->GetGfxSamplerRef());
		if (genClosestHiZ_)
		{
			gfxDevice->SetRenderTarget(1, closestHiZMap_->GetRenderSurface(0, level));
			gfxDevice->SetTextureView(1, closestHiZMap_->GetGfxTextureRef()->GetSubResourceView(0, level - 1));
			gfxDevice->SetSampler(1, closestHiZMap_->GetGfxSamplerRef());
		}
		gfxDevice->SetShaders(buildHiZVS_->GetGfxRef(), buildHiZMipsPS_->GetGfxRef());
		renderEngine->DrawQuad();
	}
}

void HiZCulling::ClearGeometries()
{
	geometries_.Clear();
}

void HiZCulling::AddGeometry(DrawableComponent* drawable)
{
	auto* testInfo = drawable->GetHiZVisibilityTestInfo();
	if (!testInfo)
	{
		SharedPtr<HiZVisibilityTestInfo> newTestInfo(new HiZVisibilityTestInfo());
		drawable->SetHiZVisibilityTestInfo(newTestInfo);
		testInfo = newTestInfo;
	}

	testInfo->frameNumber_ = GetSubsystem<Context>()->GetFrameNumber();
	// 保守估计，计算遮挡后更新此值
	testInfo->visible_ = true;

	geometries_.Push(SharedPtr<DrawableComponent>(drawable));
}

void HiZCulling::AllocTexture()
{
	if (!HiZResults_)
	{
		HiZAABBMinPos_ = new Texture2D();
		HiZAABBMinPos_->SetNumLevels(1);
		HiZAABBMinPos_->SetAddressMode(TEXTURE_COORDINATE_U, TEXTURE_ADDRESS_CLAMP);
		HiZAABBMinPos_->SetAddressMode(TEXTURE_COORDINATE_V, TEXTURE_ADDRESS_CLAMP);
		HiZAABBMinPos_->SetAddressMode(TEXTURE_COORDINATE_W, TEXTURE_ADDRESS_CLAMP);
		HiZAABBMinPos_->SetFilterMode(TEXTURE_FILTER_NEAREST);
		HiZAABBMinPos_->SetSize(HiZMapWidth, HiZMapHeight, TEXTURE_FORMAT_RGBA32F);

		HiZAABBMaxPos_ = new Texture2D();
		HiZAABBMaxPos_->SetNumLevels(1);
		HiZAABBMaxPos_->SetAddressMode(TEXTURE_COORDINATE_U, TEXTURE_ADDRESS_CLAMP);
		HiZAABBMaxPos_->SetAddressMode(TEXTURE_COORDINATE_V, TEXTURE_ADDRESS_CLAMP);
		HiZAABBMaxPos_->SetAddressMode(TEXTURE_COORDINATE_W, TEXTURE_ADDRESS_CLAMP);
		HiZAABBMaxPos_->SetFilterMode(TEXTURE_FILTER_NEAREST);
		HiZAABBMaxPos_->SetSize(HiZMapWidth, HiZMapHeight, TEXTURE_FORMAT_RGBA32F);

		HiZResults_ = new Texture2D();
		HiZResults_->SetNumLevels(1);
		HiZResults_->SetSize(HiZMapWidth, HiZMapHeight, TEXTURE_FORMAT_R8, TEXTURE_RENDERTARGET);

		MiZResultsBuffer_.Resize(HiZMapWidth * HiZMapHeight);
	}
}

void HiZCulling::EncodeGeometriesAABB()
{
	// 这里把物体世界空间的AABB编码到纹理，然后在GPU计算视图投影转换。因为物体极多时，CPU矩阵计算会带来极大的常数，对性能造成很大的影响！
	// 这样GPU计算ndc空间AABB时会引入for循环，但计算效率还是比CPU快的
	// 使用RGBA32F格式纹理，现在手机都普及了ES3.0了，因此不需要用RGBA格式去编码了

	UInt32 bufferSize = HiZMapWidth * frameHiZHeight_;

	MiZAABBBuffer1_.Resize(bufferSize * sizeof(Vector4));
	MiZAABBBuffer2_.Resize(bufferSize * sizeof(Vector4));

	for (UInt32 i = 0; i < geometries_.Size(); ++i)
	{
		const BoundingBox& AABB = geometries_[i]->GetWorldBoundingBox();
		MiZAABBBuffer1_[i] = Vector4(AABB.min_, 0.0f);
		MiZAABBBuffer2_[i] = Vector4(AABB.max_, 0.0f);
	}

	HiZAABBMinPos_->SetData(0, 0, 0, HiZMapWidth, frameHiZHeight_, MiZAABBBuffer1_.Buffer());
	HiZAABBMaxPos_->SetData(0, 0, 0, HiZMapWidth, frameHiZHeight_, MiZAABBBuffer2_.Buffer());
}

void HiZCulling::CalcGeometriesVisibility()
{
	auto* gfxDevice = GfxDevice::GetDevice();
	auto* renderEngine = GetSubsystem<RenderEngine>();

	frameHiZHeight_ = (geometries_.Size() + HiZMapWidth - 1) / HiZMapWidth;

	AllocTexture();

	EncodeGeometriesAABB();

	calcVisibilityParams_->SetValue<Matrix4>("projviewMatrix", viewProjectMatrix_);
	calcVisibilityParams_->SetValue<Vector2>("uvFactor", Vector2(1.0f, 1.0f));
	calcVisibilityParams_->SetValue<Real>("mipBias", 0.0f);
	calcVisibilityParams_->SetValue<Vector2>("HiZTextureSize", Vector2(HiZMap_->GetWidth(), HiZMap_->GetHeight()));

	gfxDevice->SetViewport(IntRect(0, 0, HiZMapWidth, HiZMapHeight));
	gfxDevice->SetScissorTest(true, IntRect(0, 0, HiZMapWidth, frameHiZHeight_));

	gfxDevice->ResetRenderTargets();
	gfxDevice->SetDepthStencil(nullptr);
	gfxDevice->ResetTextures();
	gfxDevice->ResetSamplers();

	gfxDevice->SetRenderTarget(0, HiZResults_->GetRenderSurface());

	gfxDevice->SetTexture(0, HiZMap_->GetGfxTextureRef());
	gfxDevice->SetTexture(1, HiZAABBMinPos_->GetGfxTextureRef());
	gfxDevice->SetTexture(2, HiZAABBMaxPos_->GetGfxTextureRef());

	gfxDevice->SetSampler(0, HiZMap_->GetGfxSamplerRef());
	gfxDevice->SetSampler(1, HiZAABBMinPos_->GetGfxSamplerRef());
	gfxDevice->SetSampler(2, HiZAABBMaxPos_->GetGfxSamplerRef());

	gfxDevice->SetShaders(calcVisibilityVS_->GetGfxRef(), calcVisibilityPS_->GetGfxRef());
	gfxDevice->SetMaterialShaderParameters(calcVisibilityParams_);
	renderEngine->DrawQuad();

	gfxDevice->SetScissorTest(false);
}

void HiZCulling::FetchGeometriesVisibilityResults()
{
	PROFILE_AUTO(HiZCulling::FetchGeometriesVisibilityResults);

	auto* gfxTexture = HiZResults_->GetGfxTextureRef();
#if _DEBUG
	Memory::Memzero(&MiZResultsBuffer_[0], HiZMapWidth * frameHiZHeight_);
#endif
	gfxTexture->ReadBackSubRegion(&MiZResultsBuffer_[0], 0, 0, 0, 0, HiZMapWidth, frameHiZHeight_);

	for (UInt32 i = 0; i < geometries_.Size(); ++i)
	{
		geometries_[i]->GetHiZVisibilityTestInfo()->visible_ = MiZResultsBuffer_[i];
	}
}

bool HiZCulling::IsGeometryVisible(DrawableComponent* drawable)
{
	auto* testInfo = drawable->GetHiZVisibilityTestInfo();

	// 表示没有经过测试，保守认为它是可见的
	if (testInfo == nullptr || testInfo->frameNumber_ + 1 < GetSubsystem<Context>()->GetFrameNumber())
		return true;

	return testInfo->visible_;
}

}
