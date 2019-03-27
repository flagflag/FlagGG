/*
** $Id: lprefix.h,v 1.2 2014/12/29 16:54:13 roberto Exp $
** Definitions for Lua code that must come before any other header file
** See Copyright Notice in lua.h
*/

#ifndef lprefix_h
#define lprefix_h


/*
** Allows POSIX/XSI stuff
*/
#if !defined(LUA_USE_C89)	/* { */

#if !defined(_XOPEN_SOURCE)
#define _XOPEN_SOURCE           600
#elif _XOPEN_SOURCE == 0
#undef _XOPEN_SOURCE  /* use -D_XOPEN_SOURCE=0 to undefine it */
#endif

/*
** Allows manipulation of large files in gcc and some other compilers
*/
#if !defined(LUA_32BITS) && !defined(_FILE_OFFSET_BITS)
#define _LARGEFILE_SOURCE       1
#define _FILE_OFFSET_BITS       64
#endif

#endif				/* } */


/*
** Windows stuff
*/
#if defined(_WIN32) 	/* { */

#if !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS  /* avoid warnings about ISO C functions */
#endif

#if defined(DEBUG)
#include "lstate.h"
#include "ldo.h"

__inline void luaG_callerrfunc(lua_State *L, const char* msg) {
	if (L->errfunc != 0) {
		lua_pushstring(L, msg);
		StkId errfunc = restorestack(L, L->errfunc);
		setobjs2s(L, L->top, L->top - 1);
		setobjs2s(L, L->top - 1, errfunc);
		L->top++;
		luaD_callnoyield(L, L->top - 2, 1);
	}
}

__declspec(dllimport)
void __stdcall RaiseException(unsigned long dwExceptionCode, unsigned long dwExceptionFlags, unsigned long nNumberOfArguments, const unsigned long * lpArguments);

#define LUAI_THROW(L,c)		RaiseException(0xE0000001, 0, 0, 0)
#define LUAI_TRY(L,c,a)		__try { a } __except(1) { if ((c)->status == 0) { (c)->status = -1; luaG_callerrfunc(L, "c function crash."); } }
#define luai_jmpbuf		int
#endif

#endif			/* } */

#endif

