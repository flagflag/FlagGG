#pragma once

#include "Graphics/RenderContext.h"

namespace FlagGG
{

class Shader;
class Probe;
class Light;
class Texture2D;

struct FlagGG_API RenderBatch
{
	RenderBatch();

	RenderBatch(const RenderContext& renderContext);

	// 图形类型
	GeometryType geometryType_{};
	// 图形数据
	Geometry* geometry_{};
	// 世界Transform
	const Matrix3x4* worldTransform_{};
	UInt32 numWorldTransform_{};
	// 材质
	Material* material_{};
	// Render pass type
	RenderPassType renderPassType_{};
	// Vertex shader
	Shader* vertexShader_{};
	// Pixel shader
	Shader* pixelShader_{};
	// 探针
	Probe* probe_{};
};

// 渲染批次队列
struct FlagGG_API RenderBatchQueue
{
	Vector<RenderBatch> renderBatches_;
	Vector<RenderBatch> renderBatchGroups_;
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

}
