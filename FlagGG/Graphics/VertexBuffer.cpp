#include "Graphics/VertexBuffer.h"
#include "Log.h"

namespace FlagGG
{

static const PODVector<VertexElement> EMPTY_VERTEX_ELEMENT;

VertexBuffer::~VertexBuffer()
{

}

bool VertexBuffer::SetSize(UInt32 vertexCount, const PODVector<VertexElement>& vertexElements, bool dynamic)
{
	return SetSize(vertexCount, GetSubsystem<VertexDescFactory>()->Create(vertexElements), dynamic);
}

bool VertexBuffer::SetSize(UInt32 vertexCount, VertexDescription* vertexDesc, bool dynamic)
{
	if (!vertexDesc)
	{
		FLAGGG_LOG_ERROR("VertexBuffer invalid vertex description.");
		return false;
	}

	vertexDesc_ = vertexDesc;
	vertexSize_ = vertexDesc_->GetStrideSize();
	vertexCount_ = vertexCount;

	gfxBuffer_->SetStride(vertexSize_);
	gfxBuffer_->SetSize(vertexSize_ * vertexCount_);
	gfxBuffer_->SetBind(BUFFER_BIND_VERTEX);
	gfxBuffer_->SetAccess(dynamic ? BUFFER_ACCESS_WRITE : BUFFER_ACCESS_NONE);
	gfxBuffer_->SetUsage(dynamic ? BUFFER_USAGE_DYNAMIC : BUFFER_USAGE_STATIC);
	gfxBuffer_->Apply(nullptr);

	return true;
}

void VertexBuffer::SetData(const void* data)
{
	void* dest = Lock(0, vertexCount_);
	memcpy(dest, data, vertexSize_ * vertexCount_);
	Unlock();
}

UInt32 VertexBuffer::GetVertexSize() const
{
	return vertexSize_;
}

UInt32 VertexBuffer::GetVertexCount() const
{
	return vertexCount_;
}

const PODVector<VertexElement>& VertexBuffer::GetElements() const
{
	return vertexDesc_ ? vertexDesc_->GetElements() : EMPTY_VERTEX_ELEMENT;
}

}
