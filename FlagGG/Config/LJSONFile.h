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
			~LJSONFile() override = default;

			bool LoadFile(const Container::String& fileName) override;

			const LJSONValue& GetRoot() const;

		private:
			LJSONValue root_;
		};
	}
}

