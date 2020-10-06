#include "Resource/Resource.h"
#include "IOFrame/Stream/FileStream.h"
#include "Log.h"

namespace FlagGG
{
	namespace Resource
	{
		Resource::Resource(Core::Context* context) :
			context_(context)
		{ }

		void Resource::SetName(const Container::String& name)
		{
			name_ = name;
		}

		bool Resource::LoadFile(const Container::String& filepath)
		{
			IOFrame::Stream::FileStream fileStream;
			fileStream.Open(filepath, IOFrame::Stream::FileMode::FILE_READ);
			if (!fileStream.IsOpen())
			{
				FLAGGG_LOG_ERROR("Failed to open file[{}].", filepath.CString());
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

		bool Resource::SaveFile(const Container::String& filepath)
		{
			IOFrame::Stream::FileStream fileStream;
			fileStream.Open(filepath, IOFrame::Stream::FileMode::FILE_WRITE);
			if (!fileStream.IsOpen())
			{
				FLAGGG_LOG_ERROR("Failed to open file[{}].", filepath.CString());
				return false;
			}

			return SaveFile(&fileStream);
		}

		bool Resource::SaveFile(IOFrame::Buffer::IOBuffer* stream)
		{
			bool result = BeginSave(stream);
			if (!result)
			{
				FLAGGG_LOG_ERROR("SaveFile ==> BeginSave failed.");
				return false;
			}

			result = EndSave();
			if (!result)
			{
				FLAGGG_LOG_ERROR("SaveFile ==> EndSave failed.");
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

		bool Resource::BeginSave(IOFrame::Buffer::IOBuffer* stream)
		{
			return false;
		}

		bool Resource::EndSave()
		{
			return false;
		}

		void Resource::SetMemoryUse(UInt32 memoryUse)
		{
			memoryUse_ = memoryUse;
		}

		UInt32 Resource::GetMemoryUse() const
		{
			return memoryUse_;
		}
	}
}
