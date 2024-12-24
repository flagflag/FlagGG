//
// Compute pass
//

#pragma once

#include "Core/Object.h"
#include "Container/Ptr.h"

namespace FlagGG
{

struct RenderPiplineContext;
class GfxBuffer;
class GfxShader;
class ShaderParameters;

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
	virtual void Dispatch(RenderPiplineContext& context) = 0;
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
	void Dispatch(RenderPiplineContext& context) override;

protected:
	void InitShader();

	void InitShaderParameters(RenderPiplineContext& context);

private:
	// compute buffer
	SharedPtr<GfxBuffer> clusterBuffer_;          // Cluster的AABB
	SharedPtr<GfxBuffer> atomicIndexBuffer_;      // 原子变量，用来保存lightIndices的count
	SharedPtr<GfxBuffer> lightGridBuffer_;        // 保存每个Cluster所受光源下标的范围（例如：cluster(x, y, z) = [offset, count]，指向lightIndicesBuffer_[offset ~ offset + count - 1]）
	SharedPtr<GfxBuffer> lightIndicesBuffer_;     // 保存光源的下标x（指向lightsBuffer[x]）
	SharedPtr<GfxBuffer> lightsBuffer_;           // 光源信息（position, falloffRange，intensity，radius）

	// shader
	SharedPtr<GfxShader> clusterBuildingShader_;
	SharedPtr<GfxShader> resetCounterShader_;
	SharedPtr<GfxShader> lightCullingShader_;

	//
	SharedPtr<ShaderParameters> shaderParameters_;
};

}
