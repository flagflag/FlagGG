//
// D3D11图形层Shader
//

#pragma once

#include "GfxDevice/GfxShader.h"
#include "Container/Vector.h"
#include "Container/HashMap.h"

#include <d3d11.h>

namespace FlagGG
{

struct D3D11ConstantBufferVariableDesc
{
	String name_;
	UInt32 offset_;
	UInt32 size_;
};

struct D3D11ConstantBufferDesc
{
	String name_;
	UInt32 size_;
	Vector<D3D11ConstantBufferVariableDesc> variableDescs_;
};

struct D3D11StructBufferDesc
{
	String name_;
	bool readonly_;
};

struct D3D11ShaderTextureDesc
{
	String textureName_;
	String samplerName_;
};

class GfxShaderD3D11 : public GfxShader
{
	OBJECT_OVERRIDE(GfxShaderD3D11, GfxShader);
public:
	explicit GfxShaderD3D11();

	~GfxShaderD3D11() override;

	// 编译
	bool Compile() override;

	// 获取ConstantBuffer描述
	const HashMap<UInt32, D3D11ConstantBufferDesc>& GetContantBufferVariableDesc() const { return constantBufferDescs_; }

	// 获取StructBuffer描述
	const HashMap<UInt32, D3D11StructBufferDesc>& GetStructBufferVariableDesc() const { return structBufferDescs_; }

	// 获取纹理描述
	const HashMap<UInt32, D3D11ShaderTextureDesc>& GetTextureDesc() const { return textureDescs_; }


	// 
	ID3D11VertexShader* GetD3D11VertexShader() const { return vertexShader_; }

	//
	ID3D11PixelShader* GetD3D11PixelShader() const { return pixelShader_; }

	//
	ID3D11ComputeShader* GetD3D11ComputeShader() const { return computeShader_; }

	//
	ID3DBlob* GetByteCode() const { return shaderCode_; }

protected:
	void AnalysisReflection(ID3DBlob* compileCode);

private:
	ID3DBlob* shaderCode_{};
	ID3D11VertexShader* vertexShader_{};
	ID3D11PixelShader* pixelShader_{};
	ID3D11ComputeShader* computeShader_{};

	HashMap<UInt32, D3D11ConstantBufferDesc> constantBufferDescs_;
	HashMap<UInt32, D3D11StructBufferDesc> structBufferDescs_;
	HashMap<UInt32, D3D11ShaderTextureDesc> textureDescs_;
};

}
