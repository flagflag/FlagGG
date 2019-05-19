#pragma once

#include "Resource/Resource.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/Batch3D.h"
#include "Container/Ptr.h"
#include "Container/Vector.h"

namespace FlagGG
{
	namespace Graphics
	{
		class Model : public Resource::Resource
		{
		public:
			Model(Core::Context* context);

			Container::Vector<Container::SharedPtr<VertexBuffer>>& GetVertexBuffers();

		protected:
			bool BeginLoad(IOFrame::Buffer::IOBuffer* stream) override;

			bool EndLoad() override;

		private:
			Container::Vector<Container::SharedPtr<VertexBuffer>> vertexBuffers_;
		};
	}
}
