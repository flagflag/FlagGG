#ifndef _TYPETRAITS_H_
#define _TYPETRAITS_H_

#include "Core/BaseTypes.h"

//--------------------------------------------------------------------------
//    Construct.
//--------------------------------------------------------------------------
template< class T >
inline T* ConstructArrayInPlace( T* p, UInt32 num )
{
	return new ( p ) T[ num ];
}

template <class T>
inline T* ConstructInPlace(T* p)
{
	return new ( p ) T;
}


template <class T>
inline void DestructInPlace(T* p)
{
	p->~T();
}

struct _ConstructDefault
{
   template< typename T >
   static T Single()
   {
      return T();
   }
   template< typename T, typename A >
   static T Single( A a )
   {
      return T( a );
   }
   template< typename T, typename A, typename B >
   static T Single( A a, B b )
   {
      return T( a, b );
   }
   template< typename T >
   static void array( T* ptr, UInt32 num )
   {
      ConstructArrayInPlace< T >( ptr, num );
   }
   template< typename T, typename A >
   static void array( T* ptr, UInt32 num, A a )
   {
      for( UInt32 i = 0; i < num; ++ i )
         ptr[ i ] = Single< T >( a );
   }
};
struct _ConstructPrim
{
   template< typename T >
   static T Single()
   {
      return 0;
   }
   template< typename T, typename A >
   static T Single( T a )
   {
      return a;
   }
   template< typename T >
   static void array( T* ptr, UInt32 num )
   {
      memset( ptr, 0, num * sizeof( T ) );
   }
   template< typename T, typename A >
   static void array( T* ptr, UInt32 num, T a )
   {
      for( UInt32 i = 0; i < num; ++ i )
         ptr[ i ] = a;
   }
};
struct _ConstructPtr
{
   template< typename T >
   static T* Single()
   {
      return new T;
   }
   template< typename T, typename A >
   static T* Single( A a )
   {
      return new T( a );
   }
   template< typename T, typename A, typename B >
   static T* Single( A a, B b )
   {
      return new T( a, b );
   }
   template< typename  T >
   static void array( T** ptr, UInt32 num )
   {
      for( UInt32 i = 0; i < num; ++ i )
         ptr[ i ] = Single< T >();
   }
   template< typename T, typename A >
   static void array( T** ptr, UInt32 num, A a )
   {
      for( UInt32 i = 0; i < num; ++ i )
         ptr[ i ] = Single< T >( a );
   }
};

//--------------------------------------------------------------------------
//    Destruct.
//--------------------------------------------------------------------------

struct _DestructDefault
{
   template< typename T >
   static void Single( T& val )
   {
      val.~T();
   }
   template< typename T >
   static void array( T* ptr, UInt32 num )
   {
      for( UInt32 i = 0; i < num; ++ i )
         Single< T >( ptr[ i ] );
   }
};
struct _DestructPrim
{
   template< typename T >
   static void Single( T& val ) {}
   template< typename T >
   static void array( T* ptr, UInt32 num ) {}
};
struct _DestructPtr
{
   template< typename T >
   static void Single( T*& val )
   {
      delete val;
      val = NULL;
   }
   template< typename T >
   static void array( T* ptr, UInt32 num )
   {
      for( UInt32 i = 0; i < num; ++ i )
         Single< T >( ptr[ i ] );
   }
};

//--------------------------------------------------------------------------
//    TypeTraits.
//--------------------------------------------------------------------------

template< typename T >
struct _TypeTraits
{
   typedef T BaseType;
   typedef const T ConstType;
   typedef _ConstructDefault Construct;
   typedef _DestructDefault Destruct;
};
template< typename T >
struct _TypeTraits< T* >
{
   typedef T BaseType;
   typedef const T ConstType;
   typedef _ConstructPtr Construct;
   typedef _DestructPtr Destruct;

   template< typename A >
   static bool IsTaggedPtr( A* ptr ) { return ( uintptr_t( ptr ) & 0x1 ); }
   template< typename A >
   static A* GetTaggedPtr( A* ptr ) { return ( A* ) ( uintptr_t( ptr ) | 0x1 ); }
   template< typename A >
   static A* GetUntaggedPtr( A* ptr ) { return ( A* ) ( uintptr_t( ptr ) & (~0x1) ); }
};

template< typename T >
struct TypeTraits : public TypeTraits< typename T::Parent >
{
   typedef T BaseType;
   typedef const T ConstType;
};
template< typename T >
struct TypeTraits< T* > : public TypeTraits< typename T::Parent* >
{
   typedef T BaseType;
   typedef const T ConstType;
};
template< typename T >
struct TypeTraits< T* const > : public TypeTraits< typename T::Parent* >
{
   typedef T BaseType;
   typedef const T ConstType;
};
template<>
struct TypeTraits< void > : public _TypeTraits< void > {};
template<>
struct TypeTraits< void* > : public _TypeTraits< void* > {};
template<>
struct TypeTraits< void* const > : public _TypeTraits< void* > {};

// Type traits for primitive types.

template<>
struct TypeTraits< bool > : public _TypeTraits< bool >
{
   typedef _ConstructPrim Construct;
   typedef _DestructPrim Destruct;
};
template<>
struct TypeTraits< Int8 > : public _TypeTraits< Int8 >
{
   static const Int8 MIN = Int8_MIN;
   static const Int8 MAX = Int8_MAX;
   static const Int8 ZERO = 0;
   typedef _ConstructPrim Construct;
   typedef _DestructPrim Destruct;
};
template<>
struct TypeTraits< UInt8 > : public _TypeTraits< UInt8 >
{
   static const UInt8 MIN = 0;
   static const UInt8 MAX = Uint8_MAX;
   static const UInt8 ZERO = 0;
   typedef _ConstructPrim Construct;
   typedef _DestructPrim Destruct;
};
template<>
struct TypeTraits< Int16 > : public _TypeTraits< Int16 >
{
   static const Int16 MIN = Int16_MIN;
   static const Int16 MAX = Int16_MAX;
   static const Int16 ZERO = 0;
   typedef _ConstructPrim Construct;
   typedef _DestructPrim Destruct;
};
template<>
struct TypeTraits< UInt16 > : public _TypeTraits< UInt16 >
{
   static const UInt16 MIN = 0;
   static const UInt16 MAX = Uint16_MAX;
   static const UInt16 ZERO = 0;
   typedef _ConstructPrim Construct;
   typedef _DestructPrim Destruct;
};
template<>
struct TypeTraits< Int32 > : public _TypeTraits< Int32 >
{
   static const Int32 MIN = Int32_MIN;
   static const Int32 MAX = Int32_MAX;
   static const Int32 ZERO = 0;
   typedef _ConstructPrim Construct;
   typedef _DestructPrim Destruct;
};
template<>
struct TypeTraits< UInt32 > : public _TypeTraits< UInt32 >
{
   static const UInt32 MIN = 0;
   static const UInt32 MAX = Uint32_MAX;
   static const UInt32 ZERO = 0;
   typedef _ConstructPrim Construct;
   typedef _DestructPrim Destruct;
};
template<>
struct TypeTraits< Real > : public _TypeTraits< Real >
{
   static const Real MIN;
   static const Real MAX;
   static const Real ZERO;
   typedef _ConstructPrim Construct;
   typedef _DestructPrim Destruct;
};

//--------------------------------------------------------------------------
//    Utilities.
//--------------------------------------------------------------------------

template< typename T >
inline T ConstructSingle()
{
   typedef typename TypeTraits< T >::BaseType Type;
   typedef typename TypeTraits< T >::Construct Construct;
   return Construct::template Single< Type >();
}
template< typename T, typename A >
inline T ConstructSingle( A a )
{
   typedef typename TypeTraits< T >::BaseType BaseType;
   typedef typename TypeTraits< T >::Construct Construct;
   return Construct::template Single< BaseType >( a );
}
template< typename T, typename A, typename B >
inline T ConstructSingle( A a, B b )
{
   typedef typename TypeTraits< T >::BaseType BaseType;
   typedef typename TypeTraits< T >::Construct Construct;
   return Construct::template Single< BaseType >( a, b );
}
template< typename T >
inline void constructArray( T* ptr, UInt32 num )
{
   typedef typename TypeTraits< T >::BaseType BaseType;
   typedef typename TypeTraits< T >::Construct Construct;
   Construct::template array< BaseType >( ptr, num );
}
template< typename T, typename A >
inline void constructArray( T* ptr, UInt32 num, A a )
{
   typedef typename TypeTraits< T >::BaseType BaseType;
   typedef typename TypeTraits< T >::Construct Construct;
   Construct::template array< BaseType >( ptr, num, a );
}
template< typename T >
inline void destructSingle( T& val )
{
   typedef typename TypeTraits< T >::BaseType BaseType;
   typedef typename TypeTraits< T >::Destruct Destruct;
   Destruct::template Single< BaseType >( val );
}
template< typename T >
inline void destructArray( T* ptr, UInt32 num )
{
   typedef typename TypeTraits< T >::BaseType BaseType;
   typedef typename TypeTraits< T >::Destruct Destruct;
   Destruct::template array< BaseType >( ptr, num );
}

template< typename T>
inline T& Deref( T& val )
{
   return val;
}
template< typename T >
inline T& Deref( T* ptr )
{
   return *ptr;
}

/// Delete a Single object policy.
struct DeleteSingle
{
   template<class T>
   static void Destroy(T *ptr) { delete ptr; }
};

/// Delete an array of objects policy.
struct DeleteArray
{
   template<class T>
   static void Destroy(T *ptr) { delete [] ptr; }
};

///
template< typename T >
struct ValueHolder
{
   T value;
   
   ValueHolder( const T& value )
      : value( value ) {}
      
   operator T() const { return value; }
};
template<>
struct ValueHolder< void >
{
   ValueHolder() {}
};

#endif // _TYPETRAITS_H_
