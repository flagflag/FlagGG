#include "Resource/ResourceCache.h"
#include "Utility/SystemHelper.h"
#include "IOFrame/Stream/FileStream.h"
#include "FileSystem/FileManager.h"
#include "Log.h"

namespace FlagGG
{

String ResourceCache::FormatReousrcePath(const String& path)
{
	String formatPath = FormatPath(path);
	return formatPath.ToLower();
}

bool ResourceCache::CheckResource(const String& path, SharedPtr<Resource>& res)
{
	if (!GetSubsystem<AssetFileManager>()->FileExists(path))
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
	auto buffer = GetSubsystem<AssetFileManager>()->OpenFileReader(path);

	if (!buffer)
	{
		FLAGGG_LOG_ERROR("Can not open file stream [{}].", path.CString());
		return false;
	}

	if (!res->LoadStream(buffer))
	{
		FLAGGG_LOG_ERROR("Load Resource[{}] failed.", path.CString());
		ASSERT_MESSAGE(false, "Failed to load resource.");
		return false;
	}

	resources_.Insert(MakePair(path, res));

	return true;
}

}
