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

struct FlagGG_API RenderBatch
{
	RenderBatch();

	RenderBatch(const RenderContext& renderContext);

	// ͼ������
	GeometryType geometryType_{};
	// ͼ������
	Geometry* geometry_{};
	// ����
	Material* material_{};
	// render pass type
	RenderPassType renderPassType_{};
	// ̽��
	Probe* probe_{};
};

// ��Ⱦ���ζ���
struct FlagGG_API RenderBatchQueue
{
	Vector<RenderBatch> renderBatches_;
	Vector<RenderBatch> renderBatchGroups_;
};

struct FlagGG_API ShadowRenderContext
{
	// �ƹ�
	Light* light_;
	// ��Ӱ����
	Texture2D* shadowMap_;
	// ������Ⱦ���ζ���
	RenderBatchQueue renderBatchQueue_;
	// �ƹ�����ռ�Զ�ü�
	Real nearSplit_{};
	// �ƹ�����ռ���ü�
	Real farSplit_{};
};

struct FlagGG_API LitRenderContext
{
	// �ƹ�
	Light* light_;
	// ��Ӱ����
	Texture2D* shadowMap_;
	// ������Ⱦ���ζ���
	RenderBatchQueue renderBatchQueue_;
};

struct FlagGG_API UnlitRenderContext
{
	RenderBatchQueue renderBatchQueue_;
};

}
