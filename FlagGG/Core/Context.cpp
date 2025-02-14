#include "Core/Context.h"
#include "AsyncFrame/Thread/UniqueThread.h"

namespace FlagGG
{

#if PLATFORM_WINDOWS
template <> void Context::RegisterVariable<Int8>(Int8* variable, const String& variableName)
{
            
}

template <> void Context::RegisterVariable<UInt8>(UInt8* variable, const String& variableName)
{

}

template <> void Context::RegisterVariable<Int16>(Int16* variable, const String& variableName)
{

}

template <> void Context::RegisterVariable<UInt16>(UInt16* variable, const String& variableName)
{

}

template <> void Context::RegisterVariable<Int32>(Int32* variable, const String& variableName)
{

}

template <> void Context::RegisterVariable<UInt32>(UInt32* variable, const String& variableName)
{

}

template <> void Context::RegisterVariable<Int64>(Int64* variable, const String& variableName)
{

}

template <> void Context::RegisterVariable<UInt64>(UInt64* variable, const String& variableName)
{

}
#endif

void Context::MarkCurrentThreadAsGameThread()
{
    gameThreadId_ = UniqueThread::GetCurrentThreadId();
}

void Context::MarkCurrentThreadAsAsyncLoadingThread()
{
    asyncLoadingThreadId_ = UniqueThread::GetCurrentThreadId();
}

bool Context::IsInGameThread() const
{
    return UniqueThread::GetCurrentThreadId() == gameThreadId_;
}

bool Context::IsInAsyncLoadingThread() const
{
    return UniqueThread::GetCurrentThreadId() == asyncLoadingThreadId_;
}

void Context::AddFrameNumber()
{
    ++frameNumber_;

    ++temporalAASampleIndex_;
    if (temporalAASampleIndex_ >= 17)
    {
        temporalAASampleIndex_ = 0;
    }
}

}
