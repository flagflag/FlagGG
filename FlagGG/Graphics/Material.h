#ifndef __MATERIAL__

#include "Resource/Resource.h"
#include "Math/Color.h"
#include "Container/Ptr.h"

namespace FlagGG
{
	namespace Graphics
	{
		class Texture;
		class Shader;

		class Material : public Resource::Resource
		{
		public:
			~Material() override = default;

		protected:
			bool BeginLoad(IOFrame::Buffer::IOBuffer* stream) override;

			bool EndLoad() override;

		protected:
			Container::SharedPtr<Texture> texture_;

			Container::SharedPtr<Shader> shader_;
		};
	}
}

#endif
