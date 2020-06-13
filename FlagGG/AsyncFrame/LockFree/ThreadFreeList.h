#ifndef _THREADSAFEFREELIST_H_
#define _THREADSAFEFREELIST_H_

#include "AsyncFrame/LockFree/ThreadRef.h"
#include "AsyncFrame/LockFree/Intrinsics.h"

namespace FlagGG { namespace AsyncFrame { namespace LockFree {

/// @file
/// Lock-free freelists for concurrent access.


/// Freelist for re-using allocations in a concurrent setting.
///
/// @note Make sure that there are no more allocations in use
///   when the free list is destructed.
/// @note Allocated instances come with a reference already counted
///   on the instance.
///
/// @param T Type of elements to allocate; must be derived from
///   ThreadSafeRefCount and have at least define one additional
///   pointer-sized field.
template< class T >
class ThreadSafeFreeList
{
   protected:

      T* freeList_;

      #ifdef _DEBUG
      Int32 numNodesTotal_;
      Int32 numNodesFree_;
      #endif

      T*& GetNext( T* ptr )
      {
         return *( ( T** ) &( ( UInt8* ) ptr )[ sizeof( T ) - sizeof( T* ) ] );
      }

   public:

      /// Create the freelist.
      ///
      /// @param numPreAlloc Number of instances to pre-allocate.
      ThreadSafeFreeList( UInt32 numPreAlloc = 0 )
         : freeList_( 0 )
      {
         #ifdef _DEBUG
         numNodesTotal_ = 0;
         numNodesFree_ = 0;
         #endif

         for( UInt32 i = 0; i < numPreAlloc; ++ i )
            Free( Alloc() );
      }

      ~ThreadSafeFreeList()
      {
         #ifdef _DEBUG
         ASSERT_MESSAGE( numNodesTotal_ == numNodesFree_,
            "ThreadSafeFreeList::~ThreadSafeFreeList() - still got live instances" );
         #endif

         // Destroy remaining nodes.  Not synchronized.  We assume all
         // concurrent processing to have finished.

         while( freeList_ )
         {
            T* next = GetNext( freeList_ );
            free( freeList_ );
            freeList_ = next;
         }
      }

      /// Return memory for a new instance.
      void* Alloc()
      {
         T* ptr;
         while( 1 )
         {
            ptr = ThreadSafeRef< T >::SafeRead( freeList_ );
            if( !ptr )
            {
               ptr = ( T* ) malloc( sizeof( T ) );
               memset( ptr, 0, sizeof( T ) );

               #ifdef _DEBUG
               dFetchAndAdd( numNodesTotal_, 1 );
               #endif

               ptr->AddRef();
               break;
            }
            else if( dCompareAndSwap( freeList_, ptr, GetNext( ptr ) ) )
            {
               #ifdef _DEBUG
               dFetchAndAdd( numNodesFree_, -1 );
               #endif

               ptr->ClearLowestBit();
               break;
            }
            else
               ptr->Release();
         }

         return ptr;
      }

      /// Return the memory allocated to the given instance to the freelist.
      void Free( void* ptr )
      {
         ASSERT_MESSAGE( ptr!=NULL, "ThreadSafeFreeList::free() - got a NULL pointer" );
         T* node = ( T* ) ptr;

         while( 1 )
         {
            T* list = freeList_;
            GetNext( node ) = list;
            if( dCompareAndSwap( freeList_, list, node ) )
               break;
         }

         #ifdef _DEBUG
         dFetchAndAdd( numNodesFree_, 1 );
         #endif
      }

      void DumpDebug()
      {
         #ifdef _DEBUG
         log_out(MAIN, LOG_DEBUG, "[ThreadSafeFreeList] total=%i, free=%i",
            numNodesTotal_, numNodesFree_ );
         #endif
      }

	  void Clear()
	  {
		 #ifdef _DEBUG
				  ASSERT_MESSAGE( numNodesTotal_ == numNodesFree_,
					  "ThreadSafeFreeList::~ThreadSafeFreeList() - still got live instances" );
		 #endif

		  // Destroy remaining nodes.  Not synchronized.  We assume all
		  // concurrent processing to have finished.

		  while( freeList_ )
		  {
			  T* next = GetNext( freeList_ );
			  free( freeList_ );
			  freeList_ = next;
		  }
	  }
};

/// Baseclass for objects allocated from ThreadSafeFreeLists.
template< class T, class DeletePolicy = DeleteSingle >
class ThreadSafeFreeListNode : public ThreadSafeRefCount< T, DeletePolicy >
{
   public:

      typedef ThreadSafeRefCount< T, DeletePolicy > Parent;

      ThreadSafeFreeListNode()
         : Parent( false ) {}

      static void* operator new( size_t size, ThreadSafeFreeList< T >& freeList )
      {
         ASSERT_MESSAGE( size <= sizeof( T ),
            "ThreadSafeFreeListNode::new() - size exceeds limit of freelist" );
         return freeList.Alloc();
      }
      static void operator delete( void* ptr, ThreadSafeFreeList< T >& freeList )
      {
         freeList.Free( ptr );
      }
};

}}}

#endif // _THREADSAFEFREELIST_H_
