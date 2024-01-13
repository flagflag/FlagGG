//
// ��̬����buffer
// �ɷ���������������Ϊ��ͬMesh������
// ͨ�������Ż����������Buffer����ͬ��GPU���ݺ�ʱ���������
//

#pragma once

#include "Graphics/IndexBuffer.h"
#include "Allocator/AllocatorLJ.h"

namespace FlagGG
{

class GlobalDynamicIndexBuffer
{
public:
	struct Allocation
	{
		UInt32* indexData_;
		IndexBuffer* indexBuffer_;
		UInt32 offsetInCount_;
		UInt32 sizeInCount_;
	};

	GlobalDynamicIndexBuffer();

	~GlobalDynamicIndexBuffer();

	// ������ٸ�����
	Allocation Allocate(UInt32 count);

	// �������������ύ��GPU
	void CommitToGPU();

	// �������з����Allocation
	void Clear();

private:
	// ����buffer������Alloction�ĺϲ�
	SharedPtr<IndexBuffer> indexBuffer_;

	UInt32 indexCount_;

	AllocatorLJInstance alloctor_;

	PODVector<Allocation> allocations_;
};

}
