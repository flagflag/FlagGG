#ifndef _THREADSAFEDEQUE_H_
#define _THREADSAFEDEQUE_H_

#include "AsyncFrame/LockFree/ThreadFreeList.h"

namespace FlagGG {

/// Fast, lock-free double-ended queue for concurrent access.
///
/// @param T Type of list elements; must have default contructor.
template< typename T >
class ThreadSafeDeque
{
   // Lock-free deques using just single-word atomic writes are
   // very tricky as each pointer update must immediately result
   // in a fully valid list state.  The idea here is to maintain the
   // deque's head and tail pointers unreliably but otherwise keep a
   // regular double-linked list (since we never insert nodes in the
   // middle, single-word writes are all we need).
   //
   // Deletions are a bit less straightforward and require the threads
   // to work cooperatively.  Since failure of a pointer update depends
   // on the deletion state, the deletion flag has to be encoded into
   // the link fields.  However, as there are two link fields this creates
   // two independent deletion flags for each single node, one on the
   // next link and one on the prev link.
   //
   // This will not lead to a problem, though, as it only becomes relevant
   // when there is only a single value in the list which, even if the
   // respective node gets both deleted and appended/prepended a new node,
   // will result in a valid list state.

   public:

      typedef T ValueType;

   protected:

      class Node;
      class DeleteNode;
      typedef ThreadSafeRef< Node > NodeRef;

      /// List node.
      class Node : public ThreadSafeFreeListNode< Node, DeleteNode >
      {
         public:

            friend class DeleteNode; // freeList_;
            typedef typename ThreadSafeDeque< T >::ValueType ValueType;

            /// Thread claim flag.  This is to prevent two threads who concurrently
            /// do a TryPopFront() and TryPopBack() respectively on a deque with just
            /// a single node to both claim and return the same value (which would happen
            /// without the flag as otherwise both threads would use two different
            /// deletion bits for claiming the node).
            UInt32 isClaimed_;

            /// Link to the freelist that the node has been
            /// allocated from.
            ThreadSafeFreeList< Node >& freeList_;

            /// Value contained in the node.
            ValueType value_;

            /// Reference to next node and deletion bit.
            NodeRef next_;

            /// Reference to previous node and deletion bit.
            NodeRef prev_;

            /// Construct an unlinked node allocated from "freeList".
            Node( ThreadSafeFreeList< Node >& freeList, const ValueType& value )
               : isClaimed_( 0 ), freeList_( freeList ), value_( value ) {}
      };

      class DeleteNode
      {
         public:
            template< typename N >
            static void Destroy( N* ptr )
            {
               ASSERT_MESSAGE( ptr->isClaimed_!=0,
                  "ThreadSafeDeque::DeleteNode::destroy() - deleting unclaimed node" );
               DestructInPlace( ptr );
               ptr->freeList_.Free( ptr );
            }
      };

      #ifdef _DEBUG
      Int32 numValues_;
      #endif

      /// Reference to the head node.
      NodeRef head_;

      ///
      NodeRef tail_;

      /// Free list for list nodes.
      ThreadSafeFreeList< Node > freeList_;

      /// @return the leftmost node in the list.
      /// @note Updates the list state and may purge deleted nodes.
      NodeRef GetHead();

      /// @return the rightmost node in the list.
      /// @note Updates the list state and may purge deleted nodes.
      NodeRef GetTail();

   public:

      /// Construct an empty deque.
      ThreadSafeDeque()
      {
         #ifdef _DEBUG
         numValues_ = 0;
         #endif
      }

      ~ThreadSafeDeque()
      {
         ValueType value;
         while( TryPopFront( value ) );
         ASSERT_MESSAGE( IsEmpty(), "ThreadSafeDeque::~ThreadSafeDeque() - not empty" );
      }

      /// @return true if the queue is empty.
      bool IsEmpty()
      {
         return ( !GetHead() && !GetTail() );
      }

      /// Prepend the given value to the list.
      void PushFront( const ValueType& value );

      /// Append the given value to the list.
      void PushBack( const ValueType& value );

      /// Try to take the leftmost value from the deque.
      /// Fails if the deque is empty at the time the method tries to
      /// take a node from the list.
      bool TryPopFront( ValueType& outValue );

      /// Try to take the rightmost value from the deque.
      /// Fails if the deque is empty at the time the method tries to
      /// take a node from the list.
      bool TryPopBack( ValueType& outValue );

      void DumpDebug()
      {
         #ifdef _DEBUG
         log_out( MAIN, LOG_DEBUG, "[ThreadSafeDeque] numValues=%i", numValues_ );
         freeList_.DumpDebug();
         #endif
      }

	  void ClearCache()
	  {
#ifdef _DEBUG
		  ASSERT_MESSAGE( IsEmpty(),
			  "ThreadSafeDeque::ClearCache() - not empty" );
#endif
		 freeList_.Clear();
	  }
};

// The GetHead() and GetTail() code here is pretty much brute-force in order
// to keep the complexities of synchronizing it bounded.  We just let each
// thread work as if it is the only thread but require each one to start from
// scratch on each iteration.

template< typename T >
typename ThreadSafeDeque< T >::NodeRef ThreadSafeDeque< T >::GetHead()
{
   // Find leftmost node.

   NodeRef result;
   while( 1 )
   {
      // Iterate through to leftmost node.
      
      {
         NodeRef head = head_;
         while( head != NULL )
         {
            NodeRef prev = head->prev_;
            if( prev != NULL )
               head_.TrySetFromTo( head, prev, NodeRef::TAG_Unset );
            else
               break;
               
            head = head_;
         }
      }
      
      // Clear out dead nodes at front of list.
      
      {
         NodeRef head = head_;
         if( head && head->prev_.IsTagged() )
         {
            NodeRef next = head->next_;
            
            head_.TrySetFromTo( head, next, NodeRef::TAG_Unset );
            tail_.TrySetFromTo( head, next, NodeRef::TAG_Unset );

            if( next != NULL )
               next->prev_.TrySetFromTo( head, NULL );

            head->next_.TrySetFromTo( next, NULL, NodeRef::TAG_Set );
            
            continue; // Restart.
         }
      }
      
      // Try head.
      
      NodeRef head = head_;
      if( head != NULL && !head->prev_.IsTagged() )
      {
         result = head;
         break;
      }
         
      // Try tail.
      
      if( !head )
      {
         head = tail_;
         if( !head )
            break;
      }

      // Update head.
      
      NodeRef prev = head->prev_;
      if( head->prev_ != NULL )
      {
         if( !head_.TrySetFromTo( head, prev, NodeRef::TAG_Unset ) )
            head_.TrySetFromTo( NULL, prev );
      }
      else
         head_.TrySetFromTo( NULL, head );
   }

   ASSERT_MESSAGE( !result.IsTagged(), "ThreadSafeDeque::GetHead() - head got tagged" );
   return result;
}

template< typename T >
typename ThreadSafeDeque< T >::NodeRef ThreadSafeDeque< T >::GetTail()
{
   // Find rightmost node.

   NodeRef result;
   while( 1 )
   {
      // Iterate through to rightmost node.
      
      {
         NodeRef tail = tail_;
         while( tail != NULL )
         {
            NodeRef next = tail->next_;
            if( next != NULL )
               tail_.TrySetFromTo( tail, next, NodeRef::TAG_Unset );
            else
               break;
               
            tail = tail_;
         }
      }
      
      // Clear out dead nodes at tail of list.
      
      {
         NodeRef tail = tail_;
         if( tail != NULL && tail->next_.IsTagged() )
         {
            NodeRef prev = tail->prev_;
            
            head_.TrySetFromTo( tail, prev, NodeRef::TAG_Unset );
            tail_.TrySetFromTo( tail, prev, NodeRef::TAG_Unset );

            if( prev != NULL )
               prev->next_.TrySetFromTo( tail, NULL );

            tail->prev_.TrySetFromTo( prev, NULL, NodeRef::TAG_Set );
            
            continue; // Restart.
         }
      }
      
      // Try tail.
      
      NodeRef tail = tail_;
      if( tail != NULL && !tail->next_.IsTagged() )
      {
         result = tail;
         break;
      }
         
      // Try head.
      
      if( !tail )
      {
         tail = head_;
         if( !tail )
            break;
      }

      // Update tail.
      
      NodeRef next = tail->next_;
      if( next != NULL )
      {
         if( !tail_.TrySetFromTo( tail, next, NodeRef::TAG_Unset ) )
            tail_.TrySetFromTo( NULL, next );
      }
      else
         tail_.TrySetFromTo( NULL, tail );
   }

   ASSERT_MESSAGE( !result.IsTagged(), "ThreadSafeDeque::GetTail() - tail got tagged" );
   return result;
}

template< typename T >
void ThreadSafeDeque< T >::PushFront( const ValueType& value )
{
   NodeRef nextNode;
   NodeRef newNode;

   NodeRef::UnsafeWrite( newNode, new ( freeList_ ) Node( freeList_, value ) );

   while( 1 )
   {
      nextNode = GetHead();
      if( !nextNode )
      {
         newNode->next_ = NULL;
         if( head_.TrySetFromTo( NULL, newNode ) )
            break;
      }
      else if (!nextNode->next_.IsTagged())
      {
         newNode->next_ = nextNode;
         if( nextNode->prev_.TrySetFromTo( NULL, newNode, NodeRef::TAG_FailIfSet ) )
            break;
      }
   }

#ifdef _DEBUG
   dFetchAndAdd( numValues_, 1 );
#endif
}

template< typename T >
void ThreadSafeDeque< T >::PushBack( const ValueType& value )
{
   NodeRef prevNode;
   NodeRef newNode;

   NodeRef::UnsafeWrite( newNode, new ( freeList_ ) Node( freeList_, value ) );

   while( 1 )
   {
      prevNode = GetTail();
      if( !prevNode )
      {
         newNode->prev_ = NULL;
         if( head_.TrySetFromTo( NULL, newNode ) ) // use head so we synchronize with PushFront
            break;
      }
      else if (!prevNode->prev_.IsTagged())
      {
         newNode->prev_ = prevNode;
         if( prevNode->next_.TrySetFromTo( NULL, newNode, NodeRef::TAG_FailIfSet ) )
            break;
      }
   }

#ifdef _DEBUG
   dFetchAndAdd( numValues_, 1 );
#endif
}

template< typename T >
bool ThreadSafeDeque< T >::TryPopFront( ValueType& outValue )
{
   NodeRef oldHead;

   while( 1 )
   {
      oldHead = GetHead();
      if( !oldHead )
         return false;

      // Try to claim the node.

      if( oldHead->prev_.TrySetFromTo( NULL, NULL, NodeRef::TAG_SetOrFail ) )
      {
         if( dCompareAndSwap( oldHead->isClaimed_, 0, 1 ) )
            break;
         else
            continue;
      }
   }

   outValue = oldHead->value_;
   oldHead = NULL;

   // Cleanup.
   GetHead();

#ifdef _DEBUG
   dFetchAndAdd( numValues_, -1 );
#endif
   return true;
}

template< typename T >
bool ThreadSafeDeque< T >::TryPopBack( ValueType& outValue )
{
   NodeRef oldTail;

   while( 1 )
   {
      oldTail = GetTail();
      if( !oldTail )
         return false;

      // Try to claim the node.

      if( oldTail->next_.TrySetFromTo( NULL, NULL, NodeRef::TAG_SetOrFail ) )
      {
         if( dCompareAndSwap( oldTail->isClaimed_, 0, 1 ) )
            break;
      }
   }

   outValue = oldTail->value_;
   oldTail = NULL;

   // Cleanup.
   GetTail();

#ifdef _DEBUG
   dFetchAndAdd( numValues_, -1 );
#endif
   return true;
}

}

#endif // _THREADSAFEDEQUE_H_
