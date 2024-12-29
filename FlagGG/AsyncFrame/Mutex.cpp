#include "AsyncFrame/Mutex.h"
#include "Core/GenericPlatform.h"

#if PLATFORM_WINDOWS
#include <windows.h>
#ifdef GetObject
#undef GetObject
#endif
#else
#include <pthread.h>
#endif

namespace FlagGG
{

Mutex::Mutex()
{
#if PLATFORM_WINDOWS
	handle_ = new CRITICAL_SECTION;
	InitializeCriticalSection(GetObject<CRITICAL_SECTION>());
#else	
	handle_ = new pthread_mutex_t;
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(GetObject<pthread_mutex_t>(), &attr);
#endif
}

Mutex::~Mutex()
{
	if (handle_)
	{
#if PLATFORM_WINDOWS
		auto* cs = GetObject<CRITICAL_SECTION>();
		DeleteCriticalSection(cs);
		delete cs;
#else
		auto* mutex = GetObject<pthread_mutex_t>();
		pthread_mutex_destroy(mutex);
		delete mutex;
#endif
		handle_ = NULL_HANDLE;
	}
}

void Mutex::Lock()
{
#if PLATFORM_WINDOWS
	EnterCriticalSection(GetObject<CRITICAL_SECTION>());
#else
	pthread_mutex_lock(GetObject<pthread_mutex_t>());
#endif
}

bool Mutex::TryLock()
{
#if PLATFORM_WINDOWS
	return TryEnterCriticalSection(GetObject<CRITICAL_SECTION>()) != FALSE;
#else
	return pthread_mutex_trylock(GetObject<pthread_mutex_t>()) == 0;
#endif
}

void Mutex::UnLock()
{
#if PLATFORM_WINDOWS
	LeaveCriticalSection(GetObject<CRITICAL_SECTION>());
#else
	pthread_mutex_unlock(GetObject<pthread_mutex_t>());
#endif
}

}
