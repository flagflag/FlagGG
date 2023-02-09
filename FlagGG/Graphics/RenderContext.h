#pragma once

#include "Export.h"

#include "Graphics/Geometry.h"
#include "Graphics/Texture.h"
#include "Graphics/Shader.h"
#include "Graphics/Material.h"
#include "Container/RefCounted.h"
#include "Container/Ptr.h"
#include "Container/Vector.h"
#include "Scene/Light.h"
#include "Math/Matrix3x4.h"

namespace FlagGG
{

class Geometry;
class Material;
class Light;
class Probe;
class Texture2D;
class Matrix3x4;

// 渲染批次最小单元
struct FlagGG_API RenderContext
{
	// 图形类型
	GeometryType geometryType_{};
	// 图形数据
	Geometry* geometry_{};
	// 世界Transform
	const Matrix3x4* worldTransform_{};
	UInt32 numWorldTransform_{};
	// 材质
	Material* material_{};
	// 视图可见性Mask
	UInt32 viewMask_{};
};

struct FlagGG_API RenderBatch
{
	RenderBatch();

	RenderBatch(const RenderContext& renderContext);

	// 图形类型
	GeometryType geometryType_{};
	// 图形数据
	Geometry* geometry_{};
	// 材质
	Material* material_{};
	// render pass type
	RenderPassType renderPassType_{};
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
	Light* light_;
	// 阴影纹理
	Texture2D* shadowMap_;
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
	Light* light_;
	// 阴影纹理
	Texture2D* shadowMap_;
	// 光照渲染批次队列
	RenderBatchQueue renderBatchQueue_;
};

struct FlagGG_API UnlitRenderContext
{
	RenderBatchQueue renderBatchQueue_;
};

}
