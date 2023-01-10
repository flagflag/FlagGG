#ifndef __SHADER__
#define __SHADER__

#include "Export.h"

#include "Graphics/GPUObject.h"
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

	Shader* GetShader(ShaderType type, const Vector<String>& defines);

protected:
	bool BeginLoad(IOFrame::Buffer::IOBuffer* stream) override;

	bool EndLoad() override;

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

	void SetType(ShaderType type);

	void SetDefines(const Vector<String>& defines);

	void Compile();

	String GetDefinesString() const;

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

#endif