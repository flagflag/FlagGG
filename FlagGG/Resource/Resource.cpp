#include "Resource/Resource.h"
#include "IOFrame/Stream/FileStream.h"
#include "Log.h"

namespace FlagGG
{

Resource::Resource()
{ }
		
void Resource::SetName(const String& name)
{
	name_ = name;
}

bool Resource::LoadFile(const String& filepath)
{
	IOFrame::Stream::FileStream fileStream;
	fileStream.Open(filepath, IOFrame::Stream::FileMode::FILE_READ);
	if (!fileStream.IsOpen())
	{
		FLAGGG_LOG_ERROR("Failed to open file[{}].", filepath.CString());
		return false;
	}

	return LoadStream(&fileStream);
}

bool Resource::LoadStream(IOFrame::Buffer::IOBuffer* stream)
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

bool Resource::SaveFile(const String& filepath)
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
	return true;
}

bool Resource::EndLoad()
{
	return true;
}

bool Resource::BeginSave(IOFrame::Buffer::IOBuffer* stream)
{
	return true;
}

bool Resource::EndSave()
{
	return true;
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
