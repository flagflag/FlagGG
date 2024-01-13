/*
** Bundled memory allocator.
** Donated to the public domain.
*/

#ifndef _LJ_ALLOC_H
#define _LJ_ALLOC_H

#include "lj_def.h"

LJ_FUNC void* lj_alloc_create(PRNGState* rs);
LJ_FUNC void lj_alloc_setprng(void* msp, PRNGState* rs);
LJ_FUNC void lj_alloc_destroy(void* msp);
LJ_FUNC void* lj_alloc_f(void* msp, void* ptr, size_t osize, size_t nsize);
LJ_FUNC void* lj_alloc_malloc(void* msp, size_t nsize);
LJ_FUNC void* lj_alloc_realloc(void* msp, void* ptr, size_t nsize);
LJ_FUNC void* lj_alloc_free(void* msp, void* ptr);

#endif
