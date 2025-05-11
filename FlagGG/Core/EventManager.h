#pragma once

#include "Export.h"
#include "Core/Subsystem.h"
#include "Container/Ptr.h"
#include "Container/Str.h"
#include "Container/HashMap.h"
#include "Container/Vector.h"

namespace FlagGG
{

struct EventWrapper : public RefCounted
{
	virtual void* GetEvent() = 0;
};

template < class T >
struct EventWrapperImpl : public EventWrapper
{
	explicit EventWrapperImpl(T* eventEntry) :
		eventEntry_(eventEntry)
	{ }

	~EventWrapperImpl() override
	{
		if (eventEntry_)
		{
			delete eventEntry_;
			eventEntry_ = nullptr;
		}
	}

	void* GetEvent() override
	{
		return eventEntry_;
	}

private:
	void* instance_;

	T* eventEntry_;
};

class FlagGG_API EventManager : public Subsystem<EventManager>
{
public:
	template < class FunctionType >
	void RegisterEvent(UInt32 eventId, const FunctionType& func)
	{
		FunctionType* pFunc = new FunctionType(func);
		SharedPtr<EventWrapper> wrapper(new EventWrapperImpl<FunctionType>(pFunc));
		evnets_[eventId].Push(wrapper);
	}

	template < class HandlerType, class ... Args >
	void SendEvent(Args&& ... args)
	{
		auto it = evnets_.Find(HandlerType::GetID());
		if (it != evnets_.End())
		{
			Vector<SharedPtr<EventWrapper>>& wrappers = it->second_;
			for (auto itWrapper = wrappers.Begin(); itWrapper != wrappers.End(); ++itWrapper)
			{
				(*static_cast<typename HandlerType::FunctionType*>((*itWrapper)->GetEvent()))(std::forward<Args>(args)...);
			}
		}
	}

private:
	HashMap<UInt32, Vector<SharedPtr<EventWrapper>>> evnets_;
};

}
