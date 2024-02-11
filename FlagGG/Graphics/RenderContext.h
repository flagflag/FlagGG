#pragma once

#include "Export.h"

#include "Graphics/Geometry.h"
#include "Graphics/Texture.h"
#include "Graphics/Shader.h"
#include "Graphics/Material.h"
#include "Container/RefCounted.h"
#include "Container/Ptr.h"
#include "Container/Vector.h"
#include "Math/Matrix3x4.h"

namespace FlagGG
{

class Geometry;
class Material;
class Matrix3x4;
class VertexDescription;

// 渲染批次最小单元
struct FlagGG_API RenderContext
{
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
	SharedPtr<Material> material_;
	// 视图可见性Mask
	UInt32 viewMask_{};
};

}
