#pragma once

#include "GfxDevice/D3D11/GfxD3D11Defines.h"
#include "Core/BaseTypes.h"
#include "Core/Subsystem.h"
#include "Graphics/GraphicsDef.h"
#include "Container/Vector.h"

#include <d3dcompiler.h>

namespace FlagGG
{

class D3DCompiler : public Subsystem<D3DCompiler>
{
public:
	D3DCompiler();

	typedef HRESULT(WINAPI* PFN_D3D_COMPILE)(_In_reads_bytes_(SrcDataSize) LPCVOID pSrcData
		, _In_ SIZE_T SrcDataSize
		, _In_opt_ LPCSTR pSourceName
		, _In_reads_opt_(_Inexpressible_(pDefines->Name != NULL)) CONST D3D_SHADER_MACRO* pDefines
		, _In_opt_ ID3DInclude* pInclude
		, _In_opt_ LPCSTR pEntrypoint
		, _In_ LPCSTR pTarget
		, _In_ UINT Flags1
		, _In_ UINT Flags2
		, _Out_ ID3DBlob** ppCode
		, _Always_(_Outptr_opt_result_maybenull_) ID3DBlob** ppErrorMsgs
		);

	typedef HRESULT(WINAPI* PFN_D3D_DISASSEMBLE)(_In_reads_bytes_(SrcDataSize) LPCVOID pSrcData
		, _In_ SIZE_T SrcDataSize
		, _In_ UINT Flags
		, _In_opt_ LPCSTR szComments
		, _Out_ ID3DBlob** ppDisassembly
		);

	typedef HRESULT(WINAPI* PFN_D3D_REFLECT)(_In_reads_bytes_(SrcDataSize) LPCVOID pSrcData
		, _In_ SIZE_T SrcDataSize
		, _In_ REFIID pInterface
		, _Out_ void** ppReflector
		);

	typedef HRESULT(WINAPI* PFN_D3D_STRIP_SHADER)(_In_reads_bytes_(BytecodeLength) LPCVOID pShaderBytecode
		, _In_ SIZE_T BytecodeLength
		, _In_ UINT uStripFlags
		, _Out_ ID3DBlob** ppStrippedBlob
		);

	PFN_D3D_COMPILE      D3DCompile;
	PFN_D3D_DISASSEMBLE  D3DDisassemble;
	PFN_D3D_REFLECT      D3DReflect;
	PFN_D3D_STRIP_SHADER D3DStripShader;
	GUID IID_ID3D11ShaderReflection;
};

extern bool CompileShader(const char* buffer, USize bufferSize, ShaderType type, const Vector<String>& defines, ID3DBlob*& outCompileCode, ID3DBlob*& outStrippedCode);

}
