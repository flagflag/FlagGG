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
	namespace Graphics
	{
		class Shader;

		// shader源码
		class FlagGG_API ShaderCode : public Resource::Resource
		{
		public:
			ShaderCode(Core::Context* context);

			Shader* GetShader(ShaderType type, const Container::Vector<Container::String>& defines);

		protected:
			bool BeginLoad(IOFrame::Buffer::IOBuffer* stream) override;

			bool EndLoad() override;

		private:
			Container::Vector<Container::SharedPtr<Shader>> shaders_;

			// shader代码
			Container::SharedArrayPtr<char> buffer_;
			uint32_t bufferSize_{ 0 };
		};

		struct ConstantBufferVariableDesc
		{
			Container::String name_;
			uint32_t offset_;
			uint32_t size_;
		};

		struct ConstantBufferDesc
		{
			Container::String name_;
			uint32_t size_;
			Container::Vector<ConstantBufferVariableDesc> variableDescs_;
		};

		struct TextureDesc
		{
			Container::String textureName_;
			Container::String samplerName_;
		};

		// 经过编译的shader，是GPU对象
		class FlagGG_API Shader : public GPUObject, public Container::RefCounted
		{
		public:
			Shader(Container::SharedArrayPtr<char> buffer, uint32_t bufferSize);

			~Shader() override;

			bool IsValid() override;

			void Initialize() override;

			void SetType(ShaderType type);

			void SetDefines(const Container::Vector<Container::String>& defines);

			Container::String GetDefinesString() const;

			ShaderType GetType();

			ID3DBlob* GetByteCode();

			const Container::HashMap<uint32_t, ConstantBufferDesc>& GetContantBufferVariableDesc() const;

			const Container::HashMap<uint32_t, TextureDesc>& GetTextureDesc() const;

		protected:
			void AnalysisReflection(ID3DBlob* compileCode);

		private:
			// shader代码
			Container::SharedArrayPtr<char> buffer_;
			uint32_t bufferSize_{ 0 };

			ID3DBlob* shaderCode_{ nullptr };

			ShaderType shaderType_{ None };

			Container::Vector<Container::String> defines_;
			Container::String definesString_;

			Container::HashMap<uint32_t, ConstantBufferDesc> constantBufferDescs_;
			Container::HashMap<uint32_t, TextureDesc> textureDescs_;
		};
	}
}

#endif