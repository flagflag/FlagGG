#include "HlslShaderCompile.h"
#include "FileSystem/FileHandle/LocalFileHandle.h"
#include "Utility/SystemHelper.h"
#include "Core/CryAssert.h"
#include "Log.h"

namespace FlagGG
{

bool CompileShader(const char* buffer, USize bufferSize, ShaderType type, const Vector<String>& defines, ID3DBlob*& outCompileCode, ID3DBlob*& outStrippedCode)
{
	const char* entryPoint = nullptr;
	const char* profile = nullptr;

	unsigned flags = /*D3DCOMPILE_ENABLE_STRICTNESS*/0;
	flags |= D3DCOMPILE_ENABLE_BACKWARDS_COMPATIBILITY;
	flags |= D3DCOMPILE_PARTIAL_PRECISION;
	flags |= D3DCOMPILE_AVOID_FLOW_CONTROL; // D3DCOMPILE_PREFER_FLOW_CONTROL
	flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;

#if defined( DEBUG ) || defined( _DEBUG )
	// flags |= D3DCOMPILE_DEBUG;
#endif

	if (type == VS)
	{
		entryPoint = "VS";
		profile = "vs_5_0";
	}
	else if (type == PS)
	{
		entryPoint = "PS";
		profile = "ps_5_0";
	}
	else if (type == CS)
	{
		entryPoint = "CS";
		profile = "cs_5_0";
	}

	ID3DBlob* shaderCode = nullptr;
	ID3DBlob* errorMsgs = nullptr;

	PODVector<D3D_SHADER_MACRO> macros;

	for (UInt32 i = 0; i < defines.Size(); ++i)
	{
		D3D_SHADER_MACRO macro;
		macro.Name = defines[i].CString();
		macro.Definition = "1";
		macros.Push(macro);
	}

	D3D_SHADER_MACRO emptyMacro;
	emptyMacro.Name = nullptr;
	emptyMacro.Definition = nullptr;
	macros.Push(emptyMacro);

	HRESULT hr = D3DCompile(
		buffer,
		bufferSize,
		nullptr,
		&macros[0],
		nullptr,
		entryPoint,
		profile,
		flags,
		0,
		&shaderCode,
		&errorMsgs
	);
	if (FAILED(hr))
	{
		FLAGGG_LOG_ERROR("D3DCompile failed.");

		{
			LocalFileHandle fileHandle;
			fileHandle.Open(GetProgramDir() + "shader_error.txt", FileMode::FILE_WRITE);
			fileHandle.Write(buffer, bufferSize);
			fileHandle.Close();
		}

		if (errorMsgs)
		{
			FLAGGG_LOG_ERROR("Error code: {}", (const char*)errorMsgs->GetBufferPointer());

			ASSERT_MESSAGE(false, (const char*)errorMsgs->GetBufferPointer());

			D3D11_SAFE_RELEASE(errorMsgs);
		}

		return false;
	}

	ID3DBlob* strippedCode = nullptr;
	hr = D3DStripShader(shaderCode->GetBufferPointer(), shaderCode->GetBufferSize(),
		D3DCOMPILER_STRIP_REFLECTION_DATA | D3DCOMPILER_STRIP_DEBUG_INFO | D3DCOMPILER_STRIP_TEST_BLOBS, &strippedCode);
	if (FAILED(hr))
	{
		FLAGGG_LOG_ERROR("D3DStripShader failed.");

		return false;
	}

	D3D11_SAFE_RELEASE(errorMsgs);

	outCompileCode = shaderCode;
	outStrippedCode = strippedCode;

	return true;
}


}
