#include "HlslShaderCompile.h"
#include "FileSystem/FileHandle/LocalFileHandle.h"
#include "FileSystem/FileSystem.h"
#include "Utility/SystemHelper.h"
#include "Core/CryAssert.h"
#include "Log.h"

namespace FlagGG
{

D3DCompiler::D3DCompiler()
{
	D3DCompile = NULL;
	D3DDisassemble = NULL;
	D3DReflect = NULL;
	D3DStripShader = NULL;
	IID_ID3D11ShaderReflection = {};

	struct D3DCompilerDll
	{
		const char* fileName;
		const GUID  IID_ID3D11ShaderReflection;
	};

	static const D3DCompilerDll d3dcompilerDlls[] =
	{
		{ "d3dcompiler_47.dll", { 0x8d536ca1, 0x0cca, 0x4956, { 0xa8, 0x37, 0x78, 0x69, 0x63, 0x75, 0x55, 0x84 } } },
		{ "d3dcompiler_46.dll", { 0x0a233719, 0x3960, 0x4578, { 0x9d, 0x7c, 0x20, 0x3b, 0x8b, 0x1d, 0x9c, 0xc1 } } },
		{ "d3dcompiler_45.dll", { 0x0a233719, 0x3960, 0x4578, { 0x9d, 0x7c, 0x20, 0x3b, 0x8b, 0x1d, 0x9c, 0xc1 } } },
		{ "d3dcompiler_44.dll", { 0x0a233719, 0x3960, 0x4578, { 0x9d, 0x7c, 0x20, 0x3b, 0x8b, 0x1d, 0x9c, 0xc1 } } },
		{ "d3dcompiler_43.dll", { 0x0a233719, 0x3960, 0x4578, { 0x9d, 0x7c, 0x20, 0x3b, 0x8b, 0x1d, 0x9c, 0xc1 } } },
	};

	for (auto& it : d3dcompilerDlls)
	{
		HMODULE d3dLib = ::LoadLibraryA(it.fileName);
		if (d3dLib)
		{
			D3DCompile     = (PFN_D3D_COMPILE)::GetProcAddress(d3dLib, "D3DCompile");
			D3DDisassemble = (PFN_D3D_DISASSEMBLE)::GetProcAddress(d3dLib, "D3DDisassemble");
			D3DReflect     = (PFN_D3D_REFLECT)::GetProcAddress(d3dLib, "D3DReflect");
			D3DStripShader = (PFN_D3D_STRIP_SHADER)::GetProcAddress(d3dLib, "D3DStripShader");

			if (D3DCompile == NULL || D3DDisassemble == NULL || D3DReflect == NULL || D3DStripShader == NULL)
			{
				::FreeLibrary(d3dLib);
			}
			else
			{
				IID_ID3D11ShaderReflection = it.IID_ID3D11ShaderReflection;
				break;
			}
		}
	}
}

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

	Vector<String> tempDefines;
	tempDefines.Reserve(defines.Size() * 2);

	PODVector<D3D_SHADER_MACRO> macros;

	for (UInt32 i = 0; i < defines.Size(); ++i)
	{
		String defineStr = defines[i].Trimmed();
		D3D_SHADER_MACRO macro;
		auto findPos = defineStr.Find("=");
		if (findPos == String::NPOS)
		{
			tempDefines.Push(std::move(defineStr));
			macro.Name = tempDefines.Back().CString();
			macro.Definition = "1";
		}
		else
		{
			tempDefines.Push(defineStr.Substring(0, findPos));
			macro.Name = tempDefines.Back().CString();
			tempDefines.Push(defineStr.Substring(findPos + 1));
			macro.Definition = tempDefines.Back().CString();
		}
		macros.Push(macro);
	}

	D3D_SHADER_MACRO emptyMacro;
	emptyMacro.Name = nullptr;
	emptyMacro.Definition = nullptr;
	macros.Push(emptyMacro);

	HRESULT hr = GetSubsystem<D3DCompiler>()->D3DCompile(
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
			const String shaderCacheDir = GetProgramDir() + "ShaderCache/";
			if (!GetLocalFileSystem()->DirectoryExists(shaderCacheDir))
				GetLocalFileSystem()->CreateDirectory(shaderCacheDir);
			LocalFileHandle fileHandle;
			fileHandle.Open(shaderCacheDir + "shader_error_" + GetTimeStamp() + ".txt", FileMode::FILE_WRITE);
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
	hr = GetSubsystem<D3DCompiler>()->D3DStripShader(shaderCode->GetBufferPointer(), shaderCode->GetBufferSize(),
		D3DCOMPILER_STRIP_REFLECTION_DATA/* | D3DCOMPILER_STRIP_DEBUG_INFO*/ | D3DCOMPILER_STRIP_TEST_BLOBS, &strippedCode);
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
