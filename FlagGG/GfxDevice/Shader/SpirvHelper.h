#pragma once

#include "Core/Subsystem.h"
#include "Container/Str.h"
#include "Graphics/GraphicsDef.h"

namespace FlagGG
{

class SpirvHelper final : public Subsystem<SpirvHelper>
{
public:
	SpirvHelper();

	~SpirvHelper();

	bool GlslToSpirv(ShaderType type, const String& glslShaderCode, String& spirvShaderCode);
};

}
