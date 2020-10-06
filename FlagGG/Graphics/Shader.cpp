#include "Graphics/Shader.h"
#include "Graphics/RenderEngine.h"
#include "Graphics/ShaderParameter.h"
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
		static Container::HashMap<Container::String, TextureClass> TEXTURE_CLASS =
		{
			{ "Universal", TEXTURE_CLASS_UNIVERSAL },
			{ "DiffuseMap", TEXTURE_CLASS_DIFFUSE },
			{ "NormalMap", TEXTURE_CLASS_NORMAL },
			{ "SpecularMap", TEXTURE_CLASS_SPECULAR },
			{ "EmissiveMap", TEXTURE_CLASS_EMISSIVE },
			{ "EnvMap", TEXTURE_CLASS_ENVIRONMENT },
			{ "ShadowMap", TEXTURE_CLASS_SHADOWMAP }
		};

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
			Resource(context),
			shaderParser_(context)
		{ }

		Shader* ShaderCode::GetShader(ShaderType type, const Container::String& passName, const Container::Vector<Container::String>& defines)
		{
			const auto& passMap = shaderParser_.GetPassShaderSourceCode();
			auto itPass = passMap.Find(passName);
			if (itPass == passMap.End())
				return nullptr;

			auto& shaders = shadersMap_[passName];
			Container::Vector<Container::String> newDefines = defines;
			newDefines.Push(type == VS ? "VERTEX" : "PIXEL");
			Container::String definesStr = HashVectorString(newDefines);
			for (const auto& shader : shaders)
			{
				if (shader->GetType() == type && shader->GetDefinesString() == definesStr)
				{
					return shader;
				}
			}

			Container::SharedPtr<Shader> shader(new Shader(itPass->second_));
			shader->SetType(type);
			shader->SetDefines(newDefines);
			shader->Initialize();
			shaders.Push(shader);

			return shader;
		}

		bool ShaderCode::BeginLoad(IOFrame::Buffer::IOBuffer* stream)
		{
			Container::String buffer;
			stream->ToString(buffer);

			shaderParser_.Clear();
			bool ret = shaderParser_.Parse(buffer.CString(), buffer.Length());

			return ret;
		}

		bool ShaderCode::EndLoad()
		{
			return true;
		}

		// --------------------------------------------------------------------------------------------------------------

		Shader::Shader(const PassShaderSourceCode& pass) :
			GPUObject(),
			pass_(pass)
		{ }

		Shader::~Shader()
		{
		}

		static bool CompileShader(const char* buffer, UInt32 bufferSize,
			ShaderType shaderType, const Container::Vector<Container::String>& defines,
			Container::String& out)
		{
			Container::Vector<Container::String> args;

			return true;
		}

		void Shader::Initialize()
		{
			if (shaderType_ == None)
			{
				FLAGGG_LOG_WARN("Shader type is None, so initialize failed.");

				return;
			}

			const bgfx::Memory* mem = nullptr;
			if (!CompileShader(pass_.sourceCode_.Buffer(), pass_.sourceCode_.Size(), shaderType_, defines_, compiledSourceCode_))
			{
				FLAGGG_LOG_ERROR("Failed to compile shader.");
				return;
			}

			bgfx::ShaderHandle handle = bgfx::createShader(mem);
			ResetHandler(handle);

			if (bgfx::isValid(handle))
			{
				UInt16 numUniform = bgfx::getShaderUniforms(handle);
				if (numUniform)
				{
					Container::PODVector<bgfx::UniformHandle> uniformHandles((unsigned)numUniform);
					bgfx::getShaderUniforms(handle, &uniformHandles[0], numUniform);
					for (auto& uniformHandle : uniformHandles)
					{
						bgfx::UniformInfo info;
						bgfx::getUniformInfo(uniformHandle, info);

						ShaderParameterDesc desc;
						desc.name_ = &info.name[1];
						desc.handle_ = uniformHandle;
						desc.type_ = info.type;
						desc.num_ = info.num;
						desc.offset_ = 0;
						desc.size_ = 0;

						shaderParameterDescMap[desc.name_] = desc;
					}
				}
			}
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

		const Container::String& Shader::GetDefinesString() const
		{
			return definesString_;
		}

		ShaderType Shader::GetType()
		{
			return shaderType_;
		}


		ShaderProgram::ShaderProgram(Shader* vsShader, Shader* psShader)
		{
			UniformAndSamplers(vsShader);
			UniformAndSamplers(psShader);
		}

		bool ShaderProgram::IsValid()
		{
			return bgfx::isValid(GetSrcHandler<bgfx::ProgramHandle>());
		}

		void ShaderProgram::UniformAndSamplers(Shader* shader)
		{
			for (auto& it : shader->GetShaderParameterDescs())
			{
				const auto& desc = it.second_;
				if (desc.type_ == bgfx::UniformType::Enum::Sampler)
				{
					auto itClass = TEXTURE_CLASS.Find(desc.name_);
					if (itClass != TEXTURE_CLASS.End())
					{
						texSamplers_[itClass->second_] = desc.handle_;
					}
				}
				else
				{
					uniformDescMap_[it.first_] = it.second_;
				}
			}
		}

		const ShaderParameterDesc* ShaderProgram::GetUniformDesc(const Container::String& parameterName)
		{
			auto it = uniformDescMap_.Find(parameterName);
			return it != uniformDescMap_.End() ? &(it->second_) : nullptr;
		}
	}
}
