#include "Config/LJSONFile.h"
#include "Config/LJSONParser.h"
#include "Container/ArrayPtr.h"
#include "Log.h"

#include <fstream>
#include <memory>

namespace FlagGG
{
	namespace Config
	{
		const LJSONValue& LJSONFile::GetRoot() const
		{
			return root_;
		}

		bool LJSONFile::BeginLoad(IOFrame::Stream::FileStream& fileStream)
		{
			char* buffer = nullptr;
			size_t bufferSize = 0;
			fileStream.ToString(buffer, bufferSize);
			Container::SharedArrayPtr<char> autoDelete(buffer);

			LJSONParser parser;
			if (!parser.Load(buffer, bufferSize, root_))
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
}
