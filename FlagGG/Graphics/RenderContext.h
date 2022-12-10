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

struct FlagGG_API RenderContext
{
	GeometryType geometryType_{ GEOMETRY_STATIC };

	Vector<SharedPtr<Geometry>> geometries_;

	Vector<SharedPtr<Texture>> textures_;

	SharedPtr<Shader> vertexShader_;
	SharedPtr<Shader> pixelShader_;

	const Matrix3x4* worldTransform_{ nullptr };
	UInt32 numWorldTransform_{ 0 };

	HashMap<UInt32, RenderPass>* renderPass_{ nullptr };

	SharedPtr<ShaderParameters> shaderParameters_{ nullptr };

	UInt32 viewMask_{ 0u };

	RasterizerState rasterizerState_;
};

}

#endif