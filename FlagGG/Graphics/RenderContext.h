#ifndef __RENDER_CONTEXT__
#define __RENDER_CONTEXT__

#include "Export.h"

#include "Graphics/VertexBuffer.h"
#include "Graphics/IndexBuffer.h"
#include "Graphics/Texture.h"
#include "Graphics/Shader.h"
#include "Container/RefCounted.h"
#include "Container/Ptr.h"
#include "Container/Vector.h"
#include "Math/Matrix3x4.h"

#include <vector>

namespace FlagGG
{
	namespace Graphics
	{
		struct FlagGG_API RenderContext
		{
			GeometryType geometryType_{ GEOMETRY_STATIC };

			PrimitiveType primitiveType_{ PRIMITIVE_TRIANGLE };

			Container::Vector<Container::SharedPtr<VertexBuffer>>* vertexBuffers_{ nullptr };

			Container::Vector <Container::SharedPtr<IndexBuffer>>* indexBuffers_{ nullptr };

			Container::SharedPtr<Texture> texture_;

			Container::SharedPtr<Shader> VSShader_;

			Container::SharedPtr<Shader> PSShader_;

			const Math::Matrix3x4* worldTransform_{ nullptr };
			uint32_t numWorldTransform_{ 0 };
		};
	}
}

#endif