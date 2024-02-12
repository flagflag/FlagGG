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
	template <class T>
	T* GetResource(const String& path)
	{
		return RTTICast<T>(GetResource(T::GetTypeStatic(), path));
	}

	template <class T>
	T* GetExistingResource(const String& path)
	{
		return RTTICast<T>(GetExistingResource(T::GetTypeStatic(), path));
	}

	Resource* GetResource(StringHash type, const String& path);

	Resource* GetExistingResource(StringHash type, const String& path);

	void AddManualResource(Resource* resource);

	static String FormatReousrcePath(const String& path);

private:
	HashMap<StringHash, HashMap<String, SharedPtr<Resource>>> typeToResourceMapping_;
};

}
