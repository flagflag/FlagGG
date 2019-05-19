#ifndef __VERTEX_FORMAT__
#define __VERTEX_FORMAT__

#include "Export.h"

#include "Graphics/GPUObject.h"
#include "Graphics/Shader.h"
#include "Graphics/VertexBuffer.h"
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
			VertexFormat(Shader* VSShader, VertexBuffer** vertexBuffer);

			bool IsValid() override;

		private:
			void Initialize() override;
		};
	}
}

#endif