#include "Resource/ResourceCache.h"
#include "Utility/SystemHelper.h"
#include "Log.h"

namespace FlagGG
{
	namespace Resource
	{
		ResourceCache::ResourceCache(Core::Context* context) :
			context_(context)
		{ }

		void ResourceCache::AddResourceDir(const Container::String& path)
		{
			resourceDir_ = Utility::SystemHelper::FormatPath(path).ToLower() + Utility::SystemHelper::PATH_SEPARATOR;
		}

		Container::String ResourceCache::FormatReousrcePath(const Container::String& path)
		{
			Container::String formatPath = Utility::SystemHelper::FormatPath(path);
			return formatPath.ToLower();
		}

		bool ResourceCache::CheckResource(const Container::String& path, Container::SharedPtr<Resource>& res)
		{
			if (!Utility::SystemHelper::FileExists(resourceDir_ + path))
			{
				FLAGGG_LOG_ERROR("file(%s) not exists.", path.CString());

				return false;
			}

			auto it = resources_.Find(path);
			res = it != resources_.End() ? it->second_ : nullptr;

			return true;
		}

		bool ResourceCache::LoadResource(const Container::String& path, Container::SharedPtr<Resource>& res)
		{
			if (!res->LoadFile(resourceDir_ + path))
			{
				FLAGGG_LOG_ERROR("Load Resource[%s] failed.", path.CString());

				return false;
			}

			resources_.Insert(Container::MakePair(path, res));

			return true;
		}
	}
}
