//
// Windows平台TLS
//

#pragma once

#include "Core/BaseTypes.h"
#include "Core/BaseMacro.h"

namespace FlagGG
{

namespace Windows
{

FlagGG_API UInt32 TlsAlloc();
FlagGG_API void TlsSetValue(UInt32 SlotIndex, void* Value);
FlagGG_API void* TlsGetValue(UInt32 SlotIndex);
FlagGG_API void TlsFree(UInt32 SlotIndex);
FlagGG_API UInt32 GetCurrentThreadId();

}

/**
 * Windows implementation of the TLS OS functions.
 */
struct FlagGG_API WindowsPlatformTLS
{
	/**
	 * Returns the currently executing thread's identifier.
	 *
	 * @return The thread identifier.
	 */
	static FORCEINLINE UInt32 GetCurrentThreadId(void)
	{
		return Windows::GetCurrentThreadId();
	}

	/**
	 * Return false if this is an invalid TLS slot
	 * @param SlotIndex the TLS index to check
	 * @return true if this looks like a valid slot
	 */
	static FORCEINLINE bool IsValidTlsSlot(UInt32 slotIndex)
	{
		return slotIndex != 0xFFFFFFFF;
	}

	/**
	 * Allocates a thread local store slot.
	 *
	 * @return The index of the allocated slot.
	 */
	static FORCEINLINE UInt32 AllocTlsSlot(void)
	{
		return Windows::TlsAlloc();
	}

	/**
	 * Sets a value in the specified TLS slot.
	 *
	 * @param SlotIndex the TLS index to store it in.
	 * @param Value the value to store in the slot.
	 */
	static FORCEINLINE void SetTlsValue(UInt32 slotIndex, void* value)
	{
		Windows::TlsSetValue(slotIndex, value);
	}

	/**
	 * Reads the value stored at the specified TLS slot.
	 *
	 * @param SlotIndex The index of the slot to read.
	 * @return The value stored in the slot.
	 */
	static FORCEINLINE void* GetTlsValue(UInt32 slotIndex)
	{
		return Windows::TlsGetValue(slotIndex);
	}

	/**
	 * Frees a previously allocated TLS slot
	 *
	 * @param SlotIndex the TLS index to store it in
	 */
	static FORCEINLINE void FreeTlsSlot(UInt32 slotIndex)
	{
		Windows::TlsFree(slotIndex);
	}
};

typedef WindowsPlatformTLS PlatformTLS;

}