#include "Graphics/Shader.h"
#include "Graphics/RenderEngine.h"
#include "Allocator/SmartMemory.hpp"
#include "Container/ArrayPtr.h"
#include "Container/Sort.h"
#include "IOFrame/Stream/FileStream.h"
#include "Resource/ResourceCache.h"
#include "Core/Context.h"
#include "Log.h"
#include "bgfx/bgfx.h"

namespace FlagGG
{
	namespace Graphics
	{
		static Container::String HashVectorString(Container::Vector<Container::String> vecStr)
		{
			Container::Sort(vecStr.Begin(), vecStr.End());
			Container::String hash;
			for (const auto& str : vecStr)
			{
				hash += str;
			}
			return hash;
		}

		ShaderCode::ShaderCode(Core::Context* context) :
			Resource(context)
		{ }

		Shader* ShaderCode::GetShader(ShaderType type, const Container::Vector<Container::String>& defines)
		{
			Container::Vector<Container::String> newDefines = defines;
			newDefines.Push(type == VS ? "VERTEX" : "PIXEL");
			Container::String definesStr = HashVectorString(newDefines);
			for (const auto& shader : shaders_)
			{
				if (shader->GetType() == type && shader->GetDefinesString() == definesStr)
				{
					return shader;
				}
			}

			Container::SharedPtr<Shader> shader(new Shader(buffer_, bufferSize_));
			shader->SetType(type);
			shader->SetDefines(newDefines);
			shader->Initialize();
			shaders_.Push(shader);

			return shader;
		}

		bool ShaderCode::PreCompileShaderCode(const char* head, const char* tail, Container::String& out)
		{
			while (head != tail)
			{
				while (head != tail && (*head == '\n' || *head == '\r' || *head == ' ' || *head == '\t')) ++head;
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
					while (head != tail && (*head == ' ' || *head == '\t')) ++head;
					if (*head == '\"')
					{
						++head;
						const char* temp = head;
						while (temp != tail && *temp != '\"') ++temp;

						Container::String relativePath(head, temp - head);

						if (*temp != '\"')
						{
							FLAGGG_LOG_ERROR("Pre compile shader error: missing '\"' behind #include\"{}.", relativePath.CString());
							return false;
						}

						head = temp + 1;

						while (head != tail && (*head == ' ' || *head == '\t')) ++head;

						// 这个判断不严谨，懒得写了，将就用着。
						if (head != tail && *head != '\r' && *head != '\n')
						{
							FLAGGG_LOG_ERROR("Pre compile shader error: missing wrap behind #include\"{}\".", relativePath.CString());
							return false;
						}

						auto file = context_->GetVariable<FlagGG::Resource::ResourceCache>("ResourceCache")->GetFile(relativePath);
						if (!file)
						{
							FLAGGG_LOG_ERROR("Pre compile shader error: can not open file[{}].", relativePath.CString());
							return false;
						}

						UInt32 bufferSize = 0u;
						Container::SharedArrayPtr<char> buffer;
						file->ToBuffer(buffer, bufferSize);
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
					break;
				}
			}

			out.Append(head, tail - head);
		}

		bool ShaderCode::BeginLoad(IOFrame::Buffer::IOBuffer* stream)
		{
			Container::String buffer;
			stream->ToString(buffer);

			Container::String out;
			PreCompileShaderCode(buffer.CString(), buffer.CString() + buffer.Length(), out);

			bufferSize_ = out.Length();
			buffer_ = new char[bufferSize_];
			memcpy(buffer_.Get(), out.CString(), bufferSize_);

			return true;
		}

		bool ShaderCode::EndLoad()
		{
			return true;
		}

		// --------------------------------------------------------------------------------------------------------------

		Shader::Shader(Container::SharedArrayPtr<char> buffer, UInt32 bufferSize) :
			GPUObject(),
			buffer_(buffer),
			bufferSize_(bufferSize)
		{ }

		Shader::~Shader()
		{
		}

		static bool CompileShader(Container::SharedArrayPtr<char> buffer, UInt32 bufferSize,
			ShaderType shaderType, const Container::Vector<Container::String>& defines,
			Container::SharedArrayPtr<char>& byteCode, UInt32& byteCodeSize)
		{
#ifdef COMPILE_SHADER

#else
			byteCode = buffer;
			byteCodeSize = bufferSize;
#endif
			return true;
		}

		void Shader::Initialize()
		{
			if (shaderType_ == None)
			{
				FLAGGG_LOG_WARN("Shader type is None, so initialize failed.");

				return;
			}

			Container::SharedArrayPtr<char> byteCode;
			UInt32 byteCodeSize = 0u;
			if (!CompileShader(buffer_, bufferSize_, shaderType_, defines_, byteCode, byteCodeSize))
			{
				FLAGGG_LOG_ERROR("Failed to compile shader.");
				return;
			}

			const bgfx::Memory* mem = bgfx::makeRef(byteCode.Get(), byteCodeSize);
			bgfx::ShaderHandle handle = bgfx::createShader(mem);
			ResetHandler(handle);
		}

		bool Shader::IsValid()
		{
			return GetHandler();
		}

		void Shader::SetType(ShaderType type)
		{
			shaderType_ = type;
		}

		void Shader::SetDefines(const Container::Vector<Container::String> &defines)
		{
			defines_ = defines;

			definesString_ = HashVectorString(defines_);
		}

		Container::String Shader::GetDefinesString() const
		{
			return definesString_;
		}

		ShaderType Shader::GetType()
		{
			return shaderType_;
		}
	}
}
