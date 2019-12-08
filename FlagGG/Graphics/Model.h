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

			void SetBoneMappings(const Container::Vector<Container::PODVector<uint32_t>>& boneMappings);

			void SetNumGeometries(uint32_t numGeometries);

			void SetNumGeometryLodLevels(uint32_t index, uint32_t num);

			bool SetGeometry(uint32_t index, uint32_t lodLevel, Geometry* geometry);

			const Container::Vector<Container::SharedPtr<VertexBuffer>>& GetVertexBuffers() const;

			const Container::Vector<Container::SharedPtr<IndexBuffer>>& GetIndexBuffers() const;

			const Container::Vector<Container::PODVector<uint32_t>>& GetBoneMappings() const;

			const Scene::Skeleton& GetSkeleton() const;

			const Container::Vector <Container::Vector<Container::SharedPtr<Geometry>>>& GetGeometries() const;

			Geometry* GetGeometry(uint32_t index, uint32_t lodLevel) const;

			uint32_t GetNumGeometries() const;

			uint32_t GetNumGeometryLodLevels(uint32_t index) const;

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

			Container::Vector<Container::PODVector<uint32_t>> boneMappings_;

			Scene::Skeleton skeleton_;

			Math::BoundingBox boundingBox_;
		};
	}
}
