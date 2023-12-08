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

#ifndef FORCE_INLINE
#if defined(__GNUC__)
#define FORCE_INLINE inline __attribute__ ((always_inline))
#elif defined(_MSC_VER)
#define FORCE_INLINE __forceinline
#else
#define FORCE_INLINE inline
#endif
#endif

#ifndef FORCEINLINE
#define FORCEINLINE FORCE_INLINE
#endif

#define FORCENOINLINE __declspec(noinline)							/* Force code to NOT be inline */

#define PURE_VIRTUAL(func,...) { ASSERT_MESSAGE(false, "Pure virtual not implemented (" #func ")"); __VA_ARGS__ }

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
