#ifndef __RESOURCE_CACHE__
#define __RESOURCE_CACHE__

#include "Container/Str.h"
#include "Container/HashMap.h"
#include "Container/RefCounted.h"
#include "Resource/Resource.h"

namespace FlagGG
{
	namespace Resource
	{
		class ResourceCache : public Container::RefCounted
		{
		public:
			ResourceCache(Core::Context* context);

			template < class T >
			Container::SharedPtr<T> GetResource(const Container::String& path)
			{
				Container::String formatPath = FormatReousrcePath(path);

				Container::SharedPtr<Resource> res;

				if (!CheckResource(formatPath, res))
				{
					return nullptr;
				}

				// 存在资源，但没加载过
				if (!res)
				{
					res = new T(context_);
					if (!LoadResource(formatPath, res))
					{
						return nullptr;
					}
				}

				return Container::StaticCast<T>(res);
			}

			void AddResourceDir(const Container::String& path);

			static Container::String FormatReousrcePath(const Container::String& path);

		protected:
			bool CheckResource(const Container::String& path, Container::SharedPtr<Resource>& res);

			bool LoadResource(const Container::String& path, Container::SharedPtr<Resource>& res);

		private:
			Core::Context* context_;

			Container::String resourceDir_;

			Container::HashMap<Container::String, Container::SharedPtr<Resource>> resources_;
		};
	}
}

#endif
