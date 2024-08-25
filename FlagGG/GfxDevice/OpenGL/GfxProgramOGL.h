//
// OpenGL图形层GPU程序
//

#pragma once

#include "GfxDevice/GfxProgram.h"
#include "GfxDevice/OpenGL/OpenGLInterface.h"
#include "Container/Vector.h"
#include "Container/HashMap.h"

namespace FlagGG
{

class OGLShaderUniformVariableDesc;

typedef void(*UniformFunc)(const OGLShaderUniformVariableDesc& desc, const void* data);

struct OGLShaderUniformVariableDesc
{
	String name_;
	GLenum type_;
	GLint location_;
	UInt32 arraySize_;
	UInt32 dataSize_;
	UniformFunc uniformFunc_;
};

struct OGLShaderTextureDesc
{
	String name_;
};

struct OGLVertexInputDesc
{
	GLint location_;
	String variableName_;
};

class GfxProgramOpenGL : public GfxProgram
{
	OBJECT_OVERRIDE(GfxProgramOpenGL, GfxProgram);
public:
	explicit GfxProgramOpenGL();

	~GfxProgramOpenGL() override;

	// link vs ps
	void Link(GfxShader* vertexShader, GfxShader* pixelShader) override;

	// link cs
	void LinkComputeShader(GfxShader* computeShader) override;


	// 
	GLuint GetOGLProgram() const { return oglProgram_; }

	// 获取Uniform变量描述
	const Vector<OGLShaderUniformVariableDesc>& GetUniformVariableDescs() const { return uniformVariableDescs_; }

	// 获取纹理描述
	const HashMap<UInt32, OGLShaderTextureDesc>& GetTextureDescs() const { return textureDescs_; }

	// 获取顶点输入描述
	const HashMap<StringHash, OGLVertexInputDesc>& GetVertexInputDescs() const { return vertexInputDescs_; }

protected:
	void ProcessReflect();

private:
	GLuint oglProgram_{};

	Vector<OGLShaderUniformVariableDesc> uniformVariableDescs_;
	HashMap<UInt32, OGLShaderTextureDesc> textureDescs_;
	// Key: ElementSemName hash, Value: { location, variableName }
	HashMap<StringHash, OGLVertexInputDesc> vertexInputDescs_;
};

}
