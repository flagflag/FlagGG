#pragma once

#include "Resource/Resource.h"
#include "Graphics/Batch3D.h"
#include "Container/Ptr.h"

namespace FlagGG
{
	namespace Graphics
	{
		class Model : public Resource::Resource
		{
		public:
			Model(Core::Context* context);

			Batch3D* GetBatch() const;

		protected:
			bool BeginLoad(IOFrame::Buffer::IOBuffer* stream) override;

			bool EndLoad() override;

		private:
			Container::SharedPtr<Batch3D> batch_;
		};
	}
}
