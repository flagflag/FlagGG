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
	vertexSize_ = GetVertexSize(vertexElements);
	vertexCount_ = vertexCount;

	vertexDesc_ = VertexDescFactory::Instance()->Create(vertexElements);

	if (!vertexDesc_)
	{
		FLAGGG_LOG_ERROR("VertexBuffer create vertex description failed.");
		return false;
	}

	gfxBuffer_->SetStride(vertexSize_);
	gfxBuffer_->SetSize(vertexSize_ * vertexCount_);
	gfxBuffer_->SetBind(BUFFER_VERTEX);
	gfxBuffer_->SetAccess(BUFFER_WRITE);
	gfxBuffer_->SetUsage(dynamic ? BUFFER_DYNAMIC : BUFFER_STATIC);
	gfxBuffer_->Apply(nullptr);

	return true;
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

PODVector<VertexElement> VertexBuffer::GetElements(UInt32 elementMask)
{
	PODVector<VertexElement> vertexElements;
	for (UInt32 i = 0; i < MAX_DEFAULT_VERTEX_ELEMENT; ++i)
	{
		if (elementMask & (1u << i))
		{
			vertexElements.Push(DEFAULT_VERTEX_ELEMENT[i]);
		}
	}
	return vertexElements;
}

UInt32 VertexBuffer::GetVertexSize(const PODVector<VertexElement>& elements)
{
	UInt32 vertexSize = 0u;
	for (const auto& element : elements)
	{
		vertexSize += VERTEX_ELEMENT_TYPE_SIZE[element.vertexElementType_];
	}
	return vertexSize;
}

}
