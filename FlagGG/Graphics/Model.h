//
// 模型数据
//

#pragma once

#include "Resource/Resource.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/IndexBuffer.h"
#include "Geometry.h"
#include "Graphics/Batch3D.h"
#include "Container/Ptr.h"
#include "Container/Vector.h"
#include "Scene/Bone.h"
#include "Math/BoundingBox.h"

namespace FlagGG
{

class FlagGG_API Model : public Resource
{
	OBJECT_OVERRIDE(Model, Resource);
public:
	Model();

	// 设置顶点buffers
	void SetVertexBuffers(const Vector<SharedPtr<VertexBuffer>>& vertexBuffers);

	// 设置索引buffer
	void SetIndexBuffers(const Vector<SharedPtr<IndexBuffer>>& indexBuffers);

	// 设置骨骼映射
	void SetBoneMappings(const Vector<PODVector<UInt32>>& boneMappings);

	// 设置图形个数
	void SetNumGeometries(UInt32 numGeometries);

	// 设置图形lod级别数
	void SetNumGeometryLodLevels(UInt32 index, UInt32 num);

	// 设置图形
	bool SetGeometry(UInt32 index, UInt32 lodLevel, Geometry* geometry);

	// 设置包围盒
	void SetBoundingBox(const BoundingBox& box);

	// 获取顶点buffers
	const Vector<SharedPtr<VertexBuffer>>& GetVertexBuffers() const;

	// 获取缩影buffers
	const Vector<SharedPtr<IndexBuffer>>& GetIndexBuffers() const;

	// 获取骨骼映射
	const Vector<PODVector<UInt32>>& GetBoneMappings() const;

	// 获取骨架
	const Skeleton& GetSkeleton() const;

	// 获取所有图形
	const Vector <Vector<SharedPtr<Geometry>>>& GetGeometries() const;

	// 获取图形
	Geometry* GetGeometry(UInt32 index, UInt32 lodLevel) const;

	// 获取图形个数
	UInt32 GetNumGeometries() const;

	// 获取第index个图形的lod级别数
	UInt32 GetNumGeometryLodLevels(UInt32 index) const;

	// 获取包围盒
	const BoundingBox& GetBoundingBox() const;

protected:
	bool BeginLoad(IOFrame::Buffer::IOBuffer* stream) override;

	bool EndLoad() override;

	bool BeginSave(IOFrame::Buffer::IOBuffer* stream) override;

	bool EndSave() override;

private:
	Vector<SharedPtr<VertexBuffer>> vertexBuffers_;

	Vector<SharedPtr<IndexBuffer>> indexBuffers_;

	// 每个数组表示不同lod下的图形
	Vector <Vector<SharedPtr<Geometry>>> geometries_;

	Vector<PODVector<UInt32>> boneMappings_;

	Skeleton skeleton_;

	BoundingBox boundingBox_;
};

}
