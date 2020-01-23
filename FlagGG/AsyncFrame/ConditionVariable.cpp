#include "AsyncFrame/ConditionVariable.h"

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
		ConditionVariable::ConditionVariable()
		{
#ifdef _WIN32
			handle_ = new CONDITION_VARIABLE;
			InitializeConditionVariable(GetObject<CONDITION_VARIABLE>());
#else
			handle_ = new pthread_cond_t;
			pthread_cond_init(GetObject<pthread_cond_t>(), nullptr);
#endif
		}

		ConditionVariable::~ConditionVariable()
		{
			if (handle_)
			{
#ifdef _WIN32
				auto* cv = GetObject<CONDITION_VARIABLE>();
				delete cv;
#else
				auto cnd = GetObject<pthread_cond_t>();
				delete cnd;
				pthread_cond_destroy(cnd);
#endif
				handle_ = NULL_HANDLE;
			}
		}

		void ConditionVariable::Wait(KernelObject::HandleObject* object)
		{
#ifdef _WIN32
			SleepConditionVariableCS(GetObject<CONDITION_VARIABLE>(), object->GetObject<CRITICAL_SECTION>(), INFINITE);
#else
			pthread_cond_wait(GetObject<pthread_cond_t>(), object->GetObject<pthread_mutex_t>());
#endif
		}

		void ConditionVariable::Wait(KernelObject::HandleObject* object, UInt32 waitTime)
		{
#ifdef _WIN32
			SleepConditionVariableCS(GetObject<CONDITION_VARIABLE>(), object->GetObject<CRITICAL_SECTION>(), waitTime);
#else
			timespec timeout;
			timeout.tv_sec = 0;
			timeout.tv_nsec = static_cast<uint64_t>(waitTime) * 1000000u; // ∑¿÷π±¨int
			pthread_cond_timedwait(GetObject<pthread_cond_t>(), object->GetObject<pthread_mutex_t>(), &timeout);
#endif
		}

		void ConditionVariable::Trigger()
		{
#ifdef _WIN32
			WakeConditionVariable(GetObject<CONDITION_VARIABLE>());
#else
			pthread_cond_signal(GetObject<pthread_cond_t>());
#endif
		}
	}
}
