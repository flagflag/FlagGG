#include "Resource/ResourceCache.h"
#include "Core/ObjectFactory.h"
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

void ResourceCache::AddManualResource(Resource* resource)
{
	if (resource)
	{
		typeToResourceMapping_[resource->GetType()][resource->GetName()] = resource;
	}
}

Resource* ResourceCache::GetResource(StringHash type, const String& path)
{
	const String formatPath = FormatReousrcePath(path);

	if (!GetSubsystem<AssetFileManager>()->FileExists(formatPath))
	{
		FLAGGG_LOG_ERROR("file({}) not exists.", path.CString());
		return nullptr;
	}

	Resource* existingResource = GetExistingResource(type, formatPath);
	if (existingResource)
		return existingResource;

	SharedPtr<Resource> resource(RTTICast<Resource>(GetSubsystem<ObjectFactory>()->Create(type)));
	resource->SetName(formatPath);

	auto buffer = GetSubsystem<AssetFileManager>()->OpenFileReader(formatPath);
	if (!buffer)
	{
		FLAGGG_LOG_ERROR("Can not open file stream [{}].", path.CString());
		return nullptr;
	}

	if (!resource->LoadStream(buffer))
	{
		FLAGGG_LOG_ERROR("Load Resource[{}] failed.", path.CString());
		ASSERT_MESSAGE(false, "Failed to load resource.");
		return nullptr;
	}

	typeToResourceMapping_[type].Insert(MakePair(formatPath, resource));

	return resource;
}

Resource* ResourceCache::GetExistingResource(StringHash type, const String& path)
{
	const String formatPath = FormatReousrcePath(path);
	auto it = typeToResourceMapping_.Find(type);
	if (it != typeToResourceMapping_.End())
	{
		auto it2 = it->second_.Find(formatPath);
		return it2 != it->second_.End() ? it2->second_ : nullptr;
	}
	return nullptr;
}

}
