#pragma once

#include "Graphics/Shader.h"
#include "Resource/ResourceCache.h"

#define INIT_SHADER_VARIATION(gfxShader, shaderPath, shaderType, defines) \
	{ \
		auto* shaderCode = GetSubsystem<ResourceCache>()->GetResource<ShaderCode>(shaderPath); \
		if (shaderCode) \
		{ \
			gfxShader = shaderCode->GetShader(shaderType, defines); \
		} \
	}

