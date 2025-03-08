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
#include "GfxDevice/Shader/PreProcessShaderInfo.h"

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
	{
		newDefines.Push("VERTEX");
		if (GfxDevice::GetDevice()->IsInstanceSupported())
			newDefines.Push("INSTANCE");
	}
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
			
	SharedPtr<Shader> shader(new Shader(shaderInfo_));
	shader->SetType(type);
	shader->SetDefines(newDefines);
	shader->Compile();
	shaders_.Push(shader);

	return shader;
}

static bool PreCompileShaderCode(const char* head, const char* tail, String& out, PreProcessShaderInfo& shaderInfo, UInt32 fileIndex)
{
	UInt32 fileLineNumber = 0;

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

				String shaderSource;

				{
					auto file = GetSubsystem<AssetFileManager>()->OpenFileReader(relativePath);
					if (!file)
					{
						FLAGGG_LOG_ERROR("Pre compile shader error: can not open file[{}].", relativePath.CString());
						return false;
					}

					file->ToString(shaderSource);
				}

				UInt32 nextFileIndex = shaderInfo.fileInfos_.Size();
				shaderInfo.fileInfos_.Push(PreProcessFileInfo{ relativePath });

				if (!PreCompileShaderCode(shaderSource.CString(), shaderSource.CString() + shaderSource.Length(), out, shaderInfo, nextFileIndex))
					return false;

				shaderInfo.fileInfos_[nextFileIndex].shaderSource_ = std::move(shaderSource);
			}
			else
			{
				FLAGGG_LOG_ERROR("Pre compile shader error: missing filename behind #include.");
				return false;
			}
		}
		else
		{
			if (*head == '\n')
			{
				++fileLineNumber;
				shaderInfo.lineInfos_.Push(PreProcessLineInfo{ fileIndex, fileLineNumber });
			}

			out += *head;
			++head;
		}
	}

	return true;
}

SharedPtr<PreProcessShaderInfo> ShaderCode::PreCompileShaderCode(const String& shaderSource)
{
	static String PreCompileShaderCache;
	if (PreCompileShaderCache.Capacity() == 0)
		PreCompileShaderCache.Reserve(1024 * 1024 * 5); // 5MB

	PreCompileShaderCache.Clear();

	SharedPtr<PreProcessShaderInfo> shaderInfo(new PreProcessShaderInfo);

	shaderInfo->fileInfos_.Push(PreProcessFileInfo{ GetName(), shaderSource });
	shaderInfo->lineInfos_.Push(PreProcessLineInfo{ 0, 0 });

	if (!FlagGG::PreCompileShaderCode(shaderSource.CString(), shaderSource.CString() + shaderSource.Length(), PreCompileShaderCache, *shaderInfo, 0))
		return nullptr;

	shaderInfo->bufferSize_ = PreCompileShaderCache.Length();
	shaderInfo->buffer_ = new char[shaderInfo->bufferSize_ + 1];
	shaderInfo->buffer_[shaderInfo->bufferSize_] = '\0';
	Memory::Memcpy(shaderInfo->buffer_.Get(), PreCompileShaderCache.CString(), shaderInfo->bufferSize_);

	return shaderInfo;
}

bool ShaderCode::BeginLoad(IOFrame::Buffer::IOBuffer* stream)
{
	String shaderSource;
	stream->ToString(shaderSource);

	shaderInfo_ = PreCompileShaderCode(shaderSource);

	return shaderInfo_;
}

bool ShaderCode::EndLoad()
{
	return true;
}

// --------------------------------------------------------------------------------------------------------------

Shader::Shader(PreProcessShaderInfo* shaderInfo)
	: shaderInfo_(shaderInfo)
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
	gfxShader_->SetShaderInfo(shaderInfo_);
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
