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

struct OGLShaderUniformVariableDesc
{
	String name_;
	GLenum type_;
	GLint location_;
	UInt32 vectorSize_;
	UInt32 arraySize_;
};

struct OGLShaderTextureDesc
{
	String name_;
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

private:
	GLuint oglProgram_{};

	Vector<OGLShaderUniformVariableDesc> uniformVariableDescs_;
	HashMap<UInt32, OGLShaderTextureDesc> textureDescs_;
};

}
