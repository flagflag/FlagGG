#ifndef __CRYASSERT_H__
#define __CRYASSERT_H__
#pragma once
#define USE_CRY_ASSERT

#ifdef _WIN64
#include <assert.h>
#endif

#include "Export.h"

//-----------------------------------------------------------------------------------------------------
// Use like this:
// CRY_ASSERT(expression);
// CRY_ASSERT_MESSAGE(expression,"Useful message");
// CRY_ASSERT_TRACE(expression,("This should never happen because parameter %d named %s is %f",iParameter,szParam,fValue));
//-----------------------------------------------------------------------------------------------------
#ifdef _WIN64
#define DEBUG_BREAK /*_wassert(_CRT_WIDE(__FILE__), _CRT_WIDE(__FILE__), (unsigned)(__LINE__))*/__debugbreak()
#elif __APPLE__
#define DEBUG_BREAK
#else
#define DEBUG_BREAK _asm { int 3 }
#endif
#if defined(USE_CRY_ASSERT) && defined(WIN32) && defined(_DEBUG)
    //#pragma message("CryAssert enabled.")
    FlagGG_API void CryAssertTrace(const char *,...);
	FlagGG_API bool CryAssert(const char *,const char *,unsigned int,bool *);
    #define CRY_ASSERT(condition) CRY_ASSERT_MESSAGE(condition,NULL)
    #define CRY_ASSERT_MESSAGE(condition,message) CRY_ASSERT_TRACE(condition,(message))
 
    #define CRY_ASSERT_TRACE(condition,parenthese_message)			    \
    do														            \
    {															        \
        static bool s_bIgnoreAssert = false;					        \
        if(!s_bIgnoreAssert && !(condition))				            \
        {														        \
            CryAssertTrace parenthese_message;					        \
            if(CryAssert(#condition,__FILE__,__LINE__,&s_bIgnoreAssert))    \
            {									                        \
                 DEBUG_BREAK;								            \
            }												            \
        }														        \
    } while(0)

    #undef assert
    #define assert CRY_ASSERT
#else

#if defined(__APPLE__)
    #define chSTR2(x) #x
    #define chSTR(x) chSTR2(x)
    #define CRY_ASSERT(condition) if(!(condition)) printf("[Assertion Failed] CONDITION:(" #condition")" " FILE:" __FILE__ " LINE:" chSTR(__LINE__) "\n")
    #define CRY_ASSERT_MESSAGE(condition,message) if(!(condition)) {printf("[Assertion Failed] CONDITION:(" #condition")" " FILE:" __FILE__ " LINE:" chSTR(__LINE__) " MESSAGE:");printf(message);printf("\n");}

    #define CRY_ASSERT_TRACE(condition,parenthese_message) if(!(condition)) {printf("[Assertion Failed] CONDITION:(" #condition")" " FILE:" __FILE__ " LINE:" chSTR(__LINE__) " MESSAGE:"); printf parenthese_message; printf("\n");}
#else
    #include <assert.h>
    #define CRY_ASSERT(condition) assert(condition)
    #define CRY_ASSERT_MESSAGE(condition,message) assert(condition)
    #define CRY_ASSERT_TRACE(condition,parenthese_message) assert(condition)
#endif

#endif

#endif
