#pragma once

#include "Resource/Resource.h"
#include "Graphics/VertexBuffer.h"
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

			const Container::Vector<Container::PODVector<uint32_t>>& GetBoneMappings() const;

			const Scene::Skeleton& GetSkeleton() const;

		protected:
			bool BeginLoad(IOFrame::Buffer::IOBuffer* stream) override;

			bool EndLoad() override;

		private:
			Container::Vector<Container::SharedPtr<VertexBuffer>> vertexBuffers_;

			Container::Vector<Container::PODVector<uint32_t>> boneMappings_;
		
			Scene::Skeleton skeleton_;
		};
	}
}
