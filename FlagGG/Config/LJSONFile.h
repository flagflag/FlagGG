#pragma once

#include "Resource/Resource.h"
#include "Config/LJSONValue.h"
#include "Export.h"

namespace FlagGG
{
	namespace Config
	{
		// Like JSON File
		class FlagGG_API LJSONFile : public Resource::Resource
		{
		public:
			LJSONFile(Core::Context* context);

			~LJSONFile() override = default;

			const LJSONValue& GetRoot() const;

		protected:
			bool BeginLoad(IOFrame::Buffer::IOBuffer* stream) override;

			bool EndLoad() override;

		private:
			LJSONValue root_;
		};
	}
}

