#include "Graphics/Geometry.h"

namespace FlagGG
{

void Geometry::SetPrimitiveType(PrimitiveType type)
{
	primitiveType_ = type;
}

void Geometry::SetVertexBuffer(UInt32 index, VertexBuffer* vertexBuffer)
{
	if (index >= vertexBuffers_.Size())
		vertexBuffers_.Resize(index + 1);

	vertexBuffers_[index] = vertexBuffer;
}

void Geometry::SetIndexBuffer(IndexBuffer* indexBuffer)
{
	indexBuffer_ = indexBuffer;
}

void Geometry::SetDataRange(UInt32 indexStart, UInt32 indexCount, UInt32 vertexStart, UInt32 vertexCount, UInt32 vertexBaseOffset)
{
	indexStart_ = indexStart;
	indexCount_ = indexCount;
	vertexStart_ = vertexStart;
	vertexCount_ = vertexCount;
	vertexBaseOffset_ = vertexBaseOffset;
}

void Geometry::SetLodDistance(float distance)
{
	lodDistance_ = distance;
}

const Vector<SharedPtr<VertexBuffer>>& Geometry::GetVertexBuffers() const
{
	return vertexBuffers_;
}

VertexBuffer* Geometry::GetVertexBuffer(UInt32 index) const
{
	if (index >= vertexBuffers_.Size())
		return nullptr;
	return vertexBuffers_[index];
}

IndexBuffer* Geometry::GetIndexBuffer() const
{
	return indexBuffer_;
}

float Geometry::GetLodDistance() const
{
	return lodDistance_;
}

PrimitiveType Geometry::GetPrimitiveType() const
{
	return primitiveType_;
}

UInt32 Geometry::GetIndexStart() const
{
	return indexStart_;
}

UInt32 Geometry::GetIndexCount() const
{
	return indexCount_;
}

UInt32 Geometry::GetVertexStart() const
{
	return vertexStart_;
}

UInt32 Geometry::GetVertexCount() const
{
	return vertexCount_;
}

}
