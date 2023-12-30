#pragma once

#include "Container/HashMap.h"
#include "Container/StringHash.h"
#include "Container/Ptr.h"

#include <functional>

namespace FlagGG
{

template < class ObjectType, class ... Args >
class IoC
{
public:
	using Constructor = std::function<ObjectType*(Args&& ... args)>;
	using Destructor = std::function<void(ObjectType*)>;

	template < class TargetType, class ... ExtraArgs >
	void RegisterType(StringHash typeHash, ExtraArgs ... extraArgs)
	{
		Constructor constructor = [&](Args&& ... args) { return new TargetType(std::forward<Args>(args)..., extraArgs...); };
		constructors_.Insert(MakePair(typeHash, constructor));
	}

	template < class TargetType >
	void RegisterTypeEx(StringHash typeHash, Constructor constructor, Destructor destructor)
	{
		constructors_.Insert(MakePair(typeHash, std::forward<Constructor>(constructor)));
		destructors_.Insert(MakePair(typeHash, std::forward<Destructor>(destructor)));
	}

	ObjectType* Create(StringHash typeHash, Args&& ... args)
	{
		auto it = constructors_.Find(typeHash);
		if (it == constructors_.End())
			return nullptr;
		return (it->second_)(std::forward<Args>(args)...);
	}

	void Destroy(StringHash typeHash, ObjectType* object)
	{
		auto it = destructors_.Find(typeHash);
		if (it == destructors_.End())
		{
			delete object;
			return;
		}
		(it->second_)(object);
	}

private:
	HashMap<StringHash, Constructor> constructors_;
	HashMap<StringHash, Destructor> destructors_;
};

}
