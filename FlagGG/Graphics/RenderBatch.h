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

	// ͼ������
	GeometryType geometryType_{};
	// ͼ������
	Geometry* geometry_{};
	// ����Transform
	const Matrix3x4* worldTransform_{};
	UInt32 numWorldTransform_{};
	// ����
	Material* material_{};
	// Render pass type
	RenderPassType renderPassType_{};
	// Vertex shader
	Shader* vertexShader_{};
	// Pixel shader
	Shader* pixelShader_{};
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
	Light* light_{};
	// ��Ӱ����
	Texture2D* shadowMap_{};
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
	Light* light_{};
	// ��Ӱ����
	Texture2D* shadowMap_{};
	// ������Ⱦ���ζ���
	RenderBatchQueue renderBatchQueue_;
};

struct FlagGG_API UnlitRenderContext
{
	RenderBatchQueue renderBatchQueue_;
};

}
