#pragma once

#include "Core/CryAssert.h"

#undef OUT
#define OUT

#undef IN_OUT
#define IN_OUT

#ifndef SAFE_DELETE
#define SAFE_DELETE(p){ if(p) delete (p); (p) = NULL; }
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p){ if(p) delete[] (p); (p) = NULL; }
#endif

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p){ if(p) (p)->Release(); (p) = NULL; }
#endif

// ---------------------------------------------------------------------
// assert
// ---------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
// Use like this:
// ASSERT(expression);
// ASSERT_MESSAGE(expression,"Useful message");
// ASSERT_TRACE(expression,("This should never happen because parameter %d named %s is %f",iParameter,szParam,fValue));
//-----------------------------------------------------------------------------------------------------
#ifdef ASSERT
#undef ASSERT
#endif
#if defined(_DEBUG) || defined(DEBUG)
#define ASSERT(x) CRY_ASSERT(x)
#define ASSERT_MESSAGE(condition,message) CRY_ASSERT_MESSAGE(condition,message)
#define ASSERT_TRACE(condition,message) CRY_ASSERT_TRACE(condition,message)
#else
#define ASSERT(x) ((void)0)
#define ASSERT_MESSAGE(condition,message) ((void)0)
#define ASSERT_TRACE(condition,message) ((void)0)
#endif

#ifndef VERIFY
#if defined(_DEBUG) || defined(DEBUG)
#define VERIFY(f) ASSERT(f)
#else
#define VERIFY(f) ((void)(f))
#endif
#endif

#ifdef _WIN32
	#define FORCEINLINE __forceinline									/* Force code to be inline */
#elif defined(__ANDROID__) || defined(IOS) || defined(__linux__)
	#define FORCEINLINE inline __attribute__ ((always_inline))			/* Force code to be inline */
#else
	#define FORCEINLINE
#endif

#define FORCENOINLINE __declspec(noinline)                            /* Force code to NOT be inline */

#ifdef _WIN32
	#if defined(__clang__)
		#define GCC_PACK(n) __attribute__((packed,aligned(n)))
		#define GCC_ALIGN(n) __attribute__((aligned(n)))
		#if defined(_MSC_VER)
			#define MS_ALIGN(n) __declspec(align(n)) // With -fms-extensions, Clang will accept either alignment attribute
		#endif
	#else
		#define MS_ALIGN(n) __declspec(align(n))
	#endif
#elif defined(__ANDROID__) || defined(IOS) || defined(__APPLE__)
	#define GCC_PACK(n) __attribute__((packed,aligned(n)))
	#define GCC_ALIGN(n) __attribute__((aligned(n)))
#endif

#ifndef GCC_PACK
	#define GCC_PACK(n)
#endif
#ifndef GCC_ALIGN
	#define GCC_ALIGN(n)
#endif
#ifndef MS_ALIGN
	#define MS_ALIGN(n)
#endif

/** Branch prediction hints */
#ifndef LIKELY						/* Hints compiler that expression is likely to be true, much softer than UE_ASSUME - allows (penalized by worse performance) expression to be false */
	#if ( defined(__clang__) || defined(__GNUC__) ) && defined(__linux__)	// effect of these on non-Linux platform has not been analyzed as of 2016-03-21
		#define LIKELY(x)			__builtin_expect(!!(x), 1)
	#else
		// the additional "!!" is added to silence "warning: equality comparison with exteraneous parenthese" messages on android
		#define LIKELY(x)			(!!(x))
	#endif
#endif

#ifndef UNLIKELY					/* Hints compiler that expression is unlikely to be true, allows (penalized by worse performance) expression to be true */
	#if ( defined(__clang__) || defined(__GNUC__) ) && defined(__linux__)	// effect of these on non-Linux platform has not been analyzed as of 2016-03-21
		#define UNLIKELY(x)			__builtin_expect(!!(x), 0)
	#else
		// the additional "!!" is added to silence "warning: equality comparison with exteraneous parenthese" messages on android
		#define UNLIKELY(x)			(!!(x))
	#endif
#endif

// https://source.android.google.cn/devices/tech/debug/intsan?hl=zh-cn
#define TSAN_SAFE

#if 0
	#define OPERATOR_NEW_MSVC_PRAGMA MSVC_PRAGMA( warning( suppress : 28251 ) )	//	warning C28251: Inconsistent annotation for 'new': this instance has no annotations
#else
	#define OPERATOR_NEW_MSVC_PRAGMA
#endif

#ifndef OPERATOR_NEW_THROW_SPEC
	#define OPERATOR_NEW_THROW_SPEC
#endif
#ifndef OPERATOR_DELETE_THROW_SPEC
	#define OPERATOR_DELETE_THROW_SPEC
#endif
#ifndef OPERATOR_NEW_NOTHROW_SPEC
	#define OPERATOR_NEW_NOTHROW_SPEC throw()
#endif
#ifndef OPERATOR_DELETE_NOTHROW_SPEC
	#define OPERATOR_DELETE_NOTHROW_SPEC throw()
#endif

// #define DEBUG_BREAK __debugbreak()

#define PURE_VIRTUAL(func,...) { ASSERT_MESSAGE(false, "Pure virtual not implemented (" #func ")"); __VA_ARGS__ }

#define HELPER_NAME_CAT_IMPL(A, B) A ## B
#define HELPER_NAME_CAT(A, B) HELPER_NAME_CAT_IMPL(A, B)

#if defined(USE_CRY_ASSERT) && defined(WIN32) && defined(_DEBUG)
// MSVC (v19.00.24215.1 at time of writing) ignores no-inline attributes on
// lambdas. This can be worked around by calling the lambda from inside this
// templated (and correctly non-inlined) function.
template <typename RetType = void, class InnerType, typename... ArgTypes>
RetType FORCENOINLINE DispatchCheckVerify(InnerType&& Inner, ArgTypes const&... Args)
{
    return Inner(Args...);
}

#define ENSURE_IMPL(Capture, Always, Condition, ParentheseMessage)       \
    (!!(Condition) || (DispatchCheckVerify<bool>([Capture]()             \
    {                                                                    \
        static bool executed = false;					                 \
        if (!executed || Always)				                         \
        {														         \
            CryAssertTrace ParentheseMessage;					         \
            if (!CryAssert(#Condition, __FILE__, __LINE__, &executed))   \
            {									                         \
                return false;								             \
            }												             \
            return true;                                                 \
        }                                                                \
        return false;                                                    \
    }) && ([] () { DEBUG_BREAK; } (), false)))

#define CRY_ENSURE(Condition) ENSURE_IMPL( , false, Condition, (NULL))
#define CRY_ENSURE_MESSAGE(Condition, ParentheseMessage) ENSURE_IMPL(&, false, Condition, (ParentheseMessage))
#else
#define CRY_ENSURE(Condition, ...) (Condition)
#define CRY_ENSURE_MESSAGE(Condition, ...) (Condition)
#endif
