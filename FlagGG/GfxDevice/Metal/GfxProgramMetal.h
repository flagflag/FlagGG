//
// Metal图形层GPU程序
//

#pragma once

#include "GfxDevice/GfxProgram.h"
#include "Container/Vector.h"
#include "Container/HashMap.h"
#include "mtlpp/mtlpp.hpp"

namespace FlagGG
{

struct MetalConstantBufferVariableDesc;

typedef void(*SetUniformToBuffer)(const MetalConstantBufferVariableDesc& desc, const void* uniformValue, void* targetData);

struct MetalConstantBufferVariableDesc
{
	String name_;
	UInt32 offset_;
	mtlpp::DataType dataType_; // 例如：移动端数据类型是HalfFloat，则需要做转换（CPU传过来的数据一定是float，需要转half）
	SetUniformToBuffer setterFunc_;
};

struct MetalConstanceBufferDesc
{
	String name_;
	UInt32 size_;
	Vector<MetalConstantBufferVariableDesc> variableDescs_;
};

struct MetalShaderTextureDesc
{
	String textureName_;
	String samplerName_;
};

struct MetalVertexInputDesc
{

};

class GfxProgramMetal : public GfxProgram
{
	OBJECT_OVERRIDE(GfxProgramMetal, GfxProgram);
public:
	explicit GfxProgramMetal();

	~GfxProgramMetal() override;

	// link vs ps
	void Link(GfxShader* vertexShader, GfxShader* pixelShader) override;

	// link cs
	void LinkComputeShader(GfxShader* computeShader) override;

	// 如果没分析过，则分析一次反射
	void ProcessReflectIfNeed(mtlpp::RenderPipelineReflection* reflect);

	// 获取ConstantBuffer描述
	const HashMap<UInt32, MetalConstanceBufferDesc>& GetContantBufferVariableDesc(ShaderType type) const { return constantBufferDescs_[type]; }

	// 获取纹理描述
	const HashMap<UInt32, MetalShaderTextureDesc>& GetTextureDesc(ShaderType type) const { return textureDescs_[type]; }

	// 获取顶点输入描述
	const HashMap<StringHash, MetalVertexInputDesc>& GetVertexInputDescs() const { return vertexInputDescs_; }

protected:
	void ProcessReflect(mtlpp::RenderPipelineReflection* reflect);

	void ProcessReflect(const ns::Array<mtlpp::Argument>& arguments, ShaderType type);

	void ApplyUniformSetter(MetalConstantBufferVariableDesc& varDesc);

private:
	//
	bool needProcessReflect_;
	// 常量buffer描述
	HashMap<UInt32, MetalConstanceBufferDesc> constantBufferDescs_[3];
	// 纹理描述
	HashMap<UInt32, MetalShaderTextureDesc> textureDescs_[3];
	// Key: ElementSemName hash, Value: { location, variableName }
	HashMap<StringHash, MetalVertexInputDesc> vertexInputDescs_;
};

}
