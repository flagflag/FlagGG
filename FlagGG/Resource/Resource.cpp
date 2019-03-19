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

			bool result = BeginLoad(&fileStream);
			if (!result)
			{
				FLAGGG_LOG_ERROR("BeginLoad[%s] failed.", fileName.CString());

				return false;
			}

			result = EndLoad();
			if (!result)
			{
				FLAGGG_LOG_ERROR("EndLoad[%s] failed.", fileName.CString());

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
