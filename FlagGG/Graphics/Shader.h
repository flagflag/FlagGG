//
// 引擎层Shader
//

#pragma once

#include "Export.h"

#include "Graphics/GraphicsDef.h"
#include "Graphics/ShaderParameter.h"
#include "Resource/Resource.h"
#include "Container/ArrayPtr.h"
#include "Container/Vector.h"
#include "Container/Str.h"
#include "GfxDevice/GfxShader.h"

#include <string>

namespace FlagGG
{

class Shader;
class PreProcessShaderInfo;

// shader源码
class FlagGG_API ShaderCode : public Resource
{
	OBJECT_OVERRIDE(ShaderCode, Resource);
public:
	ShaderCode();

	~ShaderCode() override;

	// 传入shader类型、shader宏，返回shader变种
	Shader* GetShader(ShaderType type, const Vector<String>& defines);

protected:
	bool BeginLoad(IOFrame::Buffer::IOBuffer* stream) override;

	bool EndLoad() override;

	SharedPtr<PreProcessShaderInfo> PreCompileShaderCode(const String& shaderSource);

private:
	Vector<SharedPtr<Shader>> shaders_;

	SharedPtr<PreProcessShaderInfo> shaderInfo_;
};

class FlagGG_API Shader : public RefCounted
{
public:
	Shader(PreProcessShaderInfo* shaderInfo);

	~Shader() override;

	// 设置shader类型
	void SetType(ShaderType type);

	// 设置shader宏
	void SetDefines(const Vector<String>& defines);

	// 编译
	void Compile();

	// 获取shader宏字符串
	String GetDefinesString() const;

	// 获取shader类型
	ShaderType GetType();

	// 获取gfx引用
	GfxShader* GetGfxRef() const { return gfxShader_; }

private:
	SharedPtr<GfxShader> gfxShader_;

	SharedPtr<PreProcessShaderInfo> shaderInfo_;

	ShaderType shaderType_{};

	Vector<String> defines_;
	String definesString_;
};

}
