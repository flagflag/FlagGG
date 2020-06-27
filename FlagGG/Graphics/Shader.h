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

			bool PreCompileShaderCode(const char* head, const char* tail, Container::String& out);

		private:
			Container::Vector<Container::SharedPtr<Shader>> shaders_;

			// shader代码
			Container::SharedArrayPtr<char> buffer_;
			UInt32 bufferSize_{ 0 };
		};

		// 经过编译的shader，是GPU对象
		class FlagGG_API Shader : public GPUObject, public Container::RefCounted
		{
		public:
			Shader(Container::SharedArrayPtr<char> buffer, UInt32 bufferSize);

			~Shader() override;

			bool IsValid() override;

			void Initialize() override;

			void SetType(ShaderType type);

			void SetDefines(const Container::Vector<Container::String>& defines);

			Container::String GetDefinesString() const;

			ShaderType GetType();

		private:
			// shader代码
			Container::SharedArrayPtr<char> buffer_;
			UInt32 bufferSize_{ 0 };

			ShaderType shaderType_{ None };

			Container::Vector<Container::String> defines_;
			Container::String definesString_;
		};
	}
}

#endif