#include "Container/LockFreeList.h"
#include "Utility/SystemHelper.h"
#include "Platform/PlatformTLS.h"
#include "Core/Context.h"

namespace FlagGG
{

#if _DEBUG

void DoTestCriticalStall()
{
	float Test = Random();
	if (Test < .001)
	{
		SleepNoStats(1);
	}
	else if (Test < .01f)
	{
		SleepNoStats(0);
	}
}

Int32 GTestCriticalStalls = 0;

#endif

void LockFreeTagCounterHasOverflowed()
{
	FLAGGG_LOG_DEBUG("LockFree Tag has overflowed...(not a problem).");
	Sleep(1);
}

void LockFreeLinksExhausted(UInt32 TotalNum)
{
	FLAGGG_LOG_STD_DEBUG("Consumed %d lock free links; there are no more.", TotalNum);
}

static void ChangeMem(Int64 Delta)
{

}

void* LockFreeAllocLinks(USize AllocSize)
{
	ChangeMem(AllocSize);
	return ::malloc(AllocSize);
}
void LockFreeFreeLinks(USize AllocSize, void* Ptr)
{
	ChangeMem(-Int32(AllocSize));
	::free(Ptr);
}

class LockFreeLinkAllocator_TLSCache : public Noncopyable
{
	enum
	{
		NUM_PER_BUNDLE = 64,
	};

	typedef LockFreeLinkPolicy::Link Link;
	typedef LockFreeLinkPolicy::LinkPtr LinkPtr;

public:

	LockFreeLinkAllocator_TLSCache()
	{
		CRY_ASSERT(GetSubsystem<Context>()->IsInGameThread());
		TlsSlot = PlatformTLS::AllocTlsSlot();
		CRY_ASSERT(PlatformTLS::IsValidTlsSlot(TlsSlot));
	}

	/** Destructor, leaks all of the memory **/
	~LockFreeLinkAllocator_TLSCache()
	{
		PlatformTLS::FreeTlsSlot(TlsSlot);
		TlsSlot = 0;
	}

	/**
	* Allocates a memory block of size SIZE.
	*
	* @return Pointer to the allocated memory.
	* @see Free
	*/
	LinkPtr Pop() TSAN_SAFE
	{
		ThreadLocalCache& TLS = GetTLS();

		if (!TLS.PartialBundle)
		{
			if (TLS.FullBundle)
			{
				TLS.PartialBundle = TLS.FullBundle;
				TLS.FullBundle = 0;
			}
			else
			{
				TLS.PartialBundle = GlobalFreeListBundles.Pop();
				if (!TLS.PartialBundle)
				{
					Int32 FirstIndex = LockFreeLinkPolicy::LinkAllocator.Alloc(NUM_PER_BUNDLE);
					for (Int32 Index = 0; Index < NUM_PER_BUNDLE; Index++)
					{
						Link* Event = LockFreeLinkPolicy::IndexToLink(FirstIndex + Index);
						Event->DoubleNext.Init();
						Event->SingleNext = 0;
						Event->Payload = (void*)UPtrInt(TLS.PartialBundle);
						TLS.PartialBundle = LockFreeLinkPolicy::IndexToPtr(FirstIndex + Index);
					}
				}
			}
			TLS.NumPartial = NUM_PER_BUNDLE;
		}
		LinkPtr Result = TLS.PartialBundle;
		Link* ResultP = LockFreeLinkPolicy::DerefLink(TLS.PartialBundle);
		TLS.PartialBundle = LinkPtr(UPtrInt(ResultP->Payload));
		TLS.NumPartial--;
		//checkLockFreePointerList(TLS.NumPartial >= 0 && ((!!TLS.NumPartial) == (!!TLS.PartialBundle)));
		ResultP->Payload = nullptr;
		checkLockFreePointerList(!ResultP->DoubleNext.GetPtr() && !ResultP->SingleNext);
		return Result;
	}

	/**
	* Puts a memory block previously obtained from Allocate() back on the free list for future use.
	*
	* @param Item The item to free.
	* @see Allocate
	*/
	void Push(LinkPtr Item) TSAN_SAFE
	{
		ThreadLocalCache& TLS = GetTLS();
		if (TLS.NumPartial >= NUM_PER_BUNDLE)
		{
			if (TLS.FullBundle)
			{
				GlobalFreeListBundles.Push(TLS.FullBundle);
				//TLS.FullBundle = nullptr;
			}
			TLS.FullBundle = TLS.PartialBundle;
			TLS.PartialBundle = 0;
			TLS.NumPartial = 0;
		}
		Link* ItemP = LockFreeLinkPolicy::DerefLink(Item);
		ItemP->DoubleNext.SetPtr(0);
		ItemP->SingleNext = 0;
		ItemP->Payload = (void*)UPtrInt(TLS.PartialBundle);
		TLS.PartialBundle = Item;
		TLS.NumPartial++;
	}

private:

	/** struct for the TLS cache. */
	struct ThreadLocalCache
	{
		LinkPtr FullBundle;
		LinkPtr PartialBundle;
		Int32 NumPartial;

		ThreadLocalCache()
			: FullBundle(0)
			, PartialBundle(0)
			, NumPartial(0)
		{
		}
	};

	ThreadLocalCache& GetTLS()
	{
		CRY_ASSERT(PlatformTLS::IsValidTlsSlot(TlsSlot));
		ThreadLocalCache* TLS = (ThreadLocalCache*)PlatformTLS::GetTlsValue(TlsSlot);
		if (!TLS)
		{
			TLS = new ThreadLocalCache();
			PlatformTLS::SetTlsValue(TlsSlot, TLS);
		}
		return *TLS;
	}

	/** Slot for TLS struct. */
	UInt32 TlsSlot;

	/** Lock free list of free memory blocks, these are all linked into a bundle of NUM_PER_BUNDLE. */
	LockFreePointerListLIFORoot<PLATFORM_CACHE_LINE_SIZE> GlobalFreeListBundles;
};

static LockFreeLinkAllocator_TLSCache* GLockFreeLinkAllocator = nullptr;

void LockFreeLinkPolicy::GlobalLockFreeAlloctorInit()
{
	if (GLockFreeLinkAllocator)
		GlobalLockFreeAlloctorUninit();

	GLockFreeLinkAllocator = new LockFreeLinkAllocator_TLSCache();
}

void LockFreeLinkPolicy::GlobalLockFreeAlloctorUninit()
{
	if (GLockFreeLinkAllocator)
	{
		delete GLockFreeLinkAllocator;
		GLockFreeLinkAllocator = nullptr;
	}
}

void LockFreeLinkPolicy::FreeLockFreeLink(LockFreeLinkPolicy::LinkPtr Item)
{
	GLockFreeLinkAllocator->Push(Item);
}

LockFreeLinkPolicy::LinkPtr LockFreeLinkPolicy::AllocLockFreeLink() TSAN_SAFE
{
	LockFreeLinkPolicy::LinkPtr Result = GLockFreeLinkAllocator->Pop();
	// this can only really be a mem stomp
	checkLockFreePointerList(Result && !LockFreeLinkPolicy::DerefLink(Result)->DoubleNext.GetPtr() && !LockFreeLinkPolicy::DerefLink(Result)->Payload && !LockFreeLinkPolicy::DerefLink(Result)->SingleNext);
	return Result;
}

LockFreeLinkPolicy::TAllocator LockFreeLinkPolicy::LinkAllocator;

}
