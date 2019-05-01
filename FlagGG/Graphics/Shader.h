#ifndef __SHADER__
#define __SHADER__

#include "Export.h"

#include "Graphics/GPUObject.h"
#include "Resource/Resource.h"
#include "Container/ArrayPtr.h"

#include <string>

namespace FlagGG
{
	namespace Graphics
	{
		enum ShaderType
		{
			None	= 0,
			VS		= 1,
			PS		= 2
		};

		class FlagGG_API Shader : public GPUObject, public Resource::Resource
		{
		public:
			Shader(Core::Context* context);

			~Shader() override;

			bool IsValid() override;

			void Initialize() override;

			void SetType(ShaderType type);

			ShaderType GetType();

			ID3DBlob* GetByteCode();

		protected:
			bool BeginLoad(IOFrame::Buffer::IOBuffer* stream) override;

			bool EndLoad() override;

		private:
			// shader代码
			Container::SharedArrayPtr<char> buffer_;
			size_t bufferSize_{ 0 };

			ID3DBlob* shaderCode_{ nullptr };

			ShaderType shaderType_{ None };
		};
	}
}

#endif