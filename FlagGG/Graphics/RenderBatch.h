#pragma once

#include "Graphics/RenderContext.h"

namespace FlagGG
{

class Shader;
class Probe;
class Light;
class Texture2D;
class RenderPassInfo;
class VertexBuffer;

struct FlagGG_API RenderBatch
{
	RenderBatch();

	RenderBatch(const RenderContext& renderContext);

	// 图形类型
	GeometryType geometryType_{};
	// 图形数据
	Geometry* geometry_{};
	// 顶点描述
	VertexDescription* vertexDesc_{};
	// 世界Transform
	const Matrix3x4* worldTransform_{};
	UInt32 numWorldTransform_{};
	// 材质
	Material* material_{};
	// Render pass type
	RenderPassType renderPassType_{};
	// Render pass info
	RenderPassInfo* renderPassInfo_{};
	// Vertex shader
	Shader* vertexShader_{};
	// Pixel shader
	Shader* pixelShader_{};
	// 探针
	Probe* probe_{};
};

struct RenderInstanceBatchKey
{
	RenderInstanceBatchKey() = default;

	RenderInstanceBatchKey(const RenderBatch& batch);

	// 图形数据
	Geometry* geometry_{};
	// 材质
	Material* material_{};
	// Render pass info
	RenderPassInfo* renderPassInfo_{};
	// 探针
	Probe* probe_{};

	UInt32 ToHash() const;

	bool operator ==(const RenderInstanceBatchKey& rhs) const
	{
		return
			geometry_ == rhs.geometry_ &&
			material_ == rhs.material_ &&
			renderPassInfo_ == rhs.renderPassInfo_ &&
			probe_ == rhs.probe_;
	}

	bool operator !=(const RenderInstanceBatchKey& rhs) const
	{
		return
			geometry_ != rhs.geometry_ ||
			material_ != rhs.material_ ||
			renderPassInfo_ != rhs.renderPassInfo_ ||
			probe_ != rhs.probe_;
	}
};

struct RenderInstanceBatch : public RenderBatch
{
	RenderInstanceBatch() = default;

	RenderInstanceBatch(const RenderBatch& rhs);

	UInt32 instanceStart_{};
	UInt32 instanceCount_{};
	PODVector<const Matrix3x4*> worldTransforms_;
	PODVector<Vector4> instanceData_;
};

// 渲染批次队列
struct FlagGG_API RenderBatchQueue
{
	void Clear();

	void AddBatch(const RenderBatch& batch, bool allowInstance);

	void SyncInstanceDataToGpu();

	bool HasAnyBatch() const { return renderBatches_.Size() || renderInstanceBatches_.Size(); }

	Vector<RenderBatch> renderBatches_;
	Vector<RenderInstanceBatch> renderInstanceBatches_;
	HashMap<RenderInstanceBatchKey, UInt32> instanceBatchMapping_;
	UInt32 instanceDataCount_{};
	SharedPtr<VertexBuffer> instanceBuffer_;
};

struct FlagGG_API ShadowRenderContext
{
	// 灯光
	Light* light_{};
	// 阴影纹理
	Texture2D* shadowMap_{};
	// 光照渲染批次队列
	RenderBatchQueue renderBatchQueue_;
	// 灯光坐标空间远裁剪
	Real nearSplit_{};
	// 灯光坐标空间近裁剪
	Real farSplit_{};
};

struct FlagGG_API LitRenderContext
{
	// 灯光
	Light* light_{};
	// 阴影纹理
	Texture2D* shadowMap_{};
	// 光照渲染批次队列
	RenderBatchQueue renderBatchQueue_;
};

struct FlagGG_API UnlitRenderContext
{
	RenderBatchQueue renderBatchQueue_;
};

struct FlagGG_API DeferredLitRenderContext
{
	// 灯光
	Light* light_{};
	//
	Probe* probe_{};
	// 阴影纹理
	Texture2D* shadowMap_{};
};

}
