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

// shader源码
class FlagGG_API ShaderCode : public Resource
{
public:
	ShaderCode(Context* context);

	~ShaderCode() override;

	// 传入shader类型、shader宏，返回shader变种
	Shader* GetShader(ShaderType type, const Vector<String>& defines);

protected:
	bool BeginLoad(IOFrame::Buffer::IOBuffer* stream) override;

	bool EndLoad() override;

	// 预处理shader代码
	bool PreCompileShaderCode(const char* head, const char* tail, String& out);

private:
	Vector<SharedPtr<Shader>> shaders_;

	// shader代码
	SharedArrayPtr<char> buffer_;
	UInt32 bufferSize_{ 0 };
};

class FlagGG_API Shader : public RefCounted
{
public:
	Shader(SharedArrayPtr<char> buffer, UInt32 bufferSize);

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

	// shader代码
	SharedArrayPtr<char> buffer_;
	UInt32 bufferSize_{ 0 };

	ShaderType shaderType_{ None };

	Vector<String> defines_;
	String definesString_;
};

}
