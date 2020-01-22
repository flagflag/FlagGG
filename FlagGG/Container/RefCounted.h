#pragma once

#include "Export.h"

namespace FlagGG
{
	namespace Container
	{
		/// Reference count structure.
		struct FlagGG_API RefCount
		{
			/// Construct.
			RefCount() :
			refs_(0),
			weakRefs_(0)
			{
			}

			/// Destruct.
			~RefCount()
			{
				// Set reference counts below zero to fire asserts if this object is still accessed
				refs_ = -1;
				weakRefs_ = -1;
			}

			// Overload new/delete:
			void* operator new(size_t byteSize);
			void operator delete(void* object);

			/// Reference count. If below zero, the object has been destroyed.
			int refs_;
			/// Weak reference count.
			int weakRefs_;
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
			

			// Overload new/delete:
			void* operator new(size_t byteSize);
			void* operator new[](size_t byteSize);
			void operator delete(void* object);
			void operator delete[](void* object);

		private:
			/// Pointer to the reference count structure.
			RefCount* refCount_;
		};
	}
}
