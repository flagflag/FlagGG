//
// Apple平台TLS
//

#pragma once

#include "Core/BaseTypes.h"
#include "Core/BaseMacro.h"

#include <pthread.h>
#include <mach/mach.h>

namespace FlagGG
{

/**
 * Windows implementation of the TLS OS functions.
 */
struct FlagGG_API ApplePlatformTLS
{
	static const UInt32 InvalidTlsSlot = 0xFFFFFFFF;

	/**
	 * Returns the currently executing thread's identifier.
	 *
	 * @return The thread identifier.
	 */
	static FORCEINLINE UInt32 GetCurrentThreadId(void)
	{
		return (UInt32)pthread_mach_thread_np(pthread_self());
	}

	/**
	 * Return false if this is an invalid TLS slot
	 * @param slotIndex the TLS index to check
	 * @return true if this looks like a valid slot
	 */
	static FORCEINLINE bool IsValidTlsSlot(UInt32 slotIndex)
	{
		return slotIndex != InvalidTlsSlot;
	}

	/**
	 * Allocates a thread local store slot.
	 *
	 * @return The index of the allocated slot.
	 */
	static FORCEINLINE UInt32 AllocTlsSlot(void)
	{
		pthread_key_t slotKey = 0;
		if (pthread_key_create(&slotKey, nullptr) != 0)
		{
			slotKey = InvalidTlsSlot;
		}
		return slotKey;
	}

	/**
	 * Sets a value in the specified TLS slot.
	 *
	 * @param slotIndex the TLS index to store it in.
	 * @param value the value to store in the slot.
	 */
	static FORCEINLINE void SetTlsValue(UInt32 slotIndex, void* value)
	{
		pthread_setspecific((pthread_key_t)slotIndex, value);
	}

	/**
	 * Reads the value stored at the specified TLS slot.
	 *
	 * @param slotIndex The index of the slot to read.
	 * @return The value stored in the slot.
	 */
	static FORCEINLINE void* GetTlsValue(UInt32 slotIndex)
	{
		return pthread_getspecific((pthread_key_t)slotIndex);
	}

	/**
	 * Frees a previously allocated TLS slot
	 *
	 * @param slotIndex the TLS index to store it in
	 */
	static FORCEINLINE void FreeTlsSlot(UInt32 slotIndex)
	{
		pthread_key_delete((pthread_key_t)slotIndex);
	}
};

typedef ApplePlatformTLS PlatformTLS;

}