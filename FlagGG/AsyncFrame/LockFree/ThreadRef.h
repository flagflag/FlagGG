#ifndef _THREADSAFEREFCOUNT_H_
#define _THREADSAFEREFCOUNT_H_

#include "AsyncFrame/LockFree/Intrinsics.h"
#include "AsyncFrame/LockFree/TypeTraits.h"
#include "Core/BaseMacro.h"

namespace FlagGG {

//--------------------------------------------------------------------------
//    ThreadSafeRefCount.
//--------------------------------------------------------------------------

/// Baseclass for concurrently reference-counted objects.
///
/// @note NOTE that freshly instantiated objects start out with a reference
///    count of ZERO!  Depending on how this class is used, this may not
///    be desirable, so override this behavior in constructors if necessary.
///
/// @param T the class being reference counted; this is passed to this class,
///    so it can call the correct destructor without having to force users
///    to have virtual methods

template< class T, class DeletePolicy = DeleteSingle >
class ThreadSafeRefCount
{
   public:

      typedef void Parent;

      ThreadSafeRefCount()
         : refCount_( 0 ) {}
      ThreadSafeRefCount( bool noSet ) {}

      bool           IsShared() const;
      UInt32            GetRefCount() const;
      void           AddRef();
      void           Release();
      void           ClearLowestBit();
      static T*      SafeRead( T* const volatile& refPtr );

   protected:

      UInt32            refCount_;  ///< Reference count and claim bit.  Note that this increments in steps of two.

      static UInt32     DecrementAndTestAndSet( UInt32& refCount_ );
};

/// @return true if the object is referenced by more than a single
///   reference.

template< class T, class DeletePolicy >
inline bool ThreadSafeRefCount< T, DeletePolicy >::IsShared() const
{
   return ( refCount_ > 3 );
}

/// Get the current reference count.  This method is mostly meant for
/// debugging and should not normally be used.

template< class T, class DeletePolicy >
inline UInt32 ThreadSafeRefCount< T, DeletePolicy >::GetRefCount() const
{
   return refCount_;
}

/// Increase the reference count on the object.

template< class T, class DeletePolicy >
inline void ThreadSafeRefCount< T, DeletePolicy >::AddRef()
{
   dFetchAndAdd( refCount_, 2 );
}

/// Decrease the object's reference count and delete the object, if the count
/// drops to zero and claiming the object by the current thread succeeds.

template< class T, class DeletePolicy >
inline void ThreadSafeRefCount< T, DeletePolicy >::Release()
{
   ASSERT_MESSAGE( refCount_ != 0, "ThreadSafeRefCount::Release() - refcount of zero" );
   if( DecrementAndTestAndSet( refCount_ ) != 0 )
      DeletePolicy::Destroy( ( T* ) this );
}

/// Dereference a reference-counted pointer in a multi-thread safe way.

template< class T, class DeletePolicy >
T* ThreadSafeRefCount< T, DeletePolicy >::SafeRead( T* const volatile& refPtr )
{
   while( 1 )
   {
      // Support tagged pointers here.

      T* ptr = TypeTraits< T* >::GetUntaggedPtr( refPtr );
      if( !ptr )
         return 0;

      ptr->AddRef();
      if( ptr == TypeTraits< T* >::GetUntaggedPtr( refPtr ) )
         return ptr;
      else
         ptr->Release();
   }
}

/// Decrement the given reference count.  Return 1 if the count dropped to zero
/// and the claim bit has been successfully set; return 0 otherwise.

template< class T, class DeletePolicy >
UInt32 ThreadSafeRefCount< T, DeletePolicy >::DecrementAndTestAndSet( UInt32& refCount_ )
{
   UInt32 oldVal;
   UInt32 newVal;

   do
   {
      oldVal = refCount_;
      newVal = oldVal - 2;

      ASSERT_MESSAGE( oldVal >= 2,
         "ThreadSafeRefCount::DecrementAndTestAndSet() - invalid refcount" );

      if( newVal == 0 )
         newVal = 1;
   }
   while( !dCompareAndSwap( refCount_, oldVal, newVal ) );

   return ( ( oldVal - newVal ) & 1 );
}

///

template< class T, class DeletePolicy >
inline void ThreadSafeRefCount< T, DeletePolicy >::ClearLowestBit()
{
   ASSERT_MESSAGE( refCount_ % 2 != 0, "ThreadSafeRefCount::ClearLowestBit() - invalid refcount" );

   UInt32 oldVal;
   UInt32 newVal;

   do
   {
      oldVal = refCount_;
      newVal = oldVal - 1;
   }
   while( !dCompareAndSwap( refCount_, oldVal, newVal ) );
}

//--------------------------------------------------------------------------
//    ThreadSafeRef.
//--------------------------------------------------------------------------

/// Reference to a concurrently reference-counted object.
///
/// This class takes care of the reference-counting as well as protecting
/// the reference itself from concurrent operations.
///
/// Tagging allows the pointer contained in the reference to be flagged.
/// Tag state is preserved through updates to the reference.
///
/// @note If you directly assign a freshly created object with a reference
///   count of zero to a ThreadSafeRef, make absolutely sure the ThreadSafeRef
///   is accessed only by a single thread.  Otherwise there's a risk of the
///   object being released and freed in midst of trying to set the reference.
template< class T >
class ThreadSafeRef
{
   public:

      enum ETag
      {
         TAG_PreserveOld,  ///< Preserve existing tagging state when changing pointer.
         TAG_PreserveNew,  ///< Preserve tagging state of new pointer when changing pointer.
         TAG_Set,          ///< Set tag when changing pointer; okay if already set.
         TAG_Unset,        ///< Unset tag when changing pointer; okay if already unset.
         TAG_SetOrFail,    ///< Set tag when changing pointer; fail if already set.
         TAG_UnsetOrFail,  ///< Unset tag when changing pointer; fail if already unset.
         TAG_FailIfSet,    ///< Fail changing pointer when currently tagged.
         TAG_FailIfUnset   ///< Fail changing pointer when currently untagged.
      };

      typedef ThreadSafeRef< T > ThisType;

      ThreadSafeRef()                        : ptr_( 0 ) {}
      ThreadSafeRef( T* ptr )                : ptr_( ThreadSafeRefCount< T >::SafeRead( ptr ) ) {}
      ThreadSafeRef( const ThisType& ref )   : ptr_( ThreadSafeRefCount< T >::SafeRead( ref.ptr_ ) ) {}
      ~ThreadSafeRef()
      {
         T* ptr = NULL;
         while( !TrySetFromTo( ptr_, ptr ) );
      }

      T*             Ptr() const { return GetUntaggedPtr( ptr_ ) ; }
      void           SetTag() { while( !TrySetFromTo( ptr_, ptr_, TAG_Set ) ); }
      bool           IsTagged() const { return IsTaggedPtr( ptr_ ); }
      bool           TrySetFromTo( T* oldVal, T* const volatile& newVal, ETag tag = TAG_PreserveOld );
      bool           TrySetFromTo( T* oldVal, const ThisType& newVal, ETag tag = TAG_PreserveOld );
      bool           TrySetFromTo( const ThisType& oldVal, const ThisType& newVal, ETag tag = TAG_PreserveOld );
      static void    UnsafeWrite( ThisType& ref, T* ptr );
      static T*      SafeRead( T* const volatile& refPtr ) { return ThreadSafeRefCount< T >::SafeRead( refPtr ); }

      bool           operator ==( T* ptr ) const;
      bool           operator ==( const ThisType& ref ) const;
      bool           operator !=( T* ptr ) const { return !( *this == ptr ); }
      bool           operator !=( const ThisType& ref ) const { return !( *this == ref ); }
      ThisType&      operator =( T* ptr );
      ThisType&      operator =( const ThisType& ref );

      bool           operator !() const   { return ( Ptr() == 0 ); }
      T&             operator *() const   { return *Ptr(); }
      T*             operator ->() const  { return Ptr(); }
                     operator T*() const  { return Ptr(); }

   protected:

      T* volatile    ptr_;

      static bool IsTaggedPtr( T* ptr ) { return TypeTraits< T* >::IsTaggedPtr( ptr ); }
      static T* GetTaggedPtr( T* ptr ) { return TypeTraits< T* >::GetTaggedPtr( ptr ); }
      static T* GetUntaggedPtr( T* ptr ) { return TypeTraits< T* >::GetUntaggedPtr( ptr ); }
};

/// Update the reference from pointing to oldVal to point to newVal.
/// Do so in a thread-safe way.
///
/// This operation will only succeed, if, when doing the pointer-swapping,
/// the reference still points to oldVal.  If, however, the reference
/// has been changed in the meantime by another thread, the operation will
/// fail.
///
/// @param oldVal The pointer assumed to currently be contained in this ThreadSafeRef.
/// @param newVal The pointer to store in this ThreadSafeRef.
/// @param tag Operation to perform on the reference's tag field.
///
/// @return true, if the reference now points to newVal.

template< class T >
bool ThreadSafeRef< T >::TrySetFromTo( T* oldVal, T* const volatile& newVal, ETag tag )
{
   bool setTag = false;
   bool getTag = false;
   bool isTagged = IsTaggedPtr( oldVal );

   switch( tag )
   {
   case TAG_PreserveOld:   setTag = IsTaggedPtr( oldVal ); break;
   case TAG_PreserveNew:   setTag = IsTaggedPtr( newVal ); break;
   case TAG_Set:           setTag = true; break;
   case TAG_Unset:         setTag = false; break;
   case TAG_SetOrFail:     setTag = true; getTag = true; break;
   case TAG_UnsetOrFail:   setTag = false; getTag = true; break;
   case TAG_FailIfSet:     if( isTagged ) return false; break;
   case TAG_FailIfUnset:   if( !isTagged ) return false; break;
   }

   T* newValPtr = ( setTag
                    ? GetTaggedPtr( ThreadSafeRefCount< T >::SafeRead( newVal ) )
                    : GetUntaggedPtr( ThreadSafeRefCount< T >::SafeRead( newVal ) ) );

   if( dCompareAndSwap( ptr_,
                        ( getTag
                          ? ( setTag
                              ? GetUntaggedPtr( oldVal )
                              : GetTaggedPtr( oldVal ) )
                          : oldVal ),
                        newValPtr ) )
   {
      if( GetUntaggedPtr( oldVal ) )
         GetUntaggedPtr( oldVal )->Release();
      return true;
   }
   else
   {
      if( GetUntaggedPtr( newValPtr ) )
         GetUntaggedPtr( newValPtr )->Release();
      return false;
   }
}

template< class T >
inline bool ThreadSafeRef< T >::TrySetFromTo( T* oldVal, const ThisType& newVal, ETag tag )
{
   return TrySetFromTo( oldVal, newVal.ptr_, tag );
}

template< class T >
inline bool ThreadSafeRef< T >::TrySetFromTo( const ThisType& oldVal, const ThisType& newVal, ETag tag )
{
   return TrySetFromTo( oldVal.ptr_, newVal.ptr_, tag );
}

/// Update ref to point to ptr but <em>do not</em> Release an existing
/// reference held by ref nor do the operation in a thread-safe way.
///
/// This method is <em>only</em> for when you absolutely know that your
/// thread is the only thread operating on a reference <em>and</em> you
/// are keeping track of reference counts yourself.
///
/// @param ref The reference to update.
/// @param ptr The new pointer to store in ref.

template< class T >
inline void ThreadSafeRef< T >::UnsafeWrite( ThisType& ref, T* ptr )
{
   ref.ptr_ = ptr;
}

template< class T >
inline bool ThreadSafeRef< T >::operator ==( T* p ) const
{
   return ( Ptr() == p );
}

template< class T >
inline bool ThreadSafeRef< T >::operator ==( const ThisType& ref ) const
{
   return ( Ptr() == ref.Ptr() );
}

template< class T >
inline ThreadSafeRef< T >& ThreadSafeRef< T >::operator =( T* ptr )
{
   while( !TrySetFromTo( ptr_, ptr, TAG_PreserveNew ) );
   return *this;
}

template< class T >
inline ThreadSafeRef< T >& ThreadSafeRef< T >::operator =( const ThisType& ref )
{
   while( !TrySetFromTo( ptr_, ref, TAG_PreserveNew ) );
   return *this;
}


template< typename T >
struct TypeTraits< ThreadSafeRef< T > > : public TypeTraits< T* > {};
template< typename T >
inline T& Deref( ThreadSafeRef< T >& ref )
{
   return *ref;
}
template< typename T >
inline T& Deref( const ThreadSafeRef< T >& ref )
{
   return *ref;
}

}

#endif // _THREADSAFEREFCOUNT_H_
