#include "ComputePass.h"
#include "GfxDevice/GfxDevice.h"
#include "GfxDevice/GfxBuffer.h"
#include "Graphics/Shader.h"
#include "Graphics/RenderPipline.h"
#include "Scene/Camera.h"
#include "Resource/ResourceCache.h"

namespace FlagGG
{

/*********************************************************/
/*                      ComputePass                      */
/*********************************************************/

ComputePass::ComputePass()
{
}

ComputePass::~ComputePass()
{

}

/*********************************************************/
/*                    ClusterLightPass                   */
/*********************************************************/

static constexpr UInt32 CLUSTERS_X = 16;
static constexpr UInt32 CLUSTERS_Y = 8;
static constexpr UInt32 CLUSTERS_Z = 24;

// limit number of threads (D3D only allows up to 1024, there might also be shared memory limitations)
// shader will be run by 6 work groups
static constexpr UInt32 CLUSTERS_X_THREADS = 16;
static constexpr UInt32 CLUSTERS_Y_THREADS = 8;
static constexpr UInt32 CLUSTERS_Z_THREADS = 1;

#define SAMPLER_LIGHTS_POINTLIGHTS 11
#define SAMPLER_CLUSTERS_CLUSTERS 12
#define SAMPLER_CLUSTERS_ATOMICINDEX 13
#define SAMPLER_CLUSTERS_LIGHTINDICES 14
#define SAMPLER_CLUSTERS_LIGHTGRID 15

ClusterLightPass::ClusterLightPass()
{
	auto* gfxDevice = GfxDevice::GetDevice();
	clusterBuffer_ = gfxDevice->CreateBuffer();
	atomicIndexBuffer_ = gfxDevice->CreateBuffer();
	lightGridBuffer_ = gfxDevice->CreateBuffer();
	lightIndicesBuffer_ = gfxDevice->CreateBuffer();
}

ClusterLightPass::~ClusterLightPass()
{

}

void ClusterLightPass::Clear()
{

}

void ClusterLightPass::InitShader()
{
	auto* cache = GetSubsystem<ResourceCache>();
	const Vector<String> defines =
	{
		ToString("SAMPLER_LIGHTS_POINTLIGHTS=%d ", SAMPLER_LIGHTS_POINTLIGHTS),
		ToString("SAMPLER_CLUSTERS_CLUSTERS=%d ", SAMPLER_CLUSTERS_CLUSTERS),
		ToString("SAMPLER_CLUSTERS_ATOMICINDEX=%d ", SAMPLER_CLUSTERS_ATOMICINDEX),
		ToString("SAMPLER_CLUSTERS_LIGHTINDICES=%d ", SAMPLER_CLUSTERS_LIGHTINDICES),
		ToString("SAMPLER_CLUSTERS_LIGHTGRID=%d ", SAMPLER_CLUSTERS_LIGHTGRID),
		"WRITE_CLUSTERS",
		"PLATFORM_WINDOWS_DIRECTX",
	};
	{
		auto* shaderCode = cache->GetResource<ShaderCode>("Shader/Cluster/ClusteredClusterBuilding.hlsl");
		auto* shader = shaderCode->GetShader(CS, defines);
		clusterBuildingShader_ = shader->GetGfxRef();
	}
	{
		auto* shaderCode = cache->GetResource<ShaderCode>("Shader/Cluster/ClustedResetCounter.hlsl");
		auto* shader = shaderCode->GetShader(CS, defines);
		resetCounterShader_ = shader->GetGfxRef();
	}
	{
		auto* shaderCode = cache->GetResource<ShaderCode>("Shader/Cluster/ClusteredLightCulling.hlsl");
		auto* shader = shaderCode->GetShader(CS, defines);
		lightCullingShader_ = shader->GetGfxRef();
	}
}

void ClusterLightPass::InitShaderParameters(RenderPiplineContext& context)
{
	static const StringHash CLUSTER_SIZES("clusterSizes");
	static const StringHash CLUSTER_ORIGIN("clusterOrigin");
	static const StringHash CLUSTER_NEAR_CLIP("nearClip");
	static const StringHash CLUSTER_FAR_CLIP("farClip");
	static const StringHash VIEW_MATERIX("viewMatrix");
	static const StringHash INV_PROJ("invProjMatrix");
	static const StringHash POINT_LIGHT_COUNT("pointLightCount");
	static const StringHash SPOT_LIGHT_COUNT("spotLightCount");
	static const StringHash NON_PUNCTUAL_POINT_LIGHT_COUNT("nonPunctualPointLightCount");
	static const StringHash POINT_LIGHT_OFFSET("pointLightOffset");
	static const StringHash SPOT_LIGHT_OFFSET("spotLightOffset");
	static const StringHash NON_PUNCTUAL_POINT_LIGHT_OFFSET("nonPunctualPointLightOffset");

	if (!shaderParameters_)
	{
		shaderParameters_ = new ShaderParameters();
		shaderParameters_->AddParametersDefine<Vector2>(CLUSTER_SIZES);
		shaderParameters_->AddParametersDefine<Vector2>(CLUSTER_ORIGIN);
		shaderParameters_->AddParametersDefine<float>(CLUSTER_NEAR_CLIP);
		shaderParameters_->AddParametersDefine<float>(CLUSTER_FAR_CLIP);
		shaderParameters_->AddParametersDefine<Matrix4>(VIEW_MATERIX);
		shaderParameters_->AddParametersDefine<Matrix4>(INV_PROJ);
		shaderParameters_->AddParametersDefine<UInt32>(POINT_LIGHT_COUNT);
		shaderParameters_->AddParametersDefine<UInt32>(SPOT_LIGHT_COUNT);
		shaderParameters_->AddParametersDefine<UInt32>(NON_PUNCTUAL_POINT_LIGHT_COUNT);
		shaderParameters_->AddParametersDefine<UInt32>(POINT_LIGHT_OFFSET);
		shaderParameters_->AddParametersDefine<UInt32>(SPOT_LIGHT_OFFSET);
		shaderParameters_->AddParametersDefine<UInt32>(NON_PUNCTUAL_POINT_LIGHT_OFFSET);
	}

	shaderParameters_->SetValue(CLUSTER_SIZES, Vector2(0.0f, 0.0f));
	shaderParameters_->SetValue(CLUSTER_ORIGIN, Vector2(context.renderSolution_));
	shaderParameters_->SetValue(CLUSTER_NEAR_CLIP, context.camera_->GetNearClip());
	shaderParameters_->SetValue(CLUSTER_FAR_CLIP, context.camera_->GetFarClip());
	shaderParameters_->SetValue(VIEW_MATERIX, context.camera_->GetViewMatrix().ToMatrix4());
	shaderParameters_->SetValue(INV_PROJ, context.camera_->GetProjectionMatrix().Inverse());
	shaderParameters_->SetValue(POINT_LIGHT_COUNT, 0u);
	shaderParameters_->SetValue(SPOT_LIGHT_COUNT, 0u);
	shaderParameters_->SetValue(NON_PUNCTUAL_POINT_LIGHT_COUNT, 0u);
	shaderParameters_->SetValue(POINT_LIGHT_OFFSET, 0u);
	shaderParameters_->SetValue(SPOT_LIGHT_OFFSET, 0u);
	shaderParameters_->SetValue(NON_PUNCTUAL_POINT_LIGHT_OFFSET, 0u);
}

void ClusterLightPass::Dispatch(RenderPiplineContext& context)
{
	if (!clusterBuildingShader_)
	{
		InitShader();
	}

	InitShaderParameters(context);

	auto* gfxDevice = GfxDevice::GetDevice();
	gfxDevice->ResetRenderTargets();
	gfxDevice->SetEngineShaderParameters(shaderParameters_);

// Cluster building
	gfxDevice->SetComputeShader(clusterBuildingShader_);
	gfxDevice->SetComputeBuffer(SAMPLER_CLUSTERS_CLUSTERS, clusterBuffer_);
	gfxDevice->Dispatch(CLUSTERS_X / CLUSTERS_X_THREADS, CLUSTERS_Y / CLUSTERS_Y_THREADS, CLUSTERS_Z / CLUSTERS_Z_THREADS);

// Reset counter
	gfxDevice->SetComputeShader(resetCounterShader_);
	gfxDevice->SetComputeBuffer(SAMPLER_CLUSTERS_ATOMICINDEX, atomicIndexBuffer_);
	gfxDevice->Dispatch(1, 1, 1);

// Light culling
	gfxDevice->SetComputeShader(lightCullingShader_);
	gfxDevice->SetComputeBuffer(SAMPLER_CLUSTERS_CLUSTERS, clusterBuffer_);
	gfxDevice->SetComputeBuffer(SAMPLER_CLUSTERS_ATOMICINDEX, atomicIndexBuffer_);
	gfxDevice->SetComputeBuffer(SAMPLER_CLUSTERS_LIGHTINDICES, lightIndicesBuffer_);
	gfxDevice->SetComputeBuffer(SAMPLER_CLUSTERS_LIGHTGRID, lightGridBuffer_);
	gfxDevice->SetComputeBuffer(SAMPLER_LIGHTS_POINTLIGHTS, nullptr);
	gfxDevice->Dispatch(CLUSTERS_X / CLUSTERS_X_THREADS, CLUSTERS_Y / CLUSTERS_Y_THREADS, CLUSTERS_Z / CLUSTERS_Z_THREADS);
}

}
