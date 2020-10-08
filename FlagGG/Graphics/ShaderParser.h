#pragma once

#include "Core/BaseTypes.h"
#include "Core/Context.h"
#include "Container/Str.h"
#include "Container/Vector.h"
#include "Container/HashMap.h"

namespace FlagGG
{
	namespace Graphics
	{
		/// 编辑器使用的属性，用于可视化调shader参数
		struct ShaderProperty
		{
			Container::String name_;
			Container::String desc_;
			Container::String type_;
			Container::String defaultValue_;
		};

		struct ShaderTags
		{
			Container::String renderType_;
		};

		struct PassShaderSourceCode
		{
			Container::String passName_;
			Container::PODVector<char> vertexVaryingDef_;
			Container::PODVector<char> pixelVaryingDef_;
			Container::PODVector<char> sourceCode_;
			Container::Vector<Container::String> inputVar_;
			Container::Vector<Container::String> outputVar_;
		};

		class ShaderParser
		{
		public:
			ShaderParser(Core::Context* context);

			/// 解析自定义的shader格式，变成bgfx的shader格式
			/// 自定义shader格式和Unity3D的shader格式一致
			bool Parse(const char* buffer, UInt32 bufferSize);

			void Clear();

			const Container::Vector<ShaderProperty>& GetShaderProperties() const { return properties_; }

			const ShaderTags& GetShaderTags() const { return shaderTags_; }

			const Container::HashMap<Container::String, PassShaderSourceCode>& GetPassShaderSourceCode() const { return passShaderSourceCodeMap_; }

		protected:
			bool PreCompileShaderCode(const char* head, const char* tail, Container::String& out);

			bool ParseShader();

			bool ParseProperties();

			bool ParseSubShader();

			bool ParsePass();

			bool ParseTags();

			bool ParseVertex(Container::PODVector<char>& varyingDef, Container::Vector<Container::String>& varName);

			bool ParsePixel(Container::PODVector<char>& varyingDef, Container::Vector<Container::String>& varName);

			bool ParseSourceCode(Container::PODVector<char>& sourceCode);


			bool IsEof();

			void Foward();

			void Back();

			bool IsEmptyChar();
			
			bool IsString(const Container::String& token);


			void AcceptEmptyChar();

			char AcceptChar();

			Container::String AcceptToken();

			Container::String AcceptKey();

			bool AcceptToken(char ch, Container::String& token);

			bool AcceptLeft();

			bool AcceptRight();

			bool AcceptEqualSign();

			bool AcceptLine(Container::String& token);

			bool AcceptProperty(ShaderProperty& property);

		private:
			Core::Context* context_;

			const char* index_;
			const char* eof_;

			Container::Vector<ShaderProperty> properties_;
			ShaderTags shaderTags_;
			Container::HashMap<Container::String, PassShaderSourceCode> passShaderSourceCodeMap_;
		};
	}
}
