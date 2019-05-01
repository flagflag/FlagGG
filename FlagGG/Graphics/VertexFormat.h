#ifndef __VERTEX_FORMAT__
#define __VERTEX_FORMAT__

#include "Export.h"

#include "Graphics/GPUObject.h"
#include "Container/RefCounted.h"

namespace FlagGG
{
	namespace Graphics
	{
		enum VertexType
		{
			VERTEX2D = 0,
			VERTEX3D = 1,
		};

		class FlagGG_API VertexFormat : public GPUObject, public Container::RefCounted
		{
		public:
			VertexFormat(ID3DBlob* shaderCode, VertexType vertexType);

			~VertexFormat() override = default;

			void Initialize() override;

			bool IsValid() override;

		private:
			// 这个以后要改成共享指针，现在先这样吧
			ID3DBlob* shaderCode_{ nullptr };

			VertexType vertexType_;
		};
	}
}

#endif