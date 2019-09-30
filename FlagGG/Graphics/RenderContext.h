#ifndef __RENDER_CONTEXT__
#define __RENDER_CONTEXT__

#include "Export.h"

#include "Graphics/Geometry.h"
#include "Graphics/Texture.h"
#include "Graphics/Shader.h"
#include "Graphics/Material.h"
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

			Container::Vector<Container::SharedPtr<Geometry>> geometries_;

			Container::Vector<Container::SharedPtr<Texture>> textures_;

			Container::SharedPtr<Shader> vertexShader_;
			Container::SharedPtr<Shader> pixelShader_;

			const Math::Matrix3x4* worldTransform_{ nullptr };
			uint32_t numWorldTransform_{ 0 };

			Container::HashMap<uint32_t, RenderPass>* renderPass_{ nullptr };

			Container::SharedPtr<ShaderParameters> shaderParameters_{ nullptr };
		};
	}
}

#endif