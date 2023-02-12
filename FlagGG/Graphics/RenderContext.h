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

// ��Ⱦ������С��Ԫ
struct FlagGG_API RenderContext
{
	// ͼ������
	GeometryType geometryType_{};
	// ͼ������
	Geometry* geometry_{};
	// ����Transform
	const Matrix3x4* worldTransform_{};
	UInt32 numWorldTransform_{};
	// ����
	Material* material_{};
	// ��ͼ�ɼ���Mask
	UInt32 viewMask_{};
};

}
