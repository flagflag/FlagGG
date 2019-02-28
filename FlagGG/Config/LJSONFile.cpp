#include "Config/LJSONFile.h"
#include "Config/LJSONParser.h"
#include "Log.h"

#include <fstream>
#include <memory>

namespace FlagGG
{
	namespace Config
	{
		bool LJSONFile::LoadFile(const std::wstring& fileName)
		{
			std::ifstream stream;
			stream.open(fileName, std::ios::in | std::ios::binary | std::ios::ate);
			if (!stream.is_open())
			{
				FLAGGG_LOG_ERROR("open json file failed.");

				return false;
			}

			size_t fileSize = stream.tellg();
			std::unique_ptr<char[]> buffer(new char[fileSize + 1]);
			stream.seekg(0, std::ios::beg);

			stream.read(buffer.get(), fileSize);
			buffer.get()[fileSize] = '\0';

			LJSONParser parser;
			if (!parser.Load(buffer.get(), fileSize, root_))
			{
				FLAGGG_LOG_ERROR("parse json file failed.");

				return false;
			}

			return true;
		}

		const LJSONValue& LJSONFile::GetRoot() const
		{
			return root_;
		}
	}
}
