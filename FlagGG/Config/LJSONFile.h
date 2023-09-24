#pragma once

#include "Resource/Resource.h"
#include "Config/LJSONValue.h"
#include "Export.h"

namespace FlagGG
{

// Like JSON File
class FlagGG_API LJSONFile : public Resource
{
	OBJECT_OVERRIDE(LJSONFile, Resource);
public:
	LJSONFile();

	~LJSONFile() override = default;

	const LJSONValue& GetRoot() const { return root_; }

protected:
	bool BeginLoad(IOFrame::Buffer::IOBuffer* stream) override;

	bool EndLoad() override;

private:
	LJSONValue root_;
};

}
