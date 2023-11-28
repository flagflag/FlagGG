#include "Resource/ResourceCache.h"
#include "Utility/SystemHelper.h"
#include "IOFrame/Stream/FileStream.h"
#include "Log.h"

namespace FlagGG
{

SharedPtr<IOFrame::Buffer::IOBuffer> ResourceCache::GetFile(const String& path)
{
	String formatPath = resourceDir_ + FormatReousrcePath(path);
	SharedPtr<IOFrame::Stream::FileStream> file(new IOFrame::Stream::FileStream());
	file->Open(formatPath, FileMode::FILE_READ);
	if (!file->IsOpen())
	{
		FLAGGG_LOG_ERROR("Can not open file[{}].", formatPath.CString());
		return nullptr;
	}
	return file;
}

void ResourceCache::AddResourceDir(const String& path)
{
	resourceDir_ = FormatPath(path).ToLower() + PATH_SEPARATOR;
}

String ResourceCache::FormatReousrcePath(const String& path)
{
	String formatPath = FormatPath(path);
	return formatPath.ToLower();
}

bool ResourceCache::CheckResource(const String& path, SharedPtr<Resource>& res)
{
	if (!FileExists(resourceDir_ + path))
	{
		FLAGGG_LOG_ERROR("file({}) not exists.", path.CString());

		return false;
	}

	auto it = resources_.Find(path);
	res = it != resources_.End() ? it->second_ : nullptr;

	return true;
}

bool ResourceCache::LoadResource(const String& path, SharedPtr<Resource>& res)
{
	if (!res->LoadFile(resourceDir_ + path))
	{
		FLAGGG_LOG_ERROR("Load Resource[{}] failed.", path.CString());
		ASSERT_MESSAGE(false, "Failed to load resource.");
		return false;
	}

	resources_.Insert(MakePair(path, res));

	return true;
}

}
