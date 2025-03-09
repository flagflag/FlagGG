//
// Compute pass
//

#pragma once

#include "Core/Object.h"
#include "Container/Ptr.h"

#define SAMPLER_CLUSTERS_OFFSET 9
#define SAMPLER_CLUSTERS_CLUSTERS 0
#define SAMPLER_CLUSTERS_ATOMICINDEX 1
#define SAMPLER_CLUSTERS_LIGHTINDICES 2
#define SAMPLER_CLUSTERS_LIGHTGRID 3
#define SAMPLER_LIGHTS_POINTLIGHTS 4

namespace FlagGG
{

struct RenderPiplineContext;
class GfxBuffer;
class GfxShader;
class ShaderParameters;
struct ClusterLightInfo;

// Compute pass基类
class FlagGG_API ComputePass : public Object
{
	OBJECT_OVERRIDE(ComputePass, Object);
public:
	explicit ComputePass();

	~ComputePass() override;

	// 清理
	virtual void Clear() = 0;

	// 执行compute
	virtual void Dispatch(RenderPiplineContext& renderPiplineContext) = 0;

	// 绑定gpu对象
	virtual void BindGpuObject() = 0;
};

// Cluster light pass
class FlagGG_API ClusterLightPass : public ComputePass
{
	OBJECT_OVERRIDE(ClusterLightPass, ComputePass);
public:
	explicit ClusterLightPass();

	~ClusterLightPass() override;

	// 清理
	void Clear() override;

	// 执行compute
	void Dispatch(RenderPiplineContext& renderPiplineContext) override;

	// 绑定gpu对象
	void BindGpuObject() override;

	static UInt32 GetComputeBinding(UInt32 binding);

	static UInt32 GetRasterizerBinding(UInt32 binding);

protected:
	void InitShader();

	void InitShaderParameters(RenderPiplineContext& renderPiplineContext);

	void ApplyLightInfo(RenderPiplineContext& renderPiplineContext);

private:
	// compute buffer
	SharedPtr<GfxBuffer> clusterBuffer_;          // Cluster的AABB
	SharedPtr<GfxBuffer> atomicIndexBuffer_;      // 原子变量，用来保存lightIndices的count
	SharedPtr<GfxBuffer> lightGridBuffer_;        // 保存每个Cluster所受光源下标的范围（例如：cluster(x, y, z) = [offset, count]，指向lightIndicesBuffer_[offset ~ offset + count - 1]）
	SharedPtr<GfxBuffer> lightIndicesBuffer_;     // 保存光源的下标x（指向lightsBuffer[x]）
	SharedPtr<GfxBuffer> lightsBuffer_;           // 光源信息（position, falloffRange，intensity，radius）

	//
	PODVector<ClusterLightInfo> lightsBufferCache_;

	// shader
	SharedPtr<GfxShader> clusterBuildingShader_;
	SharedPtr<GfxShader> resetCounterShader_;
	SharedPtr<GfxShader> lightCullingShader_;

	//
	SharedPtr<ShaderParameters> shaderParameters_;
};

}
