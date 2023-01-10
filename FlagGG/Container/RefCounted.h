#pragma once

#include "Export.h"
#include "Container/ThreadSafeCounter.h"

namespace FlagGG
{

/// Reference count base class
struct FlagGG_API IRefCount
{
	virtual ~IRefCount() {}

	/// Increment reference count
	virtual Int32 AddRef() = 0;
	/// Decrement reference count
	virtual Int32 ReleaseRef() = 0;
	/// Increment weak reference count
	virtual Int32 AddWeakRef() = 0;
	/// Decrement weak reference count
	virtual Int32 ReleaseWeakRef() = 0;
	/// Return reference count.
	virtual Int32 Refs() const = 0;
	/// Return weak reference count.
	virtual Int32 WeakRefs() const = 0;
};

/// Reference count structure.
struct FlagGG_API RefCount : public IRefCount
{
	/// Construct.
	RefCount() :
	refs_(0),
	weakRefs_(0)
	{
	}

	/// Destruct.
	~RefCount() override
	{
		// Set reference counts below zero to fire asserts if this object is still accessed
		refs_ = -1;
		weakRefs_ = -1;
	}

	/// Increment reference count
	Int32 AddRef() override { return ++refs_; }
	/// Decrement reference count
	Int32 ReleaseRef() override { return --refs_; }
	/// Increment weak reference count
	Int32 AddWeakRef() override { return ++weakRefs_; }
	/// Decrement weak reference count
	Int32 ReleaseWeakRef() override { return --weakRefs_; }
	/// Return reference count.
	int Refs() const override { return refs_; }
	/// Return weak reference count.
	int WeakRefs() const override { return weakRefs_; }

	/// Reference count. If below zero, the object has been destroyed.
	Int32 refs_;
	/// Weak reference count.
	Int32 weakRefs_;
};

/// Base class for intrusively reference-counted objects. These are noncopyable and non-assignable.
class FlagGG_API RefCounted
{
public:
	/// Construct. Allocate the reference count structure and set an initial self weak reference.
	RefCounted();
	/// Destruct. Mark as expired and also delete the reference count structure if no outside weak references exist.
	virtual ~RefCounted();

	/// Prevent copy construction.
	RefCounted(const RefCounted& rhs) = delete;
	/// Prevent assignment.
	RefCounted& operator =(const RefCounted& rhs) = delete;

	/// Increment reference count. Can also be called outside of a SharedPtr for traditional reference counting.
	void AddRef();
	/// Decrement reference count and delete self if no more references. Can also be called outside of a SharedPtr for traditional reference counting.
	void ReleaseRef();
	/// Return reference count.
	int Refs() const;
	/// Return weak reference count.
	int WeakRefs() const;

	/// Return pointer to the reference count structure.
	RefCount* RefCountPtr() { return refCount_; }

private:
	/// Pointer to the reference count structure.
	RefCount* refCount_;
};

/// Reference count structure.
struct FlagGG_API FThreadSafeRefCount : public IRefCount
{
	/// Construct.
	FThreadSafeRefCount() :
		refs_(0),
		weakRefs_(0)
	{
	}

	/// Destruct.
	~FThreadSafeRefCount() override
	{
		// Set reference counts below zero to fire asserts if this object is still accessed
		refs_.Set(-1);
		weakRefs_.Set(-1);
	}

	/// Increment reference count
	Int32 AddRef() override { return refs_.Increment(); }
	/// Decrement reference count
	Int32 ReleaseRef() override { return refs_.Decrement(); }
	/// Increment weak reference count
	Int32 AddWeakRef() override { return weakRefs_.Increment(); }
	/// Decrement weak reference count
	Int32 ReleaseWeakRef() override { return weakRefs_.Decrement(); }
	/// Return reference count.
	int Refs() const override { return refs_.GetValue(); }
	/// Return weak reference count.
	int WeakRefs() const override { return weakRefs_.GetValue(); }

	/// Reference count. If below zero, the object has been destroyed.
	ThreadSafeCounter refs_;
	/// Weak reference count.
	ThreadSafeCounter weakRefs_;
};

/// Base class for intrusively reference-counted objects. These are noncopyable and non-assignable.
class FlagGG_API ThreadSafeRefCounted
{
public:
	/// Construct. Allocate the reference count structure and set an initial self weak reference.
	ThreadSafeRefCounted();
	/// Destruct. Mark as expired and also delete the reference count structure if no outside weak references exist.
	virtual ~ThreadSafeRefCounted();

	/// Prevent copy construction.
	ThreadSafeRefCounted(const ThreadSafeRefCounted& rhs) = delete;
	/// Prevent assignment.
	ThreadSafeRefCounted& operator =(const ThreadSafeRefCounted& rhs) = delete;

	/// Increment reference count. Can also be called outside of a SharedPtr for traditional reference counting.
	void AddRef();
	/// Decrement reference count and delete self if no more references. Can also be called outside of a SharedPtr for traditional reference counting.
	void ReleaseRef();
	/// Return reference count.
	int Refs() const;
	/// Return weak reference count.
	int WeakRefs() const;

	/// Return pointer to the reference count structure.
	FThreadSafeRefCount* RefCountPtr() { return refCount_; }

private:
	/// Pointer to the reference count structure.
	FThreadSafeRefCount* refCount_;
};

}
