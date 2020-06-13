#ifndef _THREADSAFEPRIORITYQUEUE_H_
#define _THREADSAFEPRIORITYQUEUE_H_

#include "AsyncFrame/LockFree/ThreadRef.h"

namespace FlagGG { namespace AsyncFrame { namespace LockFree {

template< typename T, typename K = float32, bool SORT_MIN_TO_MAX = false, UInt32 MAX_LEVEL = 4, UInt32 PROBABILISTIC_BIAS = 50 >
struct ThreadSafePriorityQueue
{
   typedef T ValueType;
   typedef K KeyType;

   enum { MAX_LEVEL_CONST = MAX_LEVEL };

   ThreadSafePriorityQueue();

   bool              IsEmpty();
   void              Insert( KeyType priority, const T& value );
   bool              TakeNext( T& outValue, KeyType upToPriority = ( SORT_MIN_TO_MAX ? TypeTraits< KeyType >::MAX : TypeTraits< KeyType >::MIN ) );

protected:
   struct Node;
   typedef ThreadSafeRef< Node > NodePtr;
   friend class ThreadSafeRefCount< Node >;
   friend struct DeleteSingle;


   struct Node : public ThreadSafeRefCount< Node >
   {
      typedef ThreadSafeRefCount< Node > Parent;

      Node( KeyType priority, const ValueType& value );
      ~Node();

      KeyType        GetPriority()                 { return priority_; }
      ValueType&     GetValue()                    { return value_; }
      UInt32            GetLevel();
      NodePtr&       GetNext( UInt32 level );

      bool           IsMarkedForDeletion();
      bool           TryMarkForDeletion();
      
      void           ClearValue()                  { value_ = ValueType(); }

      static UInt32     RandomLevel();

      void*          operator new( size_t size, Int32 level = -1 );
      void           operator delete( void* ptr );

   private:
      KeyType        priority_;     ///< Priority key.
      UInt32            level_;        ///< Level count and deletion bit (highest).
      ValueType      value_;
      Node*          next_[ 1 ];    ///< Variable-sized array of next pointers.

      struct FreeList
      {
         bool        destroyed_;
         Node*       nodes_;

         ~FreeList();
      };

      static FreeList smFreeLists[ MAX_LEVEL ];
   };

   NodePtr           head_;         ///< Artificial head node.
   NodePtr           tail_;         ///< Artificial tail node.

   void              ReadNext( NodePtr& refPrev, NodePtr& refNext, UInt32 level );
   void              Scan( NodePtr& refPrev, NodePtr& refNext, UInt32 level, KeyType priority );
   void              ScanFromHead( NodePtr& refPrev, NodePtr& refNext, UInt32 level, KeyType priority );
   void              Insert( KeyType priority, const T& value, NodePtr& outResult );
   void              HelpDelete();
};

template< typename T, typename K, bool SORT_MIN_TO_MAX, UInt32 MAX_LEVEL, UInt32 PROBABILISTIC_BIAS >
typename ThreadSafePriorityQueue< T, K, SORT_MIN_TO_MAX, MAX_LEVEL, PROBABILISTIC_BIAS >::Node::FreeList ThreadSafePriorityQueue< T, K, SORT_MIN_TO_MAX, MAX_LEVEL, PROBABILISTIC_BIAS >::Node::smFreeLists[ MAX_LEVEL ];

/// Construct an empty queue.
///
/// Internally, this creates a head node with maximal priority and a tail node with minimal priority,
/// both at maximum level.

template< typename T, typename K, bool SORT_MIN_TO_MAX, UInt32 MAX_LEVEL, UInt32 PROBABILISTIC_BIAS >
ThreadSafePriorityQueue< T, K, SORT_MIN_TO_MAX, MAX_LEVEL, PROBABILISTIC_BIAS >::ThreadSafePriorityQueue()
{
   NodePtr::UnsafeWrite( head_, new ( MAX_LEVEL - 1 )
      Node( SORT_MIN_TO_MAX ? TypeTraits< KeyType >::MIN : TypeTraits< KeyType >::MAX, ValueType() ) );
   NodePtr::UnsafeWrite( tail_, new ( MAX_LEVEL - 1 )
      Node( SORT_MIN_TO_MAX ? TypeTraits< KeyType >::MAX : TypeTraits< KeyType >::MIN, ValueType() ) );

   for( UInt32 level = 0; level < MAX_LEVEL; level ++ )
      head_->GetNext( level ) = tail_;
}

/// Return true if the queue does not currently contain an item.

template< typename T, typename K, bool SORT_MIN_TO_MAX, UInt32 MAX_LEVEL, UInt32 PROBABILISTIC_BIAS >
bool ThreadSafePriorityQueue< T, K, SORT_MIN_TO_MAX, MAX_LEVEL, PROBABILISTIC_BIAS >::IsEmpty()
{
   return ( head_->GetNext( 0 ) == tail_ );
}

/// Insert the given value into the queue at the place determined by the given priority.

template< typename T, typename K, bool SORT_MIN_TO_MAX, UInt32 MAX_LEVEL, UInt32 PROBABILISTIC_BIAS >
inline void ThreadSafePriorityQueue< T, K, SORT_MIN_TO_MAX, MAX_LEVEL, PROBABILISTIC_BIAS >::Insert( KeyType priority, const ValueType& value )
{
   NodePtr result;
   Insert( priority, value, result );
}

template< typename T, typename K, bool SORT_MIN_TO_MAX, UInt32 MAX_LEVEL, UInt32 PROBABILISTIC_BIAS >
void ThreadSafePriorityQueue< T, K, SORT_MIN_TO_MAX, MAX_LEVEL, PROBABILISTIC_BIAS >::Insert( KeyType priority, const ValueType& value, NodePtr& outResult )
{
   // Create a new node at a random level.

   outResult = NULL;
   NodePtr::UnsafeWrite( outResult, new Node( priority, value ) );
   UInt32 resultNodeLevel = outResult->GetLevel();

   // Link up all the levels.  Do this bottom-up instead of
   // top-down (as would be the right way for a skiplist) so
   // that our list state always remains valid.  If going top-down,
   // we'll Insert nodes with NULL pointers at their lower levels.

   UInt32 currentLevel = 0;
   do 
   {
      while( 1 )
      {
         NodePtr nextNode;
         NodePtr prevNode;
         
         ScanFromHead( prevNode, nextNode, currentLevel, priority );        

         outResult->GetNext( currentLevel ) = nextNode;
         if( prevNode->GetNext( currentLevel ).trySetFromTo( nextNode, outResult, NodePtr::TAG_FailIfSet ) )
            break;
         else
            outResult->GetNext( currentLevel ) = 0;
      }

      currentLevel ++;
   }
   while(    currentLevel <= resultNodeLevel
          && !outResult->IsMarkedForDeletion() ); // No point linking up remaining levels if another thread already took this node.
}

/// Take the item with the highest priority from the queue.
///
/// @param outValue Reference to where the resulting value should be stored.
/// @param upToPriority Priority limit (inclusive) up to which items are taken from the queue.
/// @return true if there was a matching item in the queue.

template< typename T, typename K, bool SORT_MIN_TO_MAX, UInt32 MAX_LEVEL, UInt32 PROBABILISTIC_BIAS >
bool ThreadSafePriorityQueue< T, K, SORT_MIN_TO_MAX, MAX_LEVEL, PROBABILISTIC_BIAS >::TakeNext( T& outValue, KeyType upToPriority )
{
   // Iterate through to the first unmarked node.

   NodePtr prevNode = head_;
   while( 1 )
   {
      NodePtr node;
      ReadNext( prevNode, node, 0 );

      if( node == tail_ )
         return false; // End reached.

      bool priorityThresholdReached = SORT_MIN_TO_MAX
         ? ( upToPriority >= node->GetPriority() )
         : ( upToPriority <= node->GetPriority() );

      if( !priorityThresholdReached )
         return false;
      else
      {
         // Try to mark the node for deletion.  Only if that succeeds, taking the
         // node was a success and we can return.  If it fails, spin and try again.

         if( node->TryMarkForDeletion() )
         {
            HelpDelete();

            // Node is now off the list and will disappear as soon as
            // all references held by threads (including this one)
            // go out of scope.

            outValue = node->GetValue();
            node->ClearValue();

            return true;
         }
      }
   }
}

/// Update the given references to the next non-deleted node at the given level.
/// refPrev will be updated to reference the immediate predecessor of the next
/// node returned.  Note that this can be a node in deleted state.
///
/// @param refPrev Reference to a node of which the successor node should be
///    returned.  Updated to immediate predecessor of refNext on return.
/// @param refNext Reference to update to refer to next non-deleted node on
///    the given level.
/// @param level Skiplist level to operate on.

template< typename T, typename K, bool SORT_MIN_TO_MAX, UInt32 MAX_LEVEL, UInt32 PROBABILISTIC_BIAS >
inline void ThreadSafePriorityQueue< T, K, SORT_MIN_TO_MAX, MAX_LEVEL, PROBABILISTIC_BIAS >::ReadNext( NodePtr& refPrev, NodePtr& refNext, UInt32 level )
{
   while( 1 )
   {
      refNext = refPrev->GetNext( level );
      ASSERT_MESSAGE( refNext != NULL, "ThreadSafePriorityQueue::ReadNext() - next is NULL" );
      if( !refNext->IsMarkedForDeletion() || refNext == tail_ )
         break;

      refPrev = refNext;
   }
}

/// Scan for the position at which to Insert a node of the given priority.
/// Upon return, the position between refPrev and refNext is the one to Insert at.
///
/// @param refPrev position at which to start scanning; updated to match Insert position.
/// @param refNext

template< typename T, typename K, bool SORT_MIN_TO_MAX, UInt32 MAX_LEVEL, UInt32 PROBABILISTIC_BIAS >
void ThreadSafePriorityQueue< T, K, SORT_MIN_TO_MAX, MAX_LEVEL, PROBABILISTIC_BIAS >::Scan( NodePtr& refPrev, NodePtr& refNext, UInt32 level, KeyType priority )
{
   while( 1 )
   {
      ReadNext( refPrev, refNext, level );
      if( refNext == tail_
         || ( SORT_MIN_TO_MAX
            ? ( refNext->GetPriority() > priority )
            : ( refNext->GetPriority() < priority ) ) )
         break;

      refPrev = refNext;
   }
}

///

template< typename T, typename K, bool SORT_MIN_TO_MAX, UInt32 MAX_LEVEL, UInt32 PROBABILISTIC_BIAS >
void ThreadSafePriorityQueue< T, K, SORT_MIN_TO_MAX, MAX_LEVEL, PROBABILISTIC_BIAS >::ScanFromHead( NodePtr& refPrev, NodePtr& refNext, UInt32 level, KeyType priority )
{
   // Purge dead nodes at left end of queue so
   // we don't get stuck hitting the same node
   // in deletable state over and over again.
   HelpDelete();

   Int32 currentLevel = MAX_LEVEL - 1;
   refPrev = head_;
   do
   {
      Scan( refPrev, refNext, currentLevel, priority );
      currentLevel --;
   }
   while( currentLevel >= Int32( level ) );
}

template< typename T, typename K, bool SORT_MIN_TO_MAX, UInt32 MAX_LEVEL, UInt32 PROBABILISTIC_BIAS >
void ThreadSafePriorityQueue< T, K, SORT_MIN_TO_MAX, MAX_LEVEL, PROBABILISTIC_BIAS >::HelpDelete()
{
   // Clean out all the references from head.
   // Spin over a given reference on each level until head
   // clearly refers to a node in non-deletable state.  This
   // makes this code work cooperatively with other threads
   // doing takeNexts on prior or later nodes while also
   // guaranteeing that all next pointers to us will eventually
   // disappear.
   //
   // Note that this is *the only place* where we will be cleaning
   // out our lists.

   Int32 level = MAX_LEVEL - 1;
   do
   {
      while( 1 )
      {
         NodePtr ptr = head_->GetNext( level );
         if( !ptr->IsMarkedForDeletion() )
            break;
         else
         {
            NodePtr& next = ptr->GetNext( level );
            next.SetTag();
            head_->GetNext( level ).TrySetFromTo( ptr, next, NodePtr::TAG_Unset );
            ASSERT_MESSAGE( next->GetRefCount() >= 2, "ThreadSafePriorityQueue::HelpDelete() - invalid refcount" );
         }
      }

      level --;
   }
   while( level >= 0 );
}

template< typename T, typename K, bool SORT_MIN_TO_MAX, UInt32 MAX_LEVEL, UInt32 PROBABILISTIC_BIAS >
inline ThreadSafePriorityQueue< T, K, SORT_MIN_TO_MAX, MAX_LEVEL, PROBABILISTIC_BIAS >::Node::Node( KeyType priority, const ValueType& value )
   : Parent( false ),
     priority_( priority ),
     value_( value )
{
   memset( next_, 0, sizeof( Node* ) * ( GetLevel() + 1 ) );

   // Level is already set by the allocation routines.
}

template< typename T, typename K, bool SORT_MIN_TO_MAX, UInt32 MAX_LEVEL, UInt32 PROBABILISTIC_BIAS >
ThreadSafePriorityQueue< T, K, SORT_MIN_TO_MAX, MAX_LEVEL, PROBABILISTIC_BIAS >::Node::~Node()
{
   for( UInt32 level = 0; level < ( GetLevel() + 1 ); level ++ )
      GetNext( level ) = NULL;
}

/// Return the skip list level the node is at.

template< typename T, typename K, bool SORT_MIN_TO_MAX, UInt32 MAX_LEVEL, UInt32 PROBABILISTIC_BIAS >
inline UInt32 ThreadSafePriorityQueue< T, K, SORT_MIN_TO_MAX, MAX_LEVEL, PROBABILISTIC_BIAS >::Node::GetLevel()
{
   // Mask out the deletion request bit.

   return ( level_ & 0x7FFFFFFF );
}

/// Return the successor node at the given level.
/// @param level The level of the desired successor node; must be within the node's level bounds.

template< typename T, typename K, bool SORT_MIN_TO_MAX, UInt32 MAX_LEVEL, UInt32 PROBABILISTIC_BIAS >
inline typename ThreadSafePriorityQueue< T, K, SORT_MIN_TO_MAX, MAX_LEVEL, PROBABILISTIC_BIAS >::NodePtr& ThreadSafePriorityQueue< T, K, SORT_MIN_TO_MAX, MAX_LEVEL, PROBABILISTIC_BIAS >::Node::GetNext( UInt32 level )
{
   return *reinterpret_cast< NodePtr* >( &next_[ level ] );
}

/// Return true if the node is marked to be deleted.

template< typename T, typename K, bool SORT_MIN_TO_MAX, UInt32 MAX_LEVEL, UInt32 PROBABILISTIC_BIAS >
inline bool ThreadSafePriorityQueue< T, K, SORT_MIN_TO_MAX, MAX_LEVEL, PROBABILISTIC_BIAS >::Node::IsMarkedForDeletion()
{
   return ( level_ & 0x80000000 )!=0;
}

/// Attempt to mark the node for deletion.  If the mark bit has not yet been set
/// and setting it on the current thread succeeds, returns true.
///
/// @return true, if the marking succeeded.

template< typename T, typename K, bool SORT_MIN_TO_MAX, UInt32 MAX_LEVEL, UInt32 PROBABILISTIC_BIAS >
inline bool ThreadSafePriorityQueue< T, K, SORT_MIN_TO_MAX, MAX_LEVEL, PROBABILISTIC_BIAS >::Node::TryMarkForDeletion()
{
	UInt32 oldVal = level_ & 0x7FFFFFFF;
	UInt32 newVal = oldVal | 0x80000000;

	return ( dCompareAndSwap( level_, oldVal, newVal ) );
}

/// Choose a random level.
///
/// The chosen level depends on the given PROBABILISTIC_BIAS and MAX_LEVEL,
/// but is not affected by the actual number of nodes in a queue.

template< typename T, typename K, bool SORT_MIN_TO_MAX, UInt32 MAX_LEVEL, UInt32 PROBABILISTIC_BIAS >
UInt32 ThreadSafePriorityQueue< T, K, SORT_MIN_TO_MAX, MAX_LEVEL, PROBABILISTIC_BIAS >::Node::RandomLevel()
{
   UInt32 level = 0;
   while( Randomizer::frand() < ( ( ( float32 ) PROBABILISTIC_BIAS ) / 100 ) && level < ( MAX_LEVEL - 1 ) )
      level ++;
   return level;
}

/// Allocate a new node.
/// The node comes with a reference count of one and its level already set.
///
/// @param level The level to allocate the node at.  If this is -1, a random level is chosen.
/// @return a new node.

template< typename T, typename K, bool SORT_MIN_TO_MAX, UInt32 MAX_LEVEL, UInt32 PROBABILISTIC_BIAS >
void* ThreadSafePriorityQueue< T, K, SORT_MIN_TO_MAX, MAX_LEVEL, PROBABILISTIC_BIAS >::Node::operator new( size_t size, Int32 level )
{
   if( level == -1 )
      level = RandomLevel();

   Node* node = 0;
   while( 1 )
   {
      // Try to take a node from the freelist.  If there's none,
      // allocate a new one.

      if( !smFreeLists[ level ].destroyed_ )
         node = Node::SafeRead( smFreeLists[ level ].nodes_ );

      if( !node )
      {
         node = ( Node* ) malloc( sizeof( Node ) + sizeof( Node* ) * level );
         memset( node, 0, sizeof( Node ) );
         node->level_ = level;
         node->AddRef();
         break;
      }
      else if( dCompareAndSwap( smFreeLists[ level ].nodes_, node, node->next_[ 0 ] ) )
      {
         node->ClearLowestBit();
         break;
      }
      else
         node->Release(); // Other thread was quicker than us; release.
   }

   ASSERT_MESSAGE( node->GetRefCount() != 0, "ThreadSafePriorityQueue::new Node() - invalid refcount" );
   ASSERT_MESSAGE( ( node->GetRefCount() % 2 ) == 0, "ThreadSafePriorityQueue::new Node() - invalid refcount" );
   return node;
}

/// Reclaim a node.
///
/// @param node The node to reclaim.  Must refer to a Node instance.

template< typename T, typename K, bool SORT_MIN_TO_MAX, UInt32 MAX_LEVEL, UInt32 PROBABILISTIC_BIAS >
void ThreadSafePriorityQueue< T, K, SORT_MIN_TO_MAX, MAX_LEVEL, PROBABILISTIC_BIAS >::Node::operator delete( void* ptr )
{
   //TODO: limit number of nodes kept

   Node* node = ( Node* ) ptr;
   UInt32 level = node->GetLevel();
   node->level_ = level; // Reset the node's deletion bit.

   while( !smFreeLists[ level ].destroyed_ )
   {
      // Put the node on the freelist.

      Node* freeList = smFreeLists[ level ].nodes_;
      node->next_[ 0 ] = freeList;
      
      if( dCompareAndSwap( smFreeLists[ level ].nodes_, freeList, node ) )
      {
         node = NULL;
         break;
      }
   }
   
   if( node )
      free( node );
}

template< typename T, typename K, bool SORT_MIN_TO_MAX, UInt32 MAX_LEVEL, UInt32 PROBABILISTIC_BIAS >
ThreadSafePriorityQueue< T, K, SORT_MIN_TO_MAX, MAX_LEVEL, PROBABILISTIC_BIAS >::Node::FreeList::~FreeList()
{
   destroyed_ = true;
   while( nodes_ )
   {
      //FIXME: could leak some bytes under unfortunate circumstances (this in
      //   combination with destroyed_ is a dependent write)

      Node* next = nodes_;
      if( dCompareAndSwap( nodes_, next, next->next_[ 0 ] ) )
         free( next );
   }
}

//--------------------------------------------------------------------------
//    ThreadSafePriorityQueueWithUpdate.
//--------------------------------------------------------------------------

/// Fast, lock-free priority queue implementation for concurrent access that
/// performs dynamic re-prioritization of items.
///
/// Within the bounds of a set update interval UPDATE_INTERVAL, the TakeNext
/// method is guaranteed to always return the item that has the highest priority
/// at the time the method is called rather than at the time items were inserted
/// into the queue.
///
/// Values placed on the queue must implement the following interface:
///
/// @code
/// template&lt; typename K >
/// struct IThreadSafePriorityQueueItem
/// {
///    // Default constructor.
///    IThreadSafePriorityQueueItem();
///
///    // Return the current priority.
///    // This must run normally even if the item is already dead.
///    K GetPriority();
///
///    // Return true if the item is still meant to be waiting in the queue.
///    bool isAlive();
/// };
/// @endcode

template< typename T, typename K, bool SORT_MIN_TO_MAX = false, UInt32 MAX_LEVEL = 4, UInt32 PROBABILISTIC_BIAS = 50 >
struct ThreadSafePriorityQueueWithUpdate : public ThreadSafePriorityQueue< T, K, SORT_MIN_TO_MAX, MAX_LEVEL, PROBABILISTIC_BIAS >
{

   typedef T ValueType;
   typedef K KeyType;

   enum { DEFAULT_UPDATE_INTERVAL = 256 };

   ThreadSafePriorityQueueWithUpdate( UInt32 updateInterval = DEFAULT_UPDATE_INTERVAL );

   void              Insert( KeyType priority, const T& value );
   bool              TakeNext( T& outValue, KeyType upToPriority = ( SORT_MIN_TO_MAX ? TypeTraits< KeyType >::MAX : TypeTraits< KeyType >::MIN ) );

   UInt32               GetUpdateInterval() const;
   void              SetUpdateInterval( UInt32 value );

   KeyType           GetTimeBasedPriorityBoost() const;
   void              SetTimeBasedPriorityBoost( KeyType value );

   void              UpdatePriorities();

protected:
   typedef ThreadSafePriorityQueue< T, K, SORT_MIN_TO_MAX, MAX_LEVEL, PROBABILISTIC_BIAS > Parent;
   typedef UInt32 TickType;
   typedef typename Parent::NodePtr NodePtr;

   UInt32               updateInterval_;
   KeyType           priorityBoost_;      ///< If this is non-zero, priorities will be boosted by this amount each update.  This can be used to prevent constant high-priority inserts to starve low-priority items already in the queue.

   /// Work queue for node updates.
   ThreadSafePriorityQueue< NodePtr, TickType, true, MAX_LEVEL, PROBABILISTIC_BIAS > mUpdateQueue;

   TickType          GetTick()            { /*return getTimeMillsec()*/0u; }
};

template< typename T, typename K, bool SORT_MIN_TO_MAX, UInt32 MAX_LEVEL, UInt32 PROBABILISTIC_BIAS >
ThreadSafePriorityQueueWithUpdate< T, K, SORT_MIN_TO_MAX, MAX_LEVEL, PROBABILISTIC_BIAS >::ThreadSafePriorityQueueWithUpdate( UInt32 updateInterval )
   : updateInterval_( updateInterval ),
     priorityBoost_( TypeTraits< KeyType >::ZERO )
{
}

/// Return the current update interval in milliseconds.

template< typename T, typename K, bool SORT_MIN_TO_MAX, UInt32 MAX_LEVEL, UInt32 PROBABILISTIC_BIAS >
UInt32 ThreadSafePriorityQueueWithUpdate< T, K, SORT_MIN_TO_MAX, MAX_LEVEL, PROBABILISTIC_BIAS >::GetUpdateInterval() const
{
   return updateInterval_;
}

/// Set update interval of queue to given value.
///
/// <em>Call this method on the main thread only.</em>
///
/// @param value Time between priority updates in milliseconds.

template< typename T, typename K, bool SORT_MIN_TO_MAX, UInt32 MAX_LEVEL, UInt32 PROBABILISTIC_BIAS >
void ThreadSafePriorityQueueWithUpdate< T, K, SORT_MIN_TO_MAX, MAX_LEVEL, PROBABILISTIC_BIAS >::SetUpdateInterval( UInt32 value )
{
   updateInterval_ = value;
}

/// Return the delta to apply to priorities on each update.
/// Set to zero to deactivate time-based priority adjustments.

template< typename T, typename K, bool SORT_MIN_TO_MAX, UInt32 MAX_LEVEL, UInt32 PROBABILISTIC_BIAS >
K ThreadSafePriorityQueueWithUpdate< T, K, SORT_MIN_TO_MAX, MAX_LEVEL, PROBABILISTIC_BIAS >::GetTimeBasedPriorityBoost() const
{
   return priorityBoost_;
}

/// Set the delta for time-based priority adjustments to the given value.
///
/// <em>Call this method on the main thread only.</em>
///
/// @param value The new priority adjustment value.

template< typename T, typename K, bool SORT_MIN_TO_MAX, UInt32 MAX_LEVEL, UInt32 PROBABILISTIC_BIAS >
void ThreadSafePriorityQueueWithUpdate< T, K, SORT_MIN_TO_MAX, MAX_LEVEL, PROBABILISTIC_BIAS >::SetTimeBasedPriorityBoost( KeyType value )
{
   priorityBoost_ = value;
}

template< typename T, typename K, bool SORT_MIN_TO_MAX, UInt32 MAX_LEVEL, UInt32 PROBABILISTIC_BIAS >
void ThreadSafePriorityQueueWithUpdate< T, K, SORT_MIN_TO_MAX, MAX_LEVEL, PROBABILISTIC_BIAS >::Insert( KeyType priority, const ValueType& value )
{
   NodePtr node;
   Parent::Insert( priority, value, node );
   mUpdateQueue.Insert( GetTick() + GetUpdateInterval(), node );
}

template< typename T, typename K, bool SORT_MIN_TO_MAX, UInt32 MAX_LEVEL, UInt32 PROBABILISTIC_BIAS >
bool ThreadSafePriorityQueueWithUpdate< T, K, SORT_MIN_TO_MAX, MAX_LEVEL, PROBABILISTIC_BIAS >::TakeNext( T& outValue, KeyType upToPriority )
{
   UpdatePriorities();

   bool result = false;
   do
   {
      result = Parent::TakeNext( outValue, upToPriority );
   }
   while( result && !outValue.IsAlive() );
   
   return result;
}

///

template< typename T, typename K, bool SORT_MIN_TO_MAX, UInt32 MAX_LEVEL, UInt32 PROBABILISTIC_BIAS >
void ThreadSafePriorityQueueWithUpdate< T, K, SORT_MIN_TO_MAX, MAX_LEVEL, PROBABILISTIC_BIAS >::UpdatePriorities()
{
   TickType currentTime       = GetTick();
   UInt32      numNodesUpdated   = 0;
   UInt32      numNodesDead      = 0;
   UInt32      numNodesChanged   = 0;

   NodePtr node;
   while( mUpdateQueue.TakeNext( node, currentTime ) )
   {
      numNodesUpdated ++;

      // Since we're updating nodes on the update queue only periodically,
      // their associated values or main queue nodes may have died in the
      // meantime.  If so, we just discard them here.

      if( node->GetValue().IsAlive()
          && !node->IsMarkedForDeletion() )
      {
         KeyType newPriority = node->GetValue().GetPriority() + GetTimeBasedPriorityBoost();
         if( newPriority != node->GetPriority() )
         {
            // Node is outdated.  Reinsert with new priority and mark the
            // old node for deletion.

            Insert( newPriority, node->GetValue() );
            node->TryMarkForDeletion();
            numNodesChanged ++;
         }
         else
         {
            // Node is still current.  Just move to end.

            mUpdateQueue.Insert( currentTime + GetUpdateInterval(), node );
         }
      }
      else
         numNodesDead ++;
   }
}

}}}

#endif // !_THREADSAFEPRIORITYQUEUE_H_
