//
// 无锁队列
//

#pragma once

#include "Core/BaseTypes.h"
#include "Core/BaseMacro.h"
#include "Core/CryAssert.h"
#include "Container/ThreadSafeCounter.h"
#include "Container/Noncopyable.h"
#include "Container/Vector.h"
#include "Platform/PlatformAtomics.h"
#include "Math/Math.h"
#include "Log.h"
#include <functional>
#include <type_traits>

#undef InterlockedCompareExchangePointer;
#undef InterlockedCompareExchange;

// what level of checking to perform...normally checkLockFreePointerList but could be ensure or check
#if 1
	#define checkLockFreePointerList CRY_ASSERT
#else
	#if PLATFORM_WINDOWS
		#pragma warning(disable : 4706)
	#endif
	#define checkLockFreePointerList(x) ((x)||((*(char*)3) = 0))
#endif

#define FunctionRef std::function

namespace FlagGG
{

template <typename T>
FORCEINLINE constexpr bool IsAligned(T value, UInt64 alignment)
{
	static_assert(std::is_integral<T>::value || std::is_pointer<T>::value, "IsAligned expects an integer or pointer type");

	return !((UInt64)value & (alignment - 1));
}

#if _DEBUG
FlagGG_API void DoTestCriticalStall();
extern FlagGG_API Int32 GTestCriticalStalls;

FORCEINLINE void TestCriticalStall()
{
	if (GTestCriticalStalls)
	{
		DoTestCriticalStall();
	}
}
#else
FORCEINLINE void TestCriticalStall()
{
}
#endif

FlagGG_API void LockFreeTagCounterHasOverflowed();
FlagGG_API void LockFreeLinksExhausted(UInt32 TotalNum);
FlagGG_API void* LockFreeAllocLinks(USize AllocSize);
FlagGG_API void LockFreeFreeLinks(USize AllocSize, void* Ptr);

#define MAX_LOCK_FREE_LINKS_AS_BITS (26)
#define MAX_LOCK_FREE_LINKS (1 << 26)

template<int TPaddingForCacheContention>
struct PaddingForCacheContention
{
	UInt8 PadToAvoidContention[TPaddingForCacheContention];
};

template<>
struct PaddingForCacheContention<0>
{
};

template<class T, unsigned int MaxTotalItems, unsigned int ItemsPerPage>
class LockFreeAllocOnceIndexedAllocator
{
	enum
	{
		MaxBlocks = (MaxTotalItems + ItemsPerPage - 1) / ItemsPerPage
	};
public:

	LockFreeAllocOnceIndexedAllocator()
	{
		NextIndex.Increment(); // skip the null ptr
		for (UInt32 Index = 0; Index < MaxBlocks; Index++)
		{
			Pages[Index] = nullptr;
		}
	}

	FORCEINLINE UInt32 Alloc(UInt32 Count = 1)
	{
		UInt32 FirstItem = NextIndex.Add(Count);
		if (FirstItem + Count > MaxTotalItems)
		{
			LockFreeLinksExhausted(MaxTotalItems);
		}
		for (UInt32 CurrentItem = FirstItem; CurrentItem < FirstItem + Count; CurrentItem++)
		{
			new (GetRawItem(CurrentItem)) T();
		}
		return FirstItem;
	}

	FORCEINLINE T* GetItem(UInt32 Index)
	{
		if (!Index)
		{
			return nullptr;
		}
		UInt32 BlockIndex = Index / ItemsPerPage;
		UInt32 SubIndex = Index % ItemsPerPage;
		checkLockFreePointerList(Index < (UInt32)NextIndex.GetValue() && Index < MaxTotalItems && BlockIndex < MaxBlocks && Pages[BlockIndex]);
		return Pages[BlockIndex] + SubIndex;
	}

private:
	void* GetRawItem(UInt32 Index)
	{
		UInt32 BlockIndex = Index / ItemsPerPage;
		UInt32 SubIndex = Index % ItemsPerPage;
		checkLockFreePointerList(Index && Index < (UInt32)NextIndex.GetValue() && Index < MaxTotalItems && BlockIndex < MaxBlocks);
		if (!Pages[BlockIndex])
		{
			T* NewBlock = (T*)LockFreeAllocLinks(ItemsPerPage * sizeof(T));
			checkLockFreePointerList(IsAligned(NewBlock, alignof(T)));
			if (PlatformAtomics::InterlockedCompareExchangePointer((void**)&Pages[BlockIndex], NewBlock, nullptr) != nullptr)
			{
				// we lost discard block
				checkLockFreePointerList(Pages[BlockIndex] && Pages[BlockIndex] != NewBlock);
				LockFreeFreeLinks(ItemsPerPage * sizeof(T), NewBlock);
			}
			else
			{
				checkLockFreePointerList(Pages[BlockIndex]);
			}
		}
		return (void*)(Pages[BlockIndex] + SubIndex);
	}

	UInt8 PadToAvoidContention0[PLATFORM_CACHE_LINE_SIZE];
	ThreadSafeCounter NextIndex;
	UInt8 PadToAvoidContention1[PLATFORM_CACHE_LINE_SIZE];
	T* Pages[MaxBlocks];
	UInt8 PadToAvoidContention2[PLATFORM_CACHE_LINE_SIZE];
};


#define MAX_TagBitsValue (UInt64(1) << (64 - MAX_LOCK_FREE_LINKS_AS_BITS))
struct IndexedLockFreeLink;


MS_ALIGN(8)
struct IndexedPointer
{
	// no constructor, intentionally. We need to keep the ABA double counter in tact

	// This should only be used for IndexedPointer's with no outstanding concurrency.
	// Not recycled links, for example.
	void Init()
	{
		static_assert(((MAX_LOCK_FREE_LINKS - 1) & MAX_LOCK_FREE_LINKS) == 0, "MAX_LOCK_FREE_LINKS must be a power of two");
		Ptrs = 0;
	}

	FORCEINLINE void SetAll(UInt32 Ptr, UInt64 CounterAndState)
	{
		checkLockFreePointerList(Ptr < MAX_LOCK_FREE_LINKS && CounterAndState < (UInt64(1) << (64 - MAX_LOCK_FREE_LINKS_AS_BITS)));
		Ptrs = (UInt64(Ptr) | (CounterAndState << MAX_LOCK_FREE_LINKS_AS_BITS));
	}

	FORCEINLINE UInt32 GetPtr() const
	{
		return UInt32(Ptrs & (MAX_LOCK_FREE_LINKS - 1));
	}

	FORCEINLINE void SetPtr(UInt32 To)
	{
		SetAll(To, GetCounterAndState());
	}

	FORCEINLINE UInt64 GetCounterAndState() const
	{
		return (Ptrs >> MAX_LOCK_FREE_LINKS_AS_BITS);
	}

	FORCEINLINE void SetCounterAndState(UInt64 To)
	{
		SetAll(GetPtr(), To);
	}

	FORCEINLINE void AdvanceCounterAndState(const IndexedPointer &From, UInt64 TABAInc)
	{
		SetCounterAndState(From.GetCounterAndState() + TABAInc);
		if (UNLIKELY(GetCounterAndState() < From.GetCounterAndState()))
		{
			// this is not expected to be a problem and it is not expected to happen very often. When it does happen, we will sleep as an extra precaution.
			LockFreeTagCounterHasOverflowed();
		}
	}

	template<UInt64 TABAInc>
	FORCEINLINE UInt64 GetState() const
	{
		return GetCounterAndState() & (TABAInc - 1);
	}

	template<UInt64 TABAInc>
	FORCEINLINE void SetState(UInt64 Value)
	{
		checkLockFreePointerList(Value < TABAInc);
		SetCounterAndState((GetCounterAndState() & ~(TABAInc - 1)) | Value);
	}

	FORCEINLINE void AtomicRead(const IndexedPointer& Other)
	{
		checkLockFreePointerList(IsAligned(&Ptrs, 8) && IsAligned(&Other.Ptrs, 8));
		Ptrs = UInt64(PlatformAtomics::AtomicRead((volatile const Int64*)&Other.Ptrs));
		TestCriticalStall();
	}

	FORCEINLINE bool InterlockedCompareExchange(const IndexedPointer& Exchange, const IndexedPointer& Comparand)
	{
		TestCriticalStall();
		return UInt64(PlatformAtomics::InterlockedCompareExchange((volatile Int64*)&Ptrs, Exchange.Ptrs, Comparand.Ptrs)) == Comparand.Ptrs;
	}

	FORCEINLINE bool operator==(const IndexedPointer& Other) const
	{
		return Ptrs == Other.Ptrs;
	}

	FORCEINLINE bool operator!=(const IndexedPointer& Other) const
	{
		return Ptrs != Other.Ptrs;
	}

private:
	UInt64 Ptrs;

} GCC_ALIGN(8);

struct IndexedLockFreeLink
{
	IndexedPointer DoubleNext;
	void *Payload;
	UInt32 SingleNext;
};

// there is a version of this code that uses 128 bit atomics to avoid the indirection, that is why we have this policy class at all.
struct LockFreeLinkPolicy
{
	enum
	{
		MAX_BITS_IN_TLinkPtr = MAX_LOCK_FREE_LINKS_AS_BITS
	};
	typedef IndexedPointer DoublePtr;
	typedef IndexedLockFreeLink Link;
	typedef UInt32 LinkPtr;
	typedef LockFreeAllocOnceIndexedAllocator<IndexedLockFreeLink, MAX_LOCK_FREE_LINKS, 16384> TAllocator;

	static FORCEINLINE IndexedLockFreeLink* DerefLink(UInt32 Ptr)
	{
		return LinkAllocator.GetItem(Ptr);
	}

	static FORCEINLINE IndexedLockFreeLink* IndexToLink(UInt32 Index)
	{
		return LinkAllocator.GetItem(Index);
	}

	static FORCEINLINE UInt32 IndexToPtr(UInt32 Index)
	{
		return Index;
	}

	FlagGG_API static LinkPtr AllocLockFreeLink() TSAN_SAFE;
	FlagGG_API static void FreeLockFreeLink(LinkPtr Item);
	FlagGG_API static void GlobalLockFreeAlloctorInit();
	FlagGG_API static void GlobalLockFreeAlloctorUninit();
	FlagGG_API static TAllocator LinkAllocator;
};

template<int TPaddingForCacheContention, UInt64 TABAInc = 1>
class LockFreePointerListLIFORoot : public Noncopyable
{
	typedef LockFreeLinkPolicy::DoublePtr DoublePtr;
	typedef LockFreeLinkPolicy::Link Link;
	typedef LockFreeLinkPolicy::LinkPtr LinkPtr;

public:
	FORCEINLINE LockFreePointerListLIFORoot()
	{
		// We want to make sure we have quite a lot of extra counter values to avoid the ABA problem. This could probably be relaxed, but eventually it will be dangerous. 
		// The question is "how many queue operations can a thread starve for".
		static_assert(MAX_TagBitsValue / TABAInc >= (1 << 23), "risk of ABA problem");
		static_assert((TABAInc & (TABAInc - 1)) == 0, "must be power of two");
		Reset();
	}

	void Reset()
	{
		Head.Init();
	}

	void Push(LinkPtr Item) TSAN_SAFE
	{
		while (true)
		{
			DoublePtr LocalHead;
			LocalHead.AtomicRead(Head);
			DoublePtr NewHead;
			NewHead.AdvanceCounterAndState(LocalHead, TABAInc);
			NewHead.SetPtr(Item);
			LockFreeLinkPolicy::DerefLink(Item)->SingleNext = LocalHead.GetPtr();
			if (Head.InterlockedCompareExchange(NewHead, LocalHead))
			{
				break;
			}
		}
	}

	bool PushIf(FunctionRef<LinkPtr(UInt64)> AllocateIfOkToPush) TSAN_SAFE
	{
		static_assert(TABAInc > 1, "method should not be used for lists without state");
		while (true)
		{
			DoublePtr LocalHead;
			LocalHead.AtomicRead(Head);
			UInt64 LocalState = LocalHead.GetState<TABAInc>();
			LinkPtr Item = AllocateIfOkToPush(LocalState);
			if (!Item)
			{
				return false;
			}

			DoublePtr NewHead;
			NewHead.AdvanceCounterAndState(LocalHead, TABAInc);
			LockFreeLinkPolicy::DerefLink(Item)->SingleNext = LocalHead.GetPtr();
			NewHead.SetPtr(Item);
			if (Head.InterlockedCompareExchange(NewHead, LocalHead))
			{
				break;
			}
		}
		return true;
	}


	LinkPtr Pop() TSAN_SAFE
	{
		LinkPtr Item = 0;
		while (true)
		{
			DoublePtr LocalHead;
			LocalHead.AtomicRead(Head);
			Item = LocalHead.GetPtr();
			if (!Item)
			{
				break;
			}
			DoublePtr NewHead;
			NewHead.AdvanceCounterAndState(LocalHead, TABAInc);
			Link* ItemP = LockFreeLinkPolicy::DerefLink(Item);
			NewHead.SetPtr(ItemP->SingleNext);
			if (Head.InterlockedCompareExchange(NewHead, LocalHead))
			{
				ItemP->SingleNext = 0;
				break;
			}
		}
		return Item;
	}

	LinkPtr PopAll() TSAN_SAFE
	{
		LinkPtr Item = 0;
		while (true)
		{
			DoublePtr LocalHead;
			LocalHead.AtomicRead(Head);
			Item = LocalHead.GetPtr();
			if (!Item)
			{
				break;
			}
			DoublePtr NewHead;
			NewHead.AdvanceCounterAndState(LocalHead, TABAInc);
			NewHead.SetPtr(0);
			if (Head.InterlockedCompareExchange(NewHead, LocalHead))
			{
				break;
			}
		}
		return Item;
	}

	LinkPtr PopAllAndChangeState(FunctionRef<UInt64(UInt64)> StateChange) TSAN_SAFE
	{
		static_assert(TABAInc > 1, "method should not be used for lists without state");
		LinkPtr Item = 0;
		while (true)
		{
			DoublePtr LocalHead;
			LocalHead.AtomicRead(Head);
			Item = LocalHead.GetPtr();
			DoublePtr NewHead;
			NewHead.AdvanceCounterAndState(LocalHead, TABAInc);
			NewHead.SetState<TABAInc>(StateChange(LocalHead.GetState<TABAInc>()));
			NewHead.SetPtr(0);
			if (Head.InterlockedCompareExchange(NewHead, LocalHead))
			{
				break;
			}
		}
		return Item;
	}

	FORCEINLINE bool IsEmpty() const
	{
		return !Head.GetPtr();
	}

	FORCEINLINE UInt64 GetState() const
	{
		DoublePtr LocalHead;
		LocalHead.AtomicRead(Head);
		return LocalHead.GetState<TABAInc>();
	}

private:

	PaddingForCacheContention<TPaddingForCacheContention> PadToAvoidContention1;
	DoublePtr Head;
	PaddingForCacheContention<TPaddingForCacheContention> PadToAvoidContention2;
};

template<class T, int TPaddingForCacheContention, UInt64 TABAInc = 1>
class LockFreePointerListLIFOBase : public Noncopyable
{
	typedef LockFreeLinkPolicy::DoublePtr DoublePtr;
	typedef LockFreeLinkPolicy::Link Link;
	typedef LockFreeLinkPolicy::LinkPtr LinkPtr;
public:
	void Reset()
	{
		RootList.Reset();
	}

	void Push(T* InPayload) TSAN_SAFE
	{
		LinkPtr Item = LockFreeLinkPolicy::AllocLockFreeLink();
		LockFreeLinkPolicy::DerefLink(Item)->Payload = InPayload;
		RootList.Push(Item);
	}

	bool PushIf(T* InPayload, FunctionRef<bool(UInt64)> OkToPush) TSAN_SAFE
	{
		LinkPtr Item = 0;

		auto AllocateIfOkToPush = [&OkToPush, InPayload, &Item](UInt64 State)->LinkPtr
		{
			if (OkToPush(State))
			{
				if (!Item)
				{
					Item = LockFreeLinkPolicy::AllocLockFreeLink();
					LockFreeLinkPolicy::DerefLink(Item)->Payload = InPayload;
				}
				return Item;
			}
			return 0;
		};
		if (!RootList.PushIf(AllocateIfOkToPush))
		{
			if (Item)
			{
				// we allocated the link, but it turned out that the list was closed
				LockFreeLinkPolicy::FreeLockFreeLink(Item);
			}
			return false;
		}
		return true;
	}


	T* Pop() TSAN_SAFE
	{
		LinkPtr Item = RootList.Pop();
		T* Result = nullptr;
		if (Item)
		{
			Result = (T*)LockFreeLinkPolicy::DerefLink(Item)->Payload;
			LockFreeLinkPolicy::FreeLockFreeLink(Item);
		}
		return Result;
	}

	void PopAll(Vector<T*>& OutArray) TSAN_SAFE
	{
		LinkPtr Links = RootList.PopAll();
		while (Links)
		{
			Link* LinksP = LockFreeLinkPolicy::DerefLink(Links);
			OutArray.Push((T*)LinksP->Payload);
			LinkPtr Del = Links;
			Links = LinksP->SingleNext;
			LockFreeLinkPolicy::FreeLockFreeLink(Del);
		}
	}

	void PopAllAndChangeState(Vector<T*>& OutArray, FunctionRef<UInt64(UInt64)> StateChange) TSAN_SAFE
	{
		LinkPtr Links = RootList.PopAllAndChangeState(StateChange);
		while (Links)
		{
			Link* LinksP = LockFreeLinkPolicy::DerefLink(Links);
			OutArray.Push((T*)LinksP->Payload);
			LinkPtr Del = Links;
			Links = LinksP->SingleNext;
			LockFreeLinkPolicy::FreeLockFreeLink(Del);
		}
	}

	FORCEINLINE bool IsEmpty() const
	{
		return RootList.IsEmpty();
	}

	FORCEINLINE UInt64 GetState() const
	{
		return RootList.GetState();
	}

private:

	LockFreePointerListLIFORoot<TPaddingForCacheContention, TABAInc> RootList;
};

template<class T, int TPaddingForCacheContention, UInt64 TABAInc = 1>
class LockFreePointerFIFOBase : public Noncopyable
{
	typedef LockFreeLinkPolicy::DoublePtr DoublePtr;
	typedef LockFreeLinkPolicy::Link Link;
	typedef LockFreeLinkPolicy::LinkPtr LinkPtr;
public:

	FORCEINLINE LockFreePointerFIFOBase()
	{
		// We want to make sure we have quite a lot of extra counter values to avoid the ABA problem. This could probably be relaxed, but eventually it will be dangerous. 
		// The question is "how many queue operations can a thread starve for".
		static_assert(TABAInc <= 65536, "risk of ABA problem");
		static_assert((TABAInc & (TABAInc - 1)) == 0, "must be power of two");

		Head.Init();
		Tail.Init();
		LinkPtr Stub = LockFreeLinkPolicy::AllocLockFreeLink();
		Head.SetPtr(Stub);
		Tail.SetPtr(Stub);
	}

	void Push(T* InPayload) TSAN_SAFE
	{
		LinkPtr Item = LockFreeLinkPolicy::AllocLockFreeLink();
		LockFreeLinkPolicy::DerefLink(Item)->Payload = InPayload;
		DoublePtr LocalTail;
		while (true)
		{
			LocalTail.AtomicRead(Tail);
			Link* LocalTailP = LockFreeLinkPolicy::DerefLink(LocalTail.GetPtr());
			DoublePtr LocalNext;
			LocalNext.AtomicRead(LocalTailP->DoubleNext);
			DoublePtr TestLocalTail;
			TestLocalTail.AtomicRead(Tail);
			if (TestLocalTail == LocalTail)
			{
				if (LocalNext.GetPtr())
				{
					TestCriticalStall();
					DoublePtr NewTail;
					NewTail.AdvanceCounterAndState(LocalTail, TABAInc);
					NewTail.SetPtr(LocalNext.GetPtr());
					Tail.InterlockedCompareExchange(NewTail, LocalTail);
				}
				else
				{
					TestCriticalStall();
					DoublePtr NewNext;
					NewNext.AdvanceCounterAndState(LocalNext, TABAInc);
					NewNext.SetPtr(Item);
					if (LocalTailP->DoubleNext.InterlockedCompareExchange(NewNext, LocalNext))
					{
						break;
					}
				}
			}
		}
		{
			TestCriticalStall();
			DoublePtr NewTail;
			NewTail.AdvanceCounterAndState(LocalTail, TABAInc);
			NewTail.SetPtr(Item);
			Tail.InterlockedCompareExchange(NewTail, LocalTail);
		}
	}

	T* Pop() TSAN_SAFE
	{
		T* Result = nullptr;
		DoublePtr LocalHead;
		while (true)
		{
			LocalHead.AtomicRead(Head);
			DoublePtr LocalTail;
			LocalTail.AtomicRead(Tail);
			DoublePtr LocalNext;
			LocalNext.AtomicRead(LockFreeLinkPolicy::DerefLink(LocalHead.GetPtr())->DoubleNext);
			DoublePtr LocalHeadTest;
			LocalHeadTest.AtomicRead(Head);
			if (LocalHead == LocalHeadTest)
			{
				if (LocalHead.GetPtr() == LocalTail.GetPtr())
				{
					if (!LocalNext.GetPtr())
					{
						return nullptr;
					}
					TestCriticalStall();
					DoublePtr NewTail;
					NewTail.AdvanceCounterAndState(LocalTail, TABAInc);
					NewTail.SetPtr(LocalNext.GetPtr());
					Tail.InterlockedCompareExchange(NewTail, LocalTail);
				}
				else
				{
					TestCriticalStall();
					Result = (T*)LockFreeLinkPolicy::DerefLink(LocalNext.GetPtr())->Payload;
					DoublePtr NewHead;
					NewHead.AdvanceCounterAndState(LocalHead, TABAInc);
					NewHead.SetPtr(LocalNext.GetPtr());
					if (Head.InterlockedCompareExchange(NewHead, LocalHead))
					{
						break;
					}
				}
			}
		}
		LockFreeLinkPolicy::FreeLockFreeLink(LocalHead.GetPtr());
		return Result;
	}

	void PopAll(Vector<T*>& OutArray)
	{
		while (T* Item = Pop())
		{
			OutArray.Push(Item);
		}
	}

	FORCEINLINE bool IsEmpty() const
	{
		DoublePtr LocalHead;
		LocalHead.AtomicRead(Head);
		DoublePtr LocalNext;
		LocalNext.AtomicRead(LockFreeLinkPolicy::DerefLink(LocalHead.GetPtr())->DoubleNext);
		return !LocalNext.GetPtr();
	}

private:
	PaddingForCacheContention<TPaddingForCacheContention> PadToAvoidContention1;
	DoublePtr Head;
	PaddingForCacheContention<TPaddingForCacheContention> PadToAvoidContention2;
	DoublePtr Tail;
	PaddingForCacheContention<TPaddingForCacheContention> PadToAvoidContention3;
};


template<class T, Int32 TPaddingForCacheContention, Int32 NumPriorities>
class StallingTaskQueue : public Noncopyable
{
	typedef LockFreeLinkPolicy::DoublePtr DoublePtr;
	typedef LockFreeLinkPolicy::Link Link;
	typedef LockFreeLinkPolicy::LinkPtr LinkPtr;

public:
	StallingTaskQueue()
	{
		MasterState.Init();
	}

	Int32 Push(T* InPayload, UInt32 Priority)
	{
		checkLockFreePointerList(Priority < NumPriorities);
		DoublePtr LocalMasterState;
		LocalMasterState.AtomicRead(MasterState);
		PriorityQueues[Priority].Push(InPayload);
		DoublePtr NewMasterState;
		NewMasterState.AdvanceCounterAndState(LocalMasterState, 1);
		Int32 ThreadToWake = FindThreadToWake(LocalMasterState.GetPtr());
		if (ThreadToWake >= 0)
		{
			NewMasterState.SetPtr(TurnOffBit(LocalMasterState.GetPtr(), ThreadToWake));
		}
		else
		{
			NewMasterState.SetPtr(LocalMasterState.GetPtr());
		}
		while (!MasterState.InterlockedCompareExchange(NewMasterState, LocalMasterState))
		{
			LocalMasterState.AtomicRead(MasterState);
			NewMasterState.AdvanceCounterAndState(LocalMasterState, 1);
			ThreadToWake = FindThreadToWake(LocalMasterState.GetPtr());
#if 0
			// This block of code is supposed to avoid starting the task thread if the queues are empty.
			// There WAS a silly bug here. In rare cases no task thread is woken up.
			// That bug has been fixed, but I don't think we really need this code anyway.
			// Without this block, it is possible that we do a redundant wake-up, but for task threads, that can happen anyway. 
			// For named threads, the rare redundant wakeup seems acceptable.
			if (ThreadToWake >= 0)
			{
				bool bAny = false;
				for (Int32 Index = 0; !bAny && Index < NumPriorities; Index++)
				{
					bAny = !PriorityQueues[Index].IsEmpty();
				}
				if (!bAny) // if there is nothing in the queues, then don't wake anyone
				{
					ThreadToWake = -1;
				}
			}
#endif
			if (ThreadToWake >= 0)
			{
				NewMasterState.SetPtr(TurnOffBit(LocalMasterState.GetPtr(), ThreadToWake));
			}
			else
			{
				NewMasterState.SetPtr(LocalMasterState.GetPtr());
			}
		}
		return ThreadToWake;
	}

	T* Pop(Int32 MyThread, bool bAllowStall, UInt32 Priority)
	{
		checkLockFreePointerList(MyThread >= 0 && MyThread < LockFreeLinkPolicy::MAX_BITS_IN_TLinkPtr);

		while (true)
		{
			DoublePtr LocalMasterState;
			LocalMasterState.AtomicRead(MasterState);
			//checkLockFreePointerList(!TestBit(LocalMasterState.GetPtr(), MyThread) || !FPlatformProcess::SupportsMultithreading()); // you should not be stalled if you are asking for a task
			UInt32 CurrentNumPriorities = Max<UInt32>(Priority, NumPriorities);
			for (Int32 Index = 0; Index < CurrentNumPriorities; Index++)
			{
				T *Result = PriorityQueues[Index].Pop();
				if (Result)
				{
					while (true)
					{
						DoublePtr NewMasterState;
						NewMasterState.AdvanceCounterAndState(LocalMasterState, 1);
						NewMasterState.SetPtr(LocalMasterState.GetPtr());
						if (MasterState.InterlockedCompareExchange(NewMasterState, LocalMasterState))
						{
							return Result;
						}
						LocalMasterState.AtomicRead(MasterState);
						checkLockFreePointerList(!TestBit(LocalMasterState.GetPtr(), MyThread)); // you should not be stalled if you are asking for a task
					}
				}
			}
			if (!bAllowStall)
			{
				break; // if we aren't stalling, we are done, the queues are empty
			}
			{
				DoublePtr NewMasterState;
				NewMasterState.AdvanceCounterAndState(LocalMasterState, 1);
				NewMasterState.SetPtr(TurnOnBit(LocalMasterState.GetPtr(), MyThread));
				if (MasterState.InterlockedCompareExchange(NewMasterState, LocalMasterState))
				{
					break;
				}
			}
		}
		return nullptr;
	}

	bool IsEmpty()
	{
		for (Int32 Index = 0; Index < NumPriorities; ++Index)
		{
			if (!PriorityQueues[Index].IsEmpty())
				return false;
		}
		return true;
	}

private:
	static Int32 FindThreadToWake(LinkPtr Ptr)
	{
		Int32 Result = -1;
		UPtrInt Test = UPtrInt(Ptr);
		if (Test)
		{
			Result = 0;
			while (!(Test & 1))
			{
				Test >>= 1;
				Result++;
			}
		}
		return Result;
	}

	static LinkPtr TurnOffBit(LinkPtr Ptr, Int32 BitToTurnOff)
	{
		return (LinkPtr)(UPtrInt(Ptr) & ~(UPtrInt(1) << BitToTurnOff));
	}

	static LinkPtr TurnOnBit(LinkPtr Ptr, Int32 BitToTurnOn)
	{
		return (LinkPtr)(UPtrInt(Ptr) | (UPtrInt(1) << BitToTurnOn));
	}

	static bool TestBit(LinkPtr Ptr, Int32 BitToTest)
	{
		return !!(UPtrInt(Ptr) & (UPtrInt(1) << BitToTest));
	}

	LockFreePointerFIFOBase<T, TPaddingForCacheContention> PriorityQueues[NumPriorities];
	// not a pointer to anything, rather tracks the stall state of all threads servicing this queue.
	DoublePtr MasterState;
	PaddingForCacheContention<TPaddingForCacheContention> PadToAvoidContention1;
};


template<class T, int TPaddingForCacheContention>
class LockFreePointerListLIFOPad : private LockFreePointerListLIFOBase<T, TPaddingForCacheContention>
{
public:

	/**
	*	Push an item onto the head of the list.
	*
	*	@param NewItem, the new item to push on the list, cannot be NULL.
	*/
	void Push(T *NewItem)
	{
		LockFreePointerListLIFOBase<T, TPaddingForCacheContention>::Push(NewItem);
	}

	/**
	*	Pop an item from the list or return NULL if the list is empty.
	*	@return The popped item, if any.
	*/
	T* Pop()
	{
		return LockFreePointerListLIFOBase<T, TPaddingForCacheContention>::Pop();
	}

	/**
	*	Pop all items from the list.
	*
	*	@param Output The array to hold the returned items. Must be empty.
	*/
	void PopAll(Vector<T *>& Output)
	{
		LockFreePointerListLIFOBase<T, TPaddingForCacheContention>::PopAll(Output);
	}

	/**
	*	Check if the list is empty.
	*
	*	@return true if the list is empty.
	*	CAUTION: This methods safety depends on external assumptions. For example, if another thread could add to the list at any time, the return value is no better than a best guess.
	*	As typically used, the list is not being access concurrently when this is called.
	*/
	FORCEINLINE bool IsEmpty() const
	{
		return LockFreePointerListLIFOBase<T, TPaddingForCacheContention>::IsEmpty();
	}
};

template<class T>
class LockFreePointerListLIFO : public LockFreePointerListLIFOPad<T, 0>
{

};

template<class T, int TPaddingForCacheContention>
class LockFreePointerListUnordered : public LockFreePointerListLIFOPad<T, TPaddingForCacheContention>
{

};

template<class T, int TPaddingForCacheContention>
class LockFreePointerListFIFO : private LockFreePointerFIFOBase<T, TPaddingForCacheContention>
{
public:
	/**
	*	Push an item onto the head of the list.
	*
	*	@param NewItem, the new item to push on the list, cannot be NULL.
	*/
	void Push(T *NewItem)
	{
		LockFreePointerFIFOBase<T, TPaddingForCacheContention>::Push(NewItem);
	}

	/**
	*	Pop an item from the list or return NULL if the list is empty.
	*	@return The popped item, if any.
	*/
	T* Pop()
	{
		return LockFreePointerFIFOBase<T, TPaddingForCacheContention>::Pop();
	}

	/**
	*	Pop all items from the list.
	*
	*	@param Output The array to hold the returned items. Must be empty.
	*/
	void PopAll(Vector<T *>& Output)
	{
		LockFreePointerFIFOBase<T, TPaddingForCacheContention>::PopAll(Output);
	}

	/**
	*	Check if the list is empty.
	*
	*	@return true if the list is empty.
	*	CAUTION: This methods safety depends on external assumptions. For example, if another thread could add to the list at any time, the return value is no better than a best guess.
	*	As typically used, the list is not being access concurrently when this is called.
	*/
	FORCEINLINE bool IsEmpty() const
	{
		return LockFreePointerFIFOBase<T, TPaddingForCacheContention>::IsEmpty();
	}
};


template<class T, int TPaddingForCacheContention>
class ClosableLockFreePointerListUnorderedSingleConsumer : private LockFreePointerListLIFOBase<T, TPaddingForCacheContention, 2>
{
public:
	/**
	*	Reset the list to the initial state. Not thread safe, but used for recycling when we know all users are gone.
	*/
	void Reset()
	{
		LockFreePointerListLIFOBase<T, TPaddingForCacheContention, 2>::Reset();
	}

	/**
	*	Push an item onto the head of the list, unless the list is closed
	*
	*	@param NewItem, the new item to push on the list, cannot be NULL
	*	@return true if the item was pushed on the list, false if the list was closed.
	*/
	bool PushIfNotClosed(T *NewItem)
	{
		return LockFreePointerListLIFOBase<T, TPaddingForCacheContention, 2>::PushIf(NewItem, [](UInt64 State)->bool {return !(State & 1); });
	}

	/**
	*	Pop all items from the list and atomically close it.
	*
	*	@param Output The array to hold the returned items. Must be empty.
	*/
	void PopAllAndClose(Vector<T *>& Output)
	{
		auto CheckOpenAndClose = [](UInt64 State) -> UInt64
		{
			checkLockFreePointerList(!(State & 1));
			return State | 1;
		};
		LockFreePointerListLIFOBase<T, TPaddingForCacheContention, 2>::PopAllAndChangeState(Output, CheckOpenAndClose);
	}

	/**
	*	Check if the list is closed
	*
	*	@return true if the list is closed.
	*/
	bool IsClosed() const
	{
		return !!(LockFreePointerListLIFOBase<T, TPaddingForCacheContention, 2>::GetState() & 1);
	}

};

}
