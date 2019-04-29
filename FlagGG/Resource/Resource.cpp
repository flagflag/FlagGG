#include "Resource/Resource.h"
#include "IOFrame/Stream/FileStream.h"
#include "Log.h"

namespace FlagGG
{
	namespace Resource
	{
		bool Resource::LoadFile(const Container::String& fileName)
		{
			IOFrame::Stream::FileStream fileStream;
			fileStream.Open(fileName);
			if (!fileStream.IsOpen())
			{
				FLAGGG_LOG_ERROR("open file[%s] filed.", fileName.CString());

				return false;
			}

			return LoadFile(&fileStream);
		}

		bool Resource::LoadFile(IOFrame::Buffer::IOBuffer* stream)
		{
			bool result = BeginLoad(stream);
			if (!result)
			{
				FLAGGG_LOG_ERROR("LoadFile ==> BeginLoad failed.");

				return false;
			}

			result = EndLoad();
			if (!result)
			{
				FLAGGG_LOG_ERROR("LoadFile ==> EndLoad failed.");

				return false;
			}

			return true;
		}

		bool Resource::BeginLoad(IOFrame::Buffer::IOBuffer* stream)
		{
			return false;
		}

		bool Resource::EndLoad()
		{
			return false;
		}
	}
}
