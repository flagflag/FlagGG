#include "AsyncFrame/Mutex.h"

#ifdef _WIN32
#include <windows.h>
#ifdef GetObject
#undef GetObject
#endif
#else
#include <pthread.h>
#endif

namespace FlagGG
{
	namespace AsyncFrame
	{
		Mutex::Mutex()
		{
#ifdef _WIN32
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
#ifdef _WIN32
				auto* cs = GetObject<CRITICAL_SECTION>();
				DeleteCriticalSection(cs);
				delete cs;
#else
				auto* mutex = GetObject<pthread_mutex_t>();
				pthread_mutex_destroy(mutex);
				delete mutex;
#endif
				handle_ = nullptr;
			}
		}

		void Mutex::Lock()
		{
#ifdef _WIN32
			EnterCriticalSection(GetObject<CRITICAL_SECTION>());
#else
			pthread_mutex_lock(GetObject<pthread_mutex_t>());
#endif
		}

		bool Mutex::TryLock()
		{
#ifdef _WIN32
			return TryEnterCriticalSection(GetObject<CRITICAL_SECTION>()) != FALSE;
#else
			return pthread_mutex_trylock(GetObject<pthread_mutex_t>()) == 0;
#endif
		}

		void Mutex::UnLock()
		{
#ifdef _WIN32
			LeaveCriticalSection(GetObject<CRITICAL_SECTION>());
#else
			pthread_mutex_unlock(GetObject<pthread_mutex_t>());
#endif
		}
	}
}
