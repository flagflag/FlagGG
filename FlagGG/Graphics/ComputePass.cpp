#include "ComputePass.h"
#include "GfxDevice/GfxDevice.h"
#include "GfxDevice/GfxBuffer.h"
#include "Graphics/Shader.h"
#include "Graphics/RenderPipline.h"
#include "Scene/Camera.h"
#include "Scene/Light.h"
#include "Scene/Node.h"
#include "Resource/ResourceCache.h"
#include "Memory/Memory.h"

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

static constexpr UInt32 MAX_LIGHTS_PER_CLUSTER = 50;

struct ClusterLightInfo
{
	Vector3 position_;
	float falloffRange_;
	Vector3 intensity_;
	float radius_;
};

ClusterLightPass::ClusterLightPass()
{
	auto* gfxDevice = GfxDevice::GetDevice();
	
	// RWStructuredBuffer<float4>
	const UInt32 clusterCount = CLUSTERS_X * CLUSTERS_Y * CLUSTERS_Z;
	clusterBuffer_ = gfxDevice->CreateBuffer();
	clusterBuffer_->SetStride(sizeof(Vector4));
	clusterBuffer_->SetSize(clusterCount * 2u * sizeof(Vector4));
	clusterBuffer_->SetBind(BUFFER_BIND_COMPUTE_READ | BUFFER_BIND_COMPUTE_WRITE);
	clusterBuffer_->SetAccess(BUFFER_ACCESS_NONE);
	clusterBuffer_->SetUsage(BUFFER_USAGE_DYNAMIC);
	clusterBuffer_->Apply(nullptr);
	clusterBuffer_->SetGpuTag("ClusterBuffer");

	// RWStructuredBuffer<uint>
	atomicIndexBuffer_ = gfxDevice->CreateBuffer();
	atomicIndexBuffer_->SetStride(sizeof(UInt32));
	atomicIndexBuffer_->SetSize(sizeof(UInt32));
	atomicIndexBuffer_->SetBind(BUFFER_BIND_COMPUTE_READ | BUFFER_BIND_COMPUTE_WRITE);
	atomicIndexBuffer_->SetAccess(BUFFER_ACCESS_NONE);
	atomicIndexBuffer_->SetUsage(BUFFER_USAGE_DYNAMIC);
	atomicIndexBuffer_->Apply(nullptr);
	atomicIndexBuffer_->SetGpuTag("AtomicIndexBuffer");

	// RWStructuredBuffer<uint>
	lightGridBuffer_ = gfxDevice->CreateBuffer();
	lightGridBuffer_->SetStride(sizeof(UInt32));
	lightGridBuffer_->SetSize(clusterCount * 4 * sizeof(UInt32));
	lightGridBuffer_->SetBind(BUFFER_BIND_RASTE_READ | BUFFER_BIND_COMPUTE_READ | BUFFER_BIND_COMPUTE_WRITE);
	lightGridBuffer_->SetAccess(BUFFER_ACCESS_NONE);
	lightGridBuffer_->SetUsage(BUFFER_USAGE_DYNAMIC);
	lightGridBuffer_->Apply(nullptr);
	lightGridBuffer_->SetGpuTag("LightGridBuffer");

	// RWStructuredBuffer<uint>
	lightIndicesBuffer_ = gfxDevice->CreateBuffer();
	lightIndicesBuffer_->SetStride(sizeof(UInt32));
	lightIndicesBuffer_->SetSize(clusterCount * MAX_LIGHTS_PER_CLUSTER * sizeof(UInt32));
	lightIndicesBuffer_->SetBind(BUFFER_BIND_RASTE_READ | BUFFER_BIND_COMPUTE_READ | BUFFER_BIND_COMPUTE_WRITE);
	lightIndicesBuffer_->SetAccess(BUFFER_ACCESS_NONE);
	lightIndicesBuffer_->SetUsage(BUFFER_USAGE_DYNAMIC);
	lightIndicesBuffer_->Apply(nullptr);
	lightIndicesBuffer_->SetGpuTag("LightIndicesBuffer");

	lightsBuffer_ = gfxDevice->CreateBuffer();
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
		ToString("SAMPLER_CLUSTERS_CLUSTERS=%d ", GetComputeBinding(SAMPLER_CLUSTERS_CLUSTERS)),
		ToString("SAMPLER_CLUSTERS_ATOMICINDEX=%d ", GetComputeBinding(SAMPLER_CLUSTERS_ATOMICINDEX)),
		ToString("SAMPLER_CLUSTERS_LIGHTINDICES=%d ", GetComputeBinding(SAMPLER_CLUSTERS_LIGHTINDICES)),
		ToString("SAMPLER_CLUSTERS_LIGHTGRID=%d ", GetComputeBinding(SAMPLER_CLUSTERS_LIGHTGRID)),
		ToString("SAMPLER_LIGHTS_POINTLIGHTS=%d ", GetComputeBinding(SAMPLER_LIGHTS_POINTLIGHTS)),
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

void ClusterLightPass::InitShaderParameters(RenderPiplineContext& renderPiplineContext)
{
	static const StringHash CLUSTER_SIZES("clusterSizes");
	static const StringHash CLUSTER_ORIGIN("clusterOrigin");
	static const StringHash CLUSTER_VIEW_RECT("viewRect");
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
		shaderParameters_->AddParametersDefine<Vector4>(CLUSTER_VIEW_RECT);
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

	shaderParameters_->SetValue(CLUSTER_SIZES, Vector2((renderPiplineContext.renderSolution_.x_ + CLUSTERS_X - 1) / CLUSTERS_X, (renderPiplineContext.renderSolution_.y_ + CLUSTERS_Y - 1) / CLUSTERS_Y));
	shaderParameters_->SetValue(CLUSTER_ORIGIN, Vector2(0.0f, 0.0f));
	shaderParameters_->SetValue(CLUSTER_VIEW_RECT, Vector4(0, 0, renderPiplineContext.renderSolution_.x_, renderPiplineContext.renderSolution_.y_));
	shaderParameters_->SetValue(CLUSTER_NEAR_CLIP, renderPiplineContext.camera_->GetNearClip());
	shaderParameters_->SetValue(CLUSTER_FAR_CLIP, renderPiplineContext.camera_->GetFarClip());
	shaderParameters_->SetValue(VIEW_MATERIX, renderPiplineContext.camera_->GetViewMatrix().ToMatrix4());
	shaderParameters_->SetValue(INV_PROJ, renderPiplineContext.camera_->GetProjectionMatrix().Inverse());
	shaderParameters_->SetValue(POINT_LIGHT_COUNT, lightsBufferCache_.Size());
	shaderParameters_->SetValue(SPOT_LIGHT_COUNT, 0u);
	shaderParameters_->SetValue(NON_PUNCTUAL_POINT_LIGHT_COUNT, 0u);
	shaderParameters_->SetValue(POINT_LIGHT_OFFSET, 0u);
	shaderParameters_->SetValue(SPOT_LIGHT_OFFSET, 0u);
	shaderParameters_->SetValue(NON_PUNCTUAL_POINT_LIGHT_OFFSET, 0u);
}

void ClusterLightPass::ApplyLightInfo(RenderPiplineContext& renderPiplineContext)
{
	UInt32 dynamicLightCount = 0;
	for (auto& light : renderPiplineContext.lights_)
	{
		if (light->GetLightType() == LIGHT_TYPE_POINT ||
			light->GetLightType() == LIGHT_TYPE_SPOT)
		{
			++dynamicLightCount;
		}
	}

	{
		lightsBufferCache_.Clear();
		lightsBufferCache_.Reserve(dynamicLightCount);

		for (auto& light : renderPiplineContext.lights_)
		{
			if (light->GetLightType() == LIGHT_TYPE_POINT ||
				light->GetLightType() == LIGHT_TYPE_SPOT)
			{
				ClusterLightInfo& info = lightsBufferCache_.Append();
				info.position_ = light->GetNode()->GetWorldPosition();
				info.falloffRange_ = light->GetRange();
				const Color color = light->GetEffectiveColor();
				info.intensity_ = Vector3(color.r_, color.g_, color.b_);
				info.radius_ = light->GetRange();
			}
		}

		if (lightsBufferCache_.Size())
		{
			const UInt32 bufferSize = lightsBufferCache_.Size() * sizeof(ClusterLightInfo);
			if (bufferSize > lightsBuffer_->GetDesc().size_ || bufferSize < lightsBuffer_->GetDesc().size_ / 2)
			{
				// RWStructuredBuffer<float4>
				lightsBuffer_->SetStride(sizeof(Vector4));
				lightsBuffer_->SetSize(lightsBufferCache_.Size() * sizeof(ClusterLightInfo));
				lightsBuffer_->SetBind(BUFFER_BIND_UNIFORM | BUFFER_BIND_RASTE_READ | BUFFER_BIND_COMPUTE_READ);
				lightsBuffer_->SetAccess(BUFFER_ACCESS_NONE);
				lightsBuffer_->SetUsage(BUFFER_USAGE_DYNAMIC);
				lightsBuffer_->Apply(lightsBufferCache_.Buffer());
				lightsBuffer_->SetGpuTag("LightsBuffer");
			}
			else
			{
				auto* dest = lightsBuffer_->BeginWrite(0, bufferSize);
				Memory::Memcpy(dest, lightsBufferCache_.Buffer(), bufferSize);
				lightsBuffer_->EndWrite(bufferSize);
			}
		}
	}
}

void ClusterLightPass::Dispatch(RenderPiplineContext& renderPiplineContext)
{
	if (!clusterBuildingShader_)
	{
		InitShader();
	}

	ApplyLightInfo(renderPiplineContext);

	InitShaderParameters(renderPiplineContext);

	auto* gfxDevice = GfxDevice::GetDevice();
	gfxDevice->ResetRenderTargets();
	gfxDevice->SetEngineShaderParameters(shaderParameters_);

// Cluster building
	gfxDevice->SetComputeShader(clusterBuildingShader_);
	gfxDevice->ResetComputeResources();
	gfxDevice->SetComputeBuffer(GetComputeBinding(SAMPLER_CLUSTERS_CLUSTERS), clusterBuffer_, COMPUTE_BIND_ACCESS_READWRITE);
	gfxDevice->Dispatch(CLUSTERS_X / CLUSTERS_X_THREADS, CLUSTERS_Y / CLUSTERS_Y_THREADS, CLUSTERS_Z / CLUSTERS_Z_THREADS);

// Reset counter
	gfxDevice->SetComputeShader(resetCounterShader_);
	gfxDevice->ResetComputeResources();
	gfxDevice->SetComputeBuffer(GetComputeBinding(SAMPLER_CLUSTERS_ATOMICINDEX), atomicIndexBuffer_, COMPUTE_BIND_ACCESS_READWRITE);
	gfxDevice->Dispatch(1, 1, 1);

// Light culling
	gfxDevice->SetComputeShader(lightCullingShader_);
	gfxDevice->ResetComputeResources();
	gfxDevice->SetComputeBuffer(GetComputeBinding(SAMPLER_CLUSTERS_CLUSTERS), clusterBuffer_, COMPUTE_BIND_ACCESS_READWRITE);
	gfxDevice->SetComputeBuffer(GetComputeBinding(SAMPLER_CLUSTERS_ATOMICINDEX), atomicIndexBuffer_, COMPUTE_BIND_ACCESS_READWRITE);
	gfxDevice->SetComputeBuffer(GetComputeBinding(SAMPLER_CLUSTERS_LIGHTINDICES), lightIndicesBuffer_, COMPUTE_BIND_ACCESS_READWRITE);
	gfxDevice->SetComputeBuffer(GetComputeBinding(SAMPLER_CLUSTERS_LIGHTGRID), lightGridBuffer_, COMPUTE_BIND_ACCESS_READWRITE);
	gfxDevice->SetComputeBuffer(GetComputeBinding(SAMPLER_LIGHTS_POINTLIGHTS), lightsBuffer_, COMPUTE_BIND_ACCESS_READ);
	gfxDevice->Dispatch(CLUSTERS_X / CLUSTERS_X_THREADS, CLUSTERS_Y / CLUSTERS_Y_THREADS, CLUSTERS_Z / CLUSTERS_Z_THREADS);

// Reset
	gfxDevice->ResetComputeResources();
}

void ClusterLightPass::BindGpuObject()
{
	auto* gfxDevice = GfxDevice::GetDevice();
	gfxDevice->SetBuffer(GetRasterizerBinding(SAMPLER_CLUSTERS_CLUSTERS), clusterBuffer_);
	gfxDevice->SetBuffer(GetRasterizerBinding(SAMPLER_CLUSTERS_ATOMICINDEX), atomicIndexBuffer_);
	gfxDevice->SetBuffer(GetRasterizerBinding(SAMPLER_CLUSTERS_LIGHTINDICES), lightIndicesBuffer_);
	gfxDevice->SetBuffer(GetRasterizerBinding(SAMPLER_CLUSTERS_LIGHTGRID), lightGridBuffer_);
	gfxDevice->SetBuffer(GetRasterizerBinding(SAMPLER_LIGHTS_POINTLIGHTS), lightsBuffer_);
	gfxDevice->SetMaterialShaderParameters(shaderParameters_);
}

UInt32 ClusterLightPass::GetComputeBinding(UInt32 binding)
{
	return binding;
}

UInt32 ClusterLightPass::GetRasterizerBinding(UInt32 binding)
{
	return binding + SAMPLER_CLUSTERS_OFFSET;
}

}
