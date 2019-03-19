#ifndef __MATERIAL__

#include "Resource/Resource.h"
#include "Math/Color.h"

namespace FlagGG
{
	namespace Graphics
	{
		class Texture;

		class Material : public Resource::Resource
		{
		public:
			~Material() override = default;

		protected:
			bool BeginLoad(IOFrame::Buffer::IOBuffer* stream) override;

			bool EndLoad() override;

		private:
			Math::Color diffuse_;

			Math::Color ambient_;

			Math::Color specular_;

			Math::Color emissive_;

			float power_;

			Texture* texture_;
		};
	}
}

#endif
