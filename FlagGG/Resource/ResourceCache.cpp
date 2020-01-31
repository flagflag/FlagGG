#include "Resource/ResourceCache.h"
#include "Utility/SystemHelper.h"
#include "IOFrame/Stream/FileStream.h"
#include "Log.h"

namespace FlagGG
{
	namespace Resource
	{
		ResourceCache::ResourceCache(Core::Context* context) :
			context_(context)
		{ }

		Container::SharedPtr<IOFrame::Buffer::IOBuffer> ResourceCache::GetFile(const Container::String& path)
		{
			Container::String formatPath = resourceDir_ + FormatReousrcePath(path);
			Container::SharedPtr<IOFrame::Stream::FileStream> file(new IOFrame::Stream::FileStream());
			file->Open(formatPath, IOFrame::Stream::FileMode::FILE_READ);
			if (!file->IsOpen())
			{
				FLAGGG_LOG_ERROR("Can not open file[{}].", formatPath.CString());
				return nullptr;
			}
			return file;
		}

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
				FLAGGG_LOG_ERROR("file({}) not exists.", path.CString());

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
				FLAGGG_LOG_ERROR("Load Resource[{}] failed.", path.CString());

				return false;
			}

			resources_.Insert(Container::MakePair(path, res));

			return true;
		}
	}
}
