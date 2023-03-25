//
// 顶点缓冲区
//

#pragma once

#include "GPUBuffer.h"
#include "Container/Vector.h"
#include "GfxDevice/VertexDescFactory.h"

namespace FlagGG
{

class VertexDescription;

class FlagGG_API VertexBuffer : public GPUBuffer
{
public:
	~VertexBuffer() override;

	/**
	 * 设置vb的大小
	 * vertexCount     - vb顶点元素的个数
	 * vertexElements  - vb顶点描述
	 * dynamic         - 是否是动态bufer
	 */
	bool SetSize(UInt32 vertexCount, const PODVector<VertexElement>& vertexElements, bool dynamic = false);

	// 设置vb的数据
	void SetData(const void* data);

	// 获取顶点单个元素的大小
	UInt32 GetVertexSize() const;

	// 获取顶点的个数
	UInt32 GetVertexCount() const;

	// 获取顶点描述
	VertexDescription* GetVertexDescription() const { return vertexDesc_; }

	// 获取顶点描述
	const PODVector<VertexElement>& GetElements() const;

	// 通过Mask构建顶点描述
	static PODVector<VertexElement> GetElements(UInt32 elementMask);

	// 通过顶点描述计算顶点单个元素的大小
	static UInt32 GetVertexSize(const PODVector<VertexElement>& elements);

private:
	// 顶点描述（图形层）
	SharedPtr<VertexDescription> vertexDesc_;

	UInt32 vertexSize_{ 0 };
	UInt32 vertexCount_{ 0 };
};

}
