#pragma once

#include "GfxDevice/D3D11/GfxD3D11Defines.h"
#include "Core/BaseTypes.h"
#include "Graphics/GraphicsDef.h"
#include "Container/Vector.h"

#include <d3dcompiler.h>

namespace FlagGG
{

extern bool CompileShader(const char* buffer, USize bufferSize, ShaderType type, const Vector<String>& defines, ID3DBlob*& outCompileCode, ID3DBlob*& outStrippedCode);

}
