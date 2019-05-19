#ifndef __RENDER_CONTEXT__
#define __RENDER_CONTEXT__

#include "Export.h"

#include "Graphics/VertexBuffer.h"
#include "Graphics/Texture.h"
#include "Graphics/Shader.h"
#include "Container/RefCounted.h"
#include "Container/Ptr.h"
#include "Container/Vector.h"

#include <vector>

namespace FlagGG
{
	namespace Graphics
	{
		struct FlagGG_API RenderContext
		{
			PrimitiveType primitiveType_{ PRIMITIVE_TRIANGLE };

			Container::Vector<Container::SharedPtr<VertexBuffer>>* vertexBuffers_;

			Container::SharedPtr<Texture> texture_;

			Container::SharedPtr<Shader> VSShader_;

			Container::SharedPtr<Shader> PSShader_;
		};
	}
}

#endif