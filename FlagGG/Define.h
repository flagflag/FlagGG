#ifndef __DEFINE__
#define __DEFINE__

#define ONE_KB 1024
#define ONE_MB 1048576
#define ONE_GB 1073741824

#define USE_DIRECT3D

#ifdef CPP_17
#define NOEXCEPT noexcept
#else
#define NOEXCEPT
#endif

#define INIT_ARRAY(ARRAY, VALUE) for (auto& ITEM : ARRAY) ITEM = VALUE;

#endif