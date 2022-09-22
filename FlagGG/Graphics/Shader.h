#ifndef __SHADER__
#define __SHADER__

#include "Export.h"

#include "Graphics/GPUObject.h"
#include "Graphics/GraphicsDef.h"
#include "Graphics/ShaderParser.h"
#include "Resource/Resource.h"
#include "Container/ArrayPtr.h"
#include "Container/Vector.h"
#include "Container/Str.h"

namespace FlagGG
{
	namespace Graphics
	{
		class Shader;
		struct ShaderParameterDesc;

		// shader源码
		class FlagGG_API ShaderCode : public Resource::Resource
		{
		public:
			ShaderCode(Core::Context* context);

			Shader* GetShader(ShaderType type, const Container::String& passName, const Container::Vector<Container::String>& defines);

			const Container::HashMap<Container::String, unsigned>& GetSamplerRegisters() const { return samplerRegisters_; }

		protected:
			bool BeginLoad(IOFrame::Buffer::IOBuffer* stream) override;

			bool EndLoad() override;

		private:
			ShaderParser shaderParser_;

			Container::HashMap<Container::String, unsigned> samplerRegisters_;
			Container::HashMap<Container::String, Container::Vector<Container::SharedPtr<Shader>>> shadersMap_;
		};

		// 经过编译的shader，是GPU对象
		class FlagGG_API Shader : public GPUObject, public Container::RefCounted
		{
		public:
			Shader(const PassShaderSourceCode& pass);

			~Shader() override;

			bool IsValid() override;

			void Initialize() override;

			void SetShaderCode(ShaderCode* shaderCode);

			void SetType(ShaderType type);

			void SetDefines(const Container::Vector<Container::String>& defines);

			const Container::String& GetDefinesString() const;

			ShaderCode* GetShaderCode() const { return shaderCode_; }

			ShaderType GetType();

			const Container::HashMap<Container::String, ShaderParameterDesc>& GetShaderParameterDescs() const { return shaderParameterDescMap_; }

		private:
			Container::WeakPtr<ShaderCode> shaderCode_;

			ShaderType shaderType_{ None };

			const PassShaderSourceCode& pass_;

			Container::Vector<Container::String> defines_;
			Container::String definesString_;

			Container::String compiledSourceCode_;

			Container::HashMap<Container::String, ShaderParameterDesc> shaderParameterDescMap_;
		};

		class FlagGG_API ShaderProgram : public GPUObject, public Container::RefCounted
		{
		public:
			explicit ShaderProgram(Shader* vsShader, Shader* psShader);

			bool IsValid() override;

			void Initialize() override {};

			const ShaderParameterDesc* GetUniformDesc(const Container::String& parameterName);

			const Container::HashMap<Container::String, ShaderParameterDesc>& GetUniformDescMap() const { return uniformDescMap_; }

			bgfx::UniformHandle GetTexSamplers(UInt32 index) { return texSamplers_[index]; }
		
		private:
			void UniformAndSamplers(Shader* shader);

		private:
			Container::HashMap<Container::String, ShaderParameterDesc> uniformDescMap_;
			bgfx::UniformHandle texSamplers_[MAX_TEXTURE_CLASS];
		};
	}
}

#endif