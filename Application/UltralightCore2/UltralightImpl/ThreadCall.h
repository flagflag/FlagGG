#pragma once

#include <Ultralight/Defines.h>
#include <Core/Subsystem.h>
#include <Core/Forwarder.h>
#include <AsyncFrame/Mutex.h>
#include <Container/HashMap.h>

#define APP_CORE_FOR_ENGINE 1

class UExport ThreadRenderCall : public FlagGG::Subsystem<ThreadRenderCall>, public FlagGG::Forwarder<FlagGG::Mutex>
{
public:
	void SetMultiThread(bool isMultiThead)
	{
		isMultiThread_ = isMultiThead;
	}
	
	bool IsMultiThread() const { return isMultiThread_; }

	void Forward(FlagGG::ForwarderFuncType&& func)
	{
		if (isMultiThread_)
		{
			FlagGG::Forwarder<FlagGG::Mutex>::Forward(std::forward<FlagGG::ForwarderFuncType>(func));
		}
		else
		{
			func();
		}
	}

	void Forward(const FlagGG::ForwarderFuncType& func)
	{
		if (isMultiThread_)
		{
			FlagGG::Forwarder<FlagGG::Mutex>::Forward(func);
		}
		else
		{
			func();
		}
	}

	void SetRenderCall(void *ptr, FlagGG::ForwarderFuncType renderCall)
	{
		if (isMultiThread_)
		{
			renderCallMutex_.Lock();
			if (renderCall)
				renderCallsMap_[ptr] = renderCall;
			else
				renderCallsMap_.Erase(ptr);
			renderCallMutex_.UnLock();
		}
		else
		{
			if (renderCall)
				renderCall();
		}
	}

	void DoRenderCall()
	{
		if (isMultiThread_)
		{
			renderCallMutex_.Lock();
			for (auto& it : renderCallsMap_)
			{
				it.second_();
			}
			renderCallMutex_.UnLock();
		}
	}

private:
	FlagGG::Mutex renderCallMutex_;

	FlagGG::HashMap<void*, FlagGG::ForwarderFuncType> renderCallsMap_;

	bool isMultiThread_{};
};
