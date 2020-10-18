#include "Graphics/Shader.h"
#include "Graphics/RenderEngine.h"
#include "Graphics/ShaderParameter.h"
#include "Allocator/SmartMemory.hpp"
#include "Container/ArrayPtr.h"
#include "Container/Sort.h"
#include "IOFrame/Stream/FileStream.h"
#include "Resource/ResourceCache.h"
#include "Core/Context.h"
#include "Utility/SystemHelper.h"
#include "Log.h"
#include "bgfx/bgfx.h"
#include "bgfx/bgfx/tools/shaderc/compile_shader.h"

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

		static void WriteCode(IOFrame::Stream::FileStream& file, const Container::PODVector<char>& code, const Container::String& main)
		{
			const char* index = code.Buffer();
			const char* eof = code.Buffer() + code.Size();
			UInt32 count = 0u;
			while (index != eof)
			{
				++count;
				if (count >= main.Length() && strncmp(index - main.Length() + 1, main.CString(), main.Length()) == 0)
				{
					file.WriteStream(code.Buffer(), index - main.Length() + 1 - code.Buffer());
					file.WriteStream("main", strlen("main"));
					break;
				}
				++index;
			}
			file.WriteStream(index + 1, eof - (index + 1));
		}

		static bool CompileShader(const PassShaderSourceCode& pass, ShaderType shaderType,
			const Container::Vector<Container::String>& defines, const Container::String& definesString, Container::String& out)
		{
			const Container::String shaderFolder = Utility::SystemHelper::GetProgramDir() + "ShaderCache/";
			Utility::SystemHelper::CreateDir(shaderFolder);
			const Container::String name = Utility::SystemHelper::GetFileName(pass.fileName_);
			const Container::String VPS = shaderType == VS ? "VS" : "PS";
			const Container::String shaderPathIn = shaderFolder + name + "_" + VPS + "_" + pass.passName_ + "_" + definesString + "_input.shader";
			const Container::String shaderPathOut = shaderFolder + name + "_" + VPS + "_" + pass.passName_ + "_" + definesString + "_ouput.shader";
			const Container::String varyingFile = shaderFolder + name + "_" + VPS + "_" + pass.passName_ + "_" + definesString + "_varyingdef.shader";
			const Container::String include = Utility::SystemHelper::GetProgramDir() + "Res";

			{
				IOFrame::Stream::FileStream file;
				file.Open(shaderPathIn, IOFrame::Stream::FileMode::FILE_WRITE);

				if (!file.IsOpen())
					return false;

				if (shaderType == VS)
				{
					file.WriteStream(pass.prepareDefines_.CString(), pass.prepareDefines_.Length());
					file.WriteStream("$input ", strlen("$input "));
					for (UInt32 i = 0; i < pass.inputVar_.Size(); ++i)
					{
						if (i != 0)
							file.WriteStream(", ", 2);
						file.WriteStream(pass.inputVar_[i].CString(), pass.inputVar_[i].Length());
					}
					file.WriteStream("\n", 1);

					file.WriteStream("$output ", strlen("$output "));
					for (UInt32 i = 0; i < pass.outputVar_.Size(); ++i)
					{
						if (i != 0)
							file.WriteStream(", ", 2);
						file.WriteStream(pass.outputVar_[i].CString(), pass.outputVar_[i].Length());
					}
					file.WriteStream("\n", 1);
				}
				else
				{
					file.WriteStream("$input ", strlen("$input "));
					for (UInt32 i = 0; i < pass.outputVar_.Size(); ++i)
					{
						if (i != 0)
							file.WriteStream(", ", 2);
						file.WriteStream(pass.outputVar_[i].CString(), pass.outputVar_[i].Length());
					}
					file.WriteStream("\n", 1);
				}

				WriteCode(file, pass.sourceCode_, shaderType == VS ? "VS" : "PS");
				file.Close();
			}

			{
				IOFrame::Stream::FileStream file;
				file.Open(varyingFile, IOFrame::Stream::FileMode::FILE_WRITE);
				if (!file.IsOpen())
					return false;

				file.WriteStream(pass.pixelVaryingDef_.Buffer(), pass.pixelVaryingDef_.Size());
				file.WriteStream("\n", 1);
				file.WriteStream(pass.vertexVaryingDef_.Buffer(), pass.vertexVaryingDef_.Size());
				file.Close();
			}

			Container::Vector<Container::String> argsArray;
			argsArray.Push("-f");
			argsArray.Push(shaderPathIn);

			argsArray.Push("-o");
			argsArray.Push(shaderPathOut);

			argsArray.Push("--depends");

			argsArray.Push("-i");
			argsArray.Push(include);

			argsArray.Push("--varyingdef");
			argsArray.Push(varyingFile);

			argsArray.Push("--platform");
			argsArray.Push("windows");

			// 默认dx11
			argsArray.Push("--profile");
			argsArray.Push(shaderType == VS ? "vs_4_0" : "ps_4_0");

			argsArray.Push("--type");
			argsArray.Push(shaderType == VS ? "vertex" : "fragment");
#if _DEBUG
			argsArray.Push("--debug");
			argsArray.Push("--disasm");
#else
			argsArray.Push("-O");
			argsArray.Push("3");
#endif
			argsArray.Push("--define");
			Container::String defineJoin;
			defineJoin.Join(defines, ";");
			if (shaderType == VS)
				defineJoin.Append(";VERTEX");
			else
				defineJoin.Append(";PIXEL");
			argsArray.Push(defineJoin);

			const char* argv[64];
			for (unsigned i = 0; i < argsArray.Size(); ++i)
				argv[i] = argsArray[i].CString();

			int retCode = bgfx::CommandCompileShader(argsArray.Size(), argv);

			if (retCode == 0)
			{
				IOFrame::Stream::FileStream file;
				file.Open(shaderPathOut, IOFrame::Stream::FileMode::FILE_READ);
				if (!file.IsOpen())
					return false;

				out.Resize(file.GetSize());
				file.ReadStream(&out[0], out.Length());
			}

			return retCode == 0;
		}

		void Shader::Initialize()
		{
			if (shaderType_ == None)
			{
				FLAGGG_LOG_WARN("Shader type is None, so initialize failed.");

				return;
			}

			if (!CompileShader(pass_, shaderType_, defines_, definesString_, compiledSourceCode_))
			{
				FLAGGG_LOG_ERROR("Failed to compile shader.");
				return;
			}

			const bgfx::Memory* mem = bgfx::makeRef(compiledSourceCode_.CString(), compiledSourceCode_.Length());
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
			if (!shader)
				return;

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
