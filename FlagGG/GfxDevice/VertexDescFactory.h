//
// 顶点描述工厂
// 非图形层的顶点描述抽象
// 考虑到多线程渲染，产生的顶点描述可被哈希成唯一标识，图形层通过唯一标识生成图形层顶点描述
//

#pragma once

#include "Core/Subsystem.h"
#include "AsyncFrame/Mutex.h"
#include "Graphics/GraphicsDef.h"
#include "Container/Vector.h"
#include "Container/HashMap.h"
#include "Container/Ptr.h"

namespace FlagGG
{

class FlagGG_API VertexDescription : public ThreadSafeRefCounted
{
public:
	VertexDescription(UInt32 uuid, const PODVector<VertexElement>& elements);

	VertexDescription(UInt32 uuid, PODVector<VertexElement>&& elements);

	~VertexDescription() = default;

	// 获取唯一标识
	UInt32 GetUUID() const { return uuid_; }

	// 获取顶点size
	UInt32 GetStrideSize() const { return strideSize_; }

	PODVector<VertexElement>& GetElements() { return elements_; }

private:
	UInt32 uuid_;

	UInt32 strideSize_;

	PODVector<VertexElement> elements_;
};

class FlagGG_API VertexDescFactory : public Subsystem<VertexDescFactory>
{
public:
	~VertexDescFactory() override;

	// 创建顶点描述
	VertexDescription* Create(const PODVector<VertexElement>& elements);

	// 销毁未使用的顶点描述
	void DestroyUnusedVertexDec();

	// 哈希顶点元素，elements需要按照offset_排好序并且offset_没有重复的值
	static UInt32 HashVertexElement(const PODVector<VertexElement>& elements);

	// 通过Mask构建顶点描述
	static PODVector<VertexElement> GetElements(UInt32 elementMask);

	// 通过顶点描述计算顶点单个元素的大小
	static UInt32 GetVertexSize(const PODVector<VertexElement>& elements);

private:
	Mutex mutex_;

	HashMap<UInt32, SharedPtr<VertexDescription>> vertexDescsMap_;
};

}
