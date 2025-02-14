#pragma once

#include "Export.h"
#include "Core/Subsystem.h"
#include "Container/Ptr.h"
#include "Container/Str.h"
#include "Container/HashMap.h"
#include "Container/Vector.h"
#include "Core/EventDefine.h"
#include "Core/BaseTypes.h"

namespace FlagGG
{

struct VariableWrapper : public RefCounted
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

class FlagGG_API Context : public Subsystem<Context>
{
public:
	template < class VariableType >
	void RegisterVariable(VariableType* variable, const String& variableName)
	{
		SharedPtr<VariableWrapper> wrapper(new VariableWrapperImpl<VariableType>(variable));
		wrappers_.Insert(Pair<String, SharedPtr<VariableWrapper>>(variableName, wrapper));
	}

#if PLATFORM_WINDOWS
	template <> void RegisterVariable<Int8>(Int8* variable, const String& variableName);
	template <> void RegisterVariable<UInt8>(UInt8* variable, const String& variableName);
	template <> void RegisterVariable<Int16>(Int16* variable, const String& variableName);
	template <> void RegisterVariable<UInt16>(UInt16* variable, const String& variableName);
	template <> void RegisterVariable<Int32>(Int32* variable, const String& variableName);
	template <> void RegisterVariable<UInt32>(UInt32* variable, const String& variableName);
	template <> void RegisterVariable<Int64>(Int64* variable, const String& variableName);
	template <> void RegisterVariable<UInt64>(UInt64* variable, const String& variableName);
#endif

	template < class VariableType >
	VariableType* GetVariable(const String& variableName)
	{
		auto it = wrappers_.Find(variableName);
		if (it != wrappers_.End())
		{
			return static_cast<VariableType*>(it->second_->GetVariable());
		}
		return nullptr;
	}

	void MarkCurrentThreadAsGameThread();

	void MarkCurrentThreadAsAsyncLoadingThread();

	bool IsInGameThread() const;

	bool IsInAsyncLoadingThread() const;

	void AddFrameNumber();

	UInt32 GetFrameNumber() const { return frameNumber_; }

	UInt32 GetCurrentTemporalAASampleIndex() const { return temporalAASampleIndex_; }

private:
	HashMap<String, SharedPtr<VariableWrapper>> wrappers_;

	UInt64 gameThreadId_{};
	UInt64 asyncLoadingThreadId_{};

	UInt32 frameNumber_{};
	UInt32 temporalAASampleIndex_{};
};

}
