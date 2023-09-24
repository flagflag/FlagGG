#pragma once

#include "Container/Str.h"
#include "Container/HashMap.h"
#include "Container/RefCounted.h"
#include "Resource/Resource.h"
#include "Core/Subsystem.h"

namespace FlagGG
{

class FlagGG_API ResourceCache : public Subsystem<ResourceCache>
{
public:
	template < class T >
	SharedPtr<T> GetResource(const String& path)
	{
		String formatPath = FormatReousrcePath(path);

		SharedPtr<Resource> res;

		if (!CheckResource(formatPath, res))
		{
			return nullptr;
		}

		// 存在资源，但没加载过
		if (!res)
		{
			res = new T();
			res->SetName(path);
			if (!LoadResource(formatPath, res))
			{
				return nullptr;
			}
		}

		return StaticCast<T>(res);
	}

	SharedPtr<IOFrame::Buffer::IOBuffer> GetFile(const String& path);

	void AddResourceDir(const String& path);

	static String FormatReousrcePath(const String& path);

protected:
	bool CheckResource(const String& path, SharedPtr<Resource>& res);

	bool LoadResource(const String& path, SharedPtr<Resource>& res);

private:
	String resourceDir_;

	HashMap<String, SharedPtr<Resource>> resources_;
};

}
