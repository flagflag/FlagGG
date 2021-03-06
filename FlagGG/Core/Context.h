#ifndef __CONTEXT__
#define __CONTEXT__

#include "Export.h"
#include "Container/Ptr.h"
#include "Container/Str.h"
#include "Container/HashMap.h"
#include "Core/EventDefine.h"
#include "Container/Ptr.h"
#include "Core/BaseTypes.h"

namespace FlagGG
{
	namespace Core
	{
		struct VariableWrapper : public Container::RefCounted
		{
			virtual void* GetVariable() = 0;
		};

		template < class T >
		struct VariableWrapperImpl : public VariableWrapper
		{
			explicit VariableWrapperImpl(T* value) :
				value_(value)
			{ }

			void* GetVariable() override
			{
				return value_;
			}

			T* value_;
		};

		struct EventWrapper : public Container::RefCounted
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

		class FlagGG_API Context : public Container::RefCounted
		{
		public:
			template < class VariableType >
			void RegisterVariable(VariableType* variable, const Container::String& variableName)
			{
				Container::SharedPtr<VariableWrapper> wrapper(new VariableWrapperImpl<VariableType>(variable));
				wrappers_.Insert(Container::Pair<Container::String, Container::SharedPtr<VariableWrapper>>(variableName, wrapper));
			}

#ifdef _WIN32
			template <> void RegisterVariable<Int8>(Int8* variable, const Container::String& variableName);
			template <> void RegisterVariable<UInt8>(UInt8* variable, const Container::String& variableName);
			template <> void RegisterVariable<Int16>(Int16* variable, const Container::String& variableName);
			template <> void RegisterVariable<UInt16>(UInt16* variable, const Container::String& variableName);
			template <> void RegisterVariable<Int32>(Int32* variable, const Container::String& variableName);
			template <> void RegisterVariable<UInt32>(UInt32* variable, const Container::String& variableName);
			template <> void RegisterVariable<Int64>(Int64* variable, const Container::String& variableName);
			template <> void RegisterVariable<UInt64>(UInt64* variable, const Container::String& variableName);
#endif

			template < class VariableType >
			VariableType* GetVariable(const Container::String& variableName)
			{
				auto it = wrappers_.Find(variableName);
				if (it != wrappers_.End())
				{
					return static_cast<VariableType*>(it->second_->GetVariable());
				}
				return nullptr;
			}

			template < class FunctionType >
			void RegisterEvent(UInt32 eventId, const FunctionType& func)
			{
				FunctionType* pFunc = new FunctionType(func);
				Container::SharedPtr<EventWrapper> wrapper(new EventWrapperImpl<FunctionType>(pFunc));
				evnets_[eventId].Push(wrapper);
			}

			template < class FunctionType, class ... Args >
			void SendEvent(UInt32 eventId, Args&& ... args)
			{
				auto it = evnets_.Find(eventId);
				if (it != evnets_.End())
				{
					Container::Vector<Container::SharedPtr<EventWrapper>>& wrappers = it->second_;
					for (auto itWrapper = wrappers.Begin(); itWrapper != wrappers.End(); ++itWrapper)
					{
						(*static_cast<FunctionType*>((*itWrapper)->GetEvent()))(std::forward<Args>(args)...);
					}
				}
			}

		private:
			Container::HashMap<Container::String, Container::SharedPtr<VariableWrapper>> wrappers_;
			Container::HashMap<UInt32, Container::Vector<Container::SharedPtr<EventWrapper>>> evnets_;
		};
	}
}

#endif
