#include "StreamObject.h"
#include "IOFrame/Stream/FileStream.h"
#include "Log.h"

namespace FlagGG
{

bool StreamObject::LoadFile(const String& filepath)
{
	IOFrame::Stream::FileStream fileStream;
	fileStream.Open(filepath, FileMode::FILE_READ);
	if (!fileStream.IsOpen())
	{
		FLAGGG_LOG_ERROR("Failed to open file[{}].", filepath.CString());
		return false;
	}

	return LoadStream(&fileStream);
}

bool StreamObject::LoadStream(IOFrame::Buffer::IOBuffer* stream)
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

bool StreamObject::SaveFile(const String& filepath)
{
	IOFrame::Stream::FileStream fileStream;
	fileStream.Open(filepath, FileMode::FILE_WRITE);
	if (!fileStream.IsOpen())
	{
		FLAGGG_LOG_ERROR("Failed to open file[{}].", filepath.CString());
		return false;
	}

	return SaveFile(&fileStream);
}

bool StreamObject::SaveFile(IOFrame::Buffer::IOBuffer* stream)
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

bool StreamObject::BeginLoad(IOFrame::Buffer::IOBuffer* stream)
{
	return true;
}

bool StreamObject::EndLoad()
{
	return true;
}

bool StreamObject::BeginSave(IOFrame::Buffer::IOBuffer* stream)
{
	return true;
}

bool StreamObject::EndSave()
{
	return true;
}

}
