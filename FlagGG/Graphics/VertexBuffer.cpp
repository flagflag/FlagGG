#include "Graphics/VertexBuffer.h"
#include "Log.h"

namespace FlagGG
{

UInt32 VertexBuffer::GetBindFlags()
{
	return D3D11_BIND_VERTEX_BUFFER;
}

bool VertexBuffer::SetSize(UInt32 vertexCount, const PODVector<VertexElement>& vertexElements)
{
	vertexSize_ = GetVertexSize(vertexElements);
	vertexCount_ = vertexCount;
	vertexElements_ = vertexElements;

	if (!GPUBuffer::SetSize(vertexSize_ * vertexCount_))
	{
		return false;
	}

	UpdateOffset();
}

void VertexBuffer::UpdateOffset()
{
	UInt32 offset = 0;
	for (auto& element : vertexElements_)
	{
		element.offset_ = offset;
		offset += VERTEX_ELEMENT_TYPE_SIZE[element.vertexElementType_];
	}
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
	return vertexElements_;
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
