#ifndef __SHADER__
#define __SHADER__

#include "GPUObject.h"

#include <string>

namespace FlagGG
{
	namespace Graphics
	{
		enum ShaderType
		{
			VS = 0,
			PS = 1
		};

		class Shader : public GPUObject
		{
		public:
			Shader(const std::wstring& shaderPath, ShaderType shaderType);

			~Shader() override;

			void Initialize() override;

			bool IsValid() override;

			ID3DBlob* GetByteCode();

		private:
			std::wstring shaderPath_;

			ID3DBlob* shaderCode_{ nullptr };

			ShaderType shaderType_;
		};
	}
}

#endif