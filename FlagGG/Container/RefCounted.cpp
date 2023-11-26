#include "Container/RefCounted.h"
#include "Core/CryAssert.h"

namespace FlagGG
{

RefCounted::RefCounted() :
	refCount_(new RefCount())
{
	// Hold a weak ref to self to avoid possible double delete of the refcount
	(refCount_->weakRefs_)++;
}

RefCounted::~RefCounted()
{
	ASSERT(refCount_);
	ASSERT(refCount_->refs_ == 0);
	ASSERT(refCount_->weakRefs_ > 0);

	// Mark object as expired, release the self weak ref and delete the refcount if no other weak refs exist
	refCount_->refs_ = -1;
	(refCount_->weakRefs_)--;
	if (!refCount_->weakRefs_)
		delete refCount_;

	refCount_ = nullptr;
}

void RefCounted::AddRef()
{
	ASSERT(refCount_->refs_ >= 0);
	(refCount_->refs_)++;
}

void RefCounted::ReleaseRef()
{
	ASSERT(refCount_->refs_ > 0);
	(refCount_->refs_)--;
	if (!refCount_->refs_)
		delete this;
}

int RefCounted::Refs() const
{
	return refCount_->refs_;
}

int RefCounted::WeakRefs() const
{
	// Subtract one to not return the internally held reference
	return refCount_->weakRefs_ - 1;
}

ThreadSafeRefCounted::ThreadSafeRefCounted() :
	refCount_(new FThreadSafeRefCount())
{
	// Hold a weak ref to self to avoid possible double delete of the refcount
	refCount_->weakRefs_.Increment();
}

ThreadSafeRefCounted::~ThreadSafeRefCounted()
{
	ASSERT(refCount_);
	ASSERT(refCount_->refs_.GetValue() == 0);
	ASSERT(refCount_->weakRefs_.GetValue() > 0);

	// Mark object as expired, release the self weak ref and delete the refcount if no other weak refs exist
	refCount_->refs_.Set(-1);
	Int32 weakRefs = refCount_->weakRefs_.Decrement();
	if (!weakRefs)
		delete refCount_;

	refCount_ = nullptr;
}

void ThreadSafeRefCounted::AddRef()
{
	ASSERT(refCount_->refs_.GetValue() >= 0);
	refCount_->refs_.Increment();
}

void ThreadSafeRefCounted::ReleaseRef()
{
	ASSERT(refCount_->refs_.GetValue() > 0);
	Int32 refs = refCount_->refs_.Decrement();
	if (!refs)
		delete this;
}

int ThreadSafeRefCounted::Refs() const
{
	return refCount_->refs_.GetValue();
}

int ThreadSafeRefCounted::WeakRefs() const
{
	// Subtract one to not return the internally held reference
	return refCount_->weakRefs_.GetValue() - 1;
}

}
