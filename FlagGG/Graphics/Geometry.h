#pragma once

#include "Container/Vector.h"
#include "Container/Ptr.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/IndexBuffer.h"
#include "Core/BaseTypes.h"

namespace FlagGG
{

class FlagGG_API Geometry : public RefCounted
{
public:
	void SetPrimitiveType(PrimitiveType type);
			
	void SetVertexBuffer(UInt32 index, VertexBuffer* vertexBuffer);

	void SetIndexBuffer(IndexBuffer* indexBuffer);

	void SetDataRange(UInt32 indexStart, UInt32 indexCount);

	void SetLodDistance(float distance);

	const Vector<SharedPtr<VertexBuffer>>& GetVertexBuffers() const;

	VertexBuffer* GetVertexBuffer(UInt32 index) const;

	IndexBuffer* GetIndexBuffer() const;

	float GetLodDistance() const;

	PrimitiveType GetPrimitiveType() const;

	UInt32 GetIndexStart() const;

	UInt32 GetIndexCount() const;

	UInt32 GetVertexStart() const;

	UInt32 GetVertexCount() const;

private:
	PrimitiveType primitiveType_;

	Vector<SharedPtr<VertexBuffer>> vertexBuffers_;

	SharedPtr<IndexBuffer> indexBuffer_;

	UInt32 indexStart_;
	UInt32 indexCount_;
	UInt32 vertexStart_;
	UInt32 vertexCount_;
			
	float lodDistance_;
};

}
