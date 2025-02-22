#include "Config/LJSONFile.h"
#include "Config/LJSONParser.h"
#include "Container/ArrayPtr.h"
#include "Core/ObjectFactory.h"
#include "Log.h"

#include <fstream>
#include <memory>

namespace FlagGG
{

REGISTER_TYPE_FACTORY(LJSONFile);

LJSONFile::LJSONFile() :
	Resource()
{ }

bool LJSONFile::BeginLoad(IOFrame::Buffer::IOBuffer* stream)
{
	String buffer;
	stream->ToString(buffer);

	LJSONParser parser;
	if (!parser.Load(buffer.CString(), buffer.Length(), root_))
	{
		FLAGGG_LOG_ERROR("parse json file failed.");

		return false;
	}

	return true;
}

bool LJSONFile::EndLoad()
{
	return true;
}

}
