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
public:
	Model(Context* context);

	void SetVertexBuffers(const Vector<SharedPtr<VertexBuffer>>& vertexBuffers);

	void SetIndexBuffers(const Vector<SharedPtr<IndexBuffer>>& indexBuffers);

	void SetBoneMappings(const Vector<PODVector<UInt32>>& boneMappings);

	void SetNumGeometries(UInt32 numGeometries);

	void SetNumGeometryLodLevels(UInt32 index, UInt32 num);

	bool SetGeometry(UInt32 index, UInt32 lodLevel, Geometry* geometry);

	void SetBoundingBox(BoundingBox& box);

	const Vector<SharedPtr<VertexBuffer>>& GetVertexBuffers() const;

	const Vector<SharedPtr<IndexBuffer>>& GetIndexBuffers() const;

	const Vector<PODVector<UInt32>>& GetBoneMappings() const;

	const Skeleton& GetSkeleton() const;

	const Vector <Vector<SharedPtr<Geometry>>>& GetGeometries() const;

	Geometry* GetGeometry(UInt32 index, UInt32 lodLevel) const;

	UInt32 GetNumGeometries() const;

	UInt32 GetNumGeometryLodLevels(UInt32 index) const;

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
