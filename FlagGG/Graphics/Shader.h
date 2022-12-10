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

struct ConstantBufferVariableDesc
{
	String name_;
	UInt32 offset_;
	UInt32 size_;
};

struct ConstantBufferDesc
{
	String name_;
	UInt32 size_;
	Vector<ConstantBufferVariableDesc> variableDescs_;
};

struct TextureDesc
{
	String textureName_;
	String samplerName_;
};

// 经过编译的shader，是GPU对象
class FlagGG_API Shader : public GPUObject, public RefCounted
{
public:
	Shader(SharedArrayPtr<char> buffer, UInt32 bufferSize);

	~Shader() override;

	bool IsValid() override;

	void Initialize() override;

	void SetType(ShaderType type);

	void SetDefines(const Vector<String>& defines);

	String GetDefinesString() const;

	ShaderType GetType();

	ID3DBlob* GetByteCode();

	const HashMap<UInt32, ConstantBufferDesc>& GetContantBufferVariableDesc() const;

	const HashMap<UInt32, TextureDesc>& GetTextureDesc() const;

protected:
	void AnalysisReflection(ID3DBlob* compileCode);

private:
	// shader代码
	SharedArrayPtr<char> buffer_;
	UInt32 bufferSize_{ 0 };

	ID3DBlob* shaderCode_{ nullptr };

	ShaderType shaderType_{ None };

	Vector<String> defines_;
	String definesString_;

	HashMap<UInt32, ConstantBufferDesc> constantBufferDescs_;
	HashMap<UInt32, TextureDesc> textureDescs_;
};

}

#endif