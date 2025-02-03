#include "Batch.h"
#include "Texture.h"

namespace FlagGG
{

Batch::Batch(BatchType type, VertexVector* vertexs, USize vertexSize, Texture* texture)
	: type_(type)
	, vertexs_(vertexs)
	, vertexSize_(vertexSize)
	, texture_(texture)
	, blendMode_(BLEND_REPLACE)
{
	owner_ = vertexs_ == nullptr;

	if (vertexs_ == nullptr)
	{
		vertexs_ = new VertexVector();
		vertexStart_ = 0;
		vertexEnd_ = 0;
	}
	else
	{
		vertexStart_ = vertexEnd_ = vertexs->Size();
	}
}

Batch::~Batch()
{
	if (owner_ && vertexs_)
	{
		delete vertexs_;
		vertexs_ = 0;
	}
}

const char* Batch::GetVertexs() const
{
	return (const char*)&(vertexs_->At(vertexStart_));
}

UInt32 Batch::GetVertexCount() const
{
	return (vertexEnd_ - vertexStart_) * 4u / vertexSize_;
}

void Batch::SetTexture(Texture* texture)
{
	texture_ = texture;
}

void Batch::SetBlendMode(BlendMode blendMode)
{
	blendMode_ = blendMode;
}

BatchType Batch::GetType() const
{
	return type_;
}

}
