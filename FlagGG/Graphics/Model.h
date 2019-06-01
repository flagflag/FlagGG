#pragma once

#include "Resource/Resource.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/IndexBuffer.h"
#include "Geometry.h"
#include "Graphics/Batch3D.h"
#include "Container/Ptr.h"
#include "Container/Vector.h"
#include "Scene/Bone.h"

namespace FlagGG
{
	namespace Graphics
	{
		class Model : public Resource::Resource
		{
		public:
			Model(Core::Context* context);

			Container::Vector<Container::SharedPtr<VertexBuffer>>& GetVertexBuffers();

			Container::Vector<Container::SharedPtr<IndexBuffer>>& GetIndexBuffers();

			const Container::Vector<Container::PODVector<uint32_t>>& GetBoneMappings() const;

			const Scene::Skeleton& GetSkeleton() const;

		protected:
			bool BeginLoad(IOFrame::Buffer::IOBuffer* stream) override;

			bool EndLoad() override;

		private:
			Container::Vector<Container::SharedPtr<VertexBuffer>> vertexBuffers_;

			Container::Vector<Container::SharedPtr<IndexBuffer>> indexBuffers_;

			// 每个数组表示不同lod下的图形
			Container::Vector <Container::Vector<Container::SharedPtr<Geometry>>> geometries_;

			Container::Vector<Container::PODVector<uint32_t>> boneMappings_;

			Scene::Skeleton skeleton_;
		};
	}
}
