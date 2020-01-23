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
	namespace Graphics
	{
		class FlagGG_API Model : public Resource::Resource
		{
		public:
			Model(Core::Context* context);

			void SetVertexBuffers(const Container::Vector<Container::SharedPtr<VertexBuffer>>& vertexBuffers);

			void SetIndexBuffers(const Container::Vector<Container::SharedPtr<IndexBuffer>>& indexBuffers);

			void SetBoneMappings(const Container::Vector<Container::PODVector<UInt32>>& boneMappings);

			void SetNumGeometries(UInt32 numGeometries);

			void SetNumGeometryLodLevels(UInt32 index, UInt32 num);

			bool SetGeometry(UInt32 index, UInt32 lodLevel, Geometry* geometry);

			void SetBoundingBox(Math::BoundingBox& box);

			const Container::Vector<Container::SharedPtr<VertexBuffer>>& GetVertexBuffers() const;

			const Container::Vector<Container::SharedPtr<IndexBuffer>>& GetIndexBuffers() const;

			const Container::Vector<Container::PODVector<UInt32>>& GetBoneMappings() const;

			const Scene::Skeleton& GetSkeleton() const;

			const Container::Vector <Container::Vector<Container::SharedPtr<Geometry>>>& GetGeometries() const;

			Geometry* GetGeometry(UInt32 index, UInt32 lodLevel) const;

			UInt32 GetNumGeometries() const;

			UInt32 GetNumGeometryLodLevels(UInt32 index) const;

			const Math::BoundingBox& GetBoundingBox() const;

		protected:
			bool BeginLoad(IOFrame::Buffer::IOBuffer* stream) override;

			bool EndLoad() override;

			bool BeginSave(IOFrame::Buffer::IOBuffer* stream) override;

			bool EndSave() override;

		private:
			Container::Vector<Container::SharedPtr<VertexBuffer>> vertexBuffers_;

			Container::Vector<Container::SharedPtr<IndexBuffer>> indexBuffers_;

			// 每个数组表示不同lod下的图形
			Container::Vector <Container::Vector<Container::SharedPtr<Geometry>>> geometries_;

			Container::Vector<Container::PODVector<UInt32>> boneMappings_;

			Scene::Skeleton skeleton_;

			Math::BoundingBox boundingBox_;
		};
	}
}
