#ifndef __CONTEXT__
#define __CONTEXT__

#include "Export.h"
#include "Container/Ptr.h"
#include "Container/Str.h"
#include "Container/HashMap.h"
#include "Core/EventDefine.h"
#include "Container/Ptr.h"

#include <stdint.h>

namespace FlagGG
{
	namespace Core
	{
		struct VariableWrapperBase : public Container::RefCounted
		{
			virtual void* GetVariable() = 0;
		};

		template < class T >
		struct VariableWrapper : public VariableWrapperBase
		{
			explicit VariableWrapper(T* value) :
				value_(value)
			{ }

			void* GetVariable() override
			{
				return value_;
			}

			T* value_;
		};

		struct EventWrapperBase : public Container::RefCounted
		{
			virtual ~EventWrapperBase() = default;
			virtual void* GetInstance() = 0;
			virtual void* GetEvent() = 0;
		};

		template < class T >
		struct EventWrapper : public EventWrapperBase
		{
			explicit EventWrapper(void* instance, T* eventEntry) :
				instance_(instance),
				eventEntry_(eventEntry)
			{ }

			~EventWrapper() override
			{
				if (eventEntry_)
				{
					delete eventEntry_;
					eventEntry_ = nullptr;
				}
			}

			void* GetInstance() override
			{
				return instance_;
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
				Container::SharedPtr<VariableWrapperBase> wrapper(new VariableWrapper<VariableType>(variable));
				wrappers_.Insert(Container::Pair<Container::String, Container::SharedPtr<VariableWrapperBase>>(variableName, wrapper));
			}

			template <> void RegisterVariable<int8_t>(int8_t* variable, const Container::String& variableName);
			template <> void RegisterVariable<uint8_t>(uint8_t* variable, const Container::String& variableName);
			template <> void RegisterVariable<int16_t>(int16_t* variable, const Container::String& variableName);
			template <> void RegisterVariable<uint16_t>(uint16_t* variable, const Container::String& variableName);
			template <> void RegisterVariable<int32_t>(int32_t* variable, const Container::String& variableName);
			template <> void RegisterVariable<uint32_t>(uint32_t* variable, const Container::String& variableName);
			template <> void RegisterVariable<int64_t>(int64_t* variable, const Container::String& variableName);
			template <> void RegisterVariable<uint64_t>(uint64_t* variable, const Container::String& variableName);

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
			void RegisterEvent(uint32_t eventId, const FunctionType& func)
			{
				FunctionType* pFunc = new FunctionType(func);
				Container::SharedPtr<EventWrapperBase> wrapper(new EventWrapper<FunctionType>(nullptr, pFunc));
				evnets_[eventId].Push(wrapper);
			}

			template < class FunctionType >
			void RegisterEvent(uint32_t eventId, const FunctionType& func, void* instance)
			{
				FunctionType* pFunc = new FunctionType(func);
				Container::SharedPtr<EventWrapperBase> wrapper(new EventWrapper<FunctionType>(instance, pFunc));
				evnets_[eventId].Push(wrapper);
			}

			template < class FunctionType, class ... Args >
			void SendEvent(uint32_t eventId, Args ... args)
			{
				auto it = evnets_.Find(eventId);
				if (it != evnets_.End())
				{
					Container::Vector<Container::SharedPtr<EventWrapperBase>>& wrappers = it->second_;
					for (auto itWrapper = wrappers.Begin(); itWrapper != wrappers.End(); ++itWrapper)
					{
						(*static_cast<FunctionType*>((*itWrapper)->GetEvent()))(args ...);
					}
				}
			}

		private:
			Container::HashMap<Container::String, Container::SharedPtr<VariableWrapperBase>> wrappers_;
			Container::HashMap<uint32_t, Container::Vector<Container::SharedPtr<EventWrapperBase>>> evnets_;
		};
	}
}

#endif
