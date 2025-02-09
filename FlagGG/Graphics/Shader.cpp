#include "Graphics/Shader.h"
#include "Graphics/RenderEngine.h"
#include "Allocator/SmartMemory.hpp"
#include "Container/ArrayPtr.h"
#include "Container/Sort.h"
#include "IOFrame/Stream/FileStream.h"
#include "Resource/ResourceCache.h"
#include "FileSystem/FileManager.h"
#include "Core/Context.h"
#include "Core/CryAssert.h"
#include "Core/ObjectFactory.h"
#include "Log.h"
#include "GfxDevice/GfxDevice.h"

#include <fstream>

namespace FlagGG
{

REGISTER_TYPE_FACTORY(ShaderCode);

static String HashVectorString(Vector<String> vecStr)
{
	Sort(vecStr.Begin(), vecStr.End());
	String hash;
	for (const auto& str : vecStr)
	{
		hash += str;
	}
	return hash;
}

ShaderCode::ShaderCode() :
	Resource()
{ }

ShaderCode::~ShaderCode()
{
}

Shader* ShaderCode::GetShader(ShaderType type, const Vector<String>& defines)
{
	Vector<String> newDefines = defines;
	if (type == VS)
		newDefines.Push("VERTEX");
	else if (type == PS)
		newDefines.Push("PIXEL");
	else if (type == CS)
		newDefines.Push("COMPUTE");
	String definesStr = HashVectorString(newDefines);
	for (const auto& shader : shaders_)
	{
		if (shader->GetType() == type && shader->GetDefinesString() == definesStr)
		{
			return shader;
		}
	}
			
	SharedPtr<Shader> shader(new Shader(buffer_, bufferSize_));
	shader->SetType(type);
	shader->SetDefines(newDefines);
	shader->Compile();
	shaders_.Push(shader);

	return shader;
}

bool ShaderCode::PreCompileShaderCode(const char* head, const char* tail, String& out)
{
	while (head < tail)
	{
		if (head + 10 < tail &&
			head[0] == '#' &&
			head[1] == 'i' &&
			head[2] == 'n' &&
			head[3] == 'c' &&
			head[4] == 'l' &&
			head[5] == 'u' &&
			head[6] == 'd' &&
			head[7] == 'e')
		{
			head += 8;
			while (head < tail && (*head == ' ' || *head == '\t')) ++head;
			if (*head == '\"')
			{
				++head;
				const char* temp = head;
				while (temp < tail && *temp != '\"') ++temp;

				String relativePath(head, temp - head);

				if (*temp != '\"')
				{
					FLAGGG_LOG_ERROR("Pre compile shader error: missing '\"' behind #include\"{}.", relativePath.CString());
					return false;
				}

				head = temp + 1;

				while (head < tail && (*head == ' ' || *head == '\t')) ++head;

				// 这个判断不严谨，懒得写了，将就用着。
				if (head < tail && *head != '\r' && *head != '\n')
				{
					FLAGGG_LOG_ERROR("Pre compile shader error: missing wrap behind #include\"{}\".", relativePath.CString());
					return false;
				}

				UInt32 bufferSize = 0u;
				SharedArrayPtr<char> buffer;

				{
					auto file = GetSubsystem<AssetFileManager>()->OpenFileReader(relativePath);
					if (!file)
					{
						FLAGGG_LOG_ERROR("Pre compile shader error: can not open file[{}].", relativePath.CString());
						return false;
					}


					file->ToBuffer(buffer, bufferSize);
				}

				if (!PreCompileShaderCode(buffer.Get(), buffer.Get() + bufferSize, out))
					return false;
			}
			else
			{
				FLAGGG_LOG_ERROR("Pre compile shader error: missing filename behind #include.");
				return false;
			}
		}
		else
		{
			out += *head;
			++head;
		}
	}

	return true;
}

static String PreCompileShaderCache;

bool ShaderCode::BeginLoad(IOFrame::Buffer::IOBuffer* stream)
{
	String buffer;
	stream->ToString(buffer);

	if (PreCompileShaderCache.Capacity() == 0)
		PreCompileShaderCache.Reserve(1024 * 1024 * 5); // 5MB
	PreCompileShaderCache.Clear();
	if (!PreCompileShaderCode(buffer.CString(), buffer.CString() + buffer.Length(), PreCompileShaderCache))
		return false;

	bufferSize_ = PreCompileShaderCache.Length();
	buffer_ = new char[bufferSize_ + 1];
	buffer_[bufferSize_] = '\0';
	memcpy(buffer_.Get(), PreCompileShaderCache.CString(), bufferSize_);

	return true;
}

bool ShaderCode::EndLoad()
{
	return true;
}

// --------------------------------------------------------------------------------------------------------------

Shader::Shader(SharedArrayPtr<char> buffer, UInt32 bufferSize) :
	buffer_(buffer),
	bufferSize_(bufferSize)
{
	gfxShader_ = GfxDevice::GetDevice()->CreateShader();
}

Shader::~Shader()
{

}

void Shader::Compile()
{
	gfxShader_->SetShaderType(shaderType_);
	gfxShader_->SetDefines(defines_);
	gfxShader_->SetShaderSource(buffer_, bufferSize_);
	gfxShader_->Compile();
}

void Shader::SetType(ShaderType type)
{
	shaderType_ = type;
}

void Shader::SetDefines(const Vector<String> &defines)
{
	defines_ = defines;

	definesString_ = HashVectorString(defines_);
}

String Shader::GetDefinesString() const
{
	return definesString_;
}

ShaderType Shader::GetType()
{
	return shaderType_;
}

}
