//
// 3D图形数据
//

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
	// 设置图元类型
	void SetPrimitiveType(PrimitiveType type);
			
	// 设置顶点buffer
	void SetVertexBuffer(UInt32 index, VertexBuffer* vertexBuffer);

	// 设置索引buffer
	void SetIndexBuffer(IndexBuffer* indexBuffer);

	// 设置buffer的范围
	void SetDataRange(UInt32 indexStart, UInt32 indexCount, UInt32 vertexStart = 0u, UInt32 vertexCount = 0u, UInt32 vertexBaseOffset = 0u);

	// 设置lod距离
	void SetLodDistance(float distance);

	// 获取顶点buffers
	const Vector<SharedPtr<VertexBuffer>>& GetVertexBuffers() const;

	// 获取第index个顶点buffer
	VertexBuffer* GetVertexBuffer(UInt32 index) const;

	// 获取索引buffer
	IndexBuffer* GetIndexBuffer() const;

	// 获取lod距离
	float GetLodDistance() const;

	// 获取图元类型
	PrimitiveType GetPrimitiveType() const;

	// 获取索引开始的位置
	UInt32 GetIndexStart() const;

	// 获取索引个数
	UInt32 GetIndexCount() const;

	// 获取顶点开始的位置
	UInt32 GetVertexStart() const;

	// 获取顶点的个数
	UInt32 GetVertexCount() const;

private:
	PrimitiveType primitiveType_;

	Vector<SharedPtr<VertexBuffer>> vertexBuffers_;

	SharedPtr<IndexBuffer> indexBuffer_;

	UInt32 indexStart_;
	UInt32 indexCount_;
	UInt32 vertexStart_;
	UInt32 vertexCount_;
	UInt32 vertexBaseOffset_;
			
	float lodDistance_;
};

}
