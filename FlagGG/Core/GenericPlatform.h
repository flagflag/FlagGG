#pragma once

//---------------------------------------------------------------------
// Utility for automatically setting up the pointer-sized integer type
//---------------------------------------------------------------------

template<typename T32BITS, typename T64BITS, int PointerSize>
struct SelectIntPointerType
{
	// nothing here are is it an error if the partial specializations fail
};

template<typename T32BITS, typename T64BITS>
struct SelectIntPointerType<T32BITS, T64BITS, 8>
{
	// Select the 64 bit type.
	typedef T64BITS TIntPointer;
};

template<typename T32BITS, typename T64BITS>
struct SelectIntPointerType<T32BITS, T64BITS, 4>
{
	// Select the 32 bit type.
	typedef T32BITS TIntPointer;
};

/**
* Generic types for almost all compilers and platforms
**/
struct GenericPlatformTypes
{
	//~ Unsigned base types
	
	// 8-bit unsigned integer
	typedef unsigned char 		uint8;
	
	// 16-bit unsigned integer
	typedef unsigned short int	uint16;
	
	// 32-bit unsigned integer
	typedef unsigned int		uint32;
	
	// 64-bit unsigned integer
	typedef unsigned long long	uint64;

	//~ Signed base types.
	
	// 8-bit signed integer
	typedef	signed char			int8;
	
	// 16-bit signed integer
	typedef signed short int	int16;
	
	// 32-bit signed integer
	typedef signed int	 		int32;
	
	// 64-bit signed integer
	typedef signed long long	int64;

	//~ Character types
	
	// An ANSI character. 8-bit fixed-width representation of 7-bit characters.
	typedef char				ansichar;
	
	// A wide character. In-memory only. ?-bit fixed-width representation of the platform's natural wide character set. Could be different sizes on different platforms.
	typedef wchar_t				widechar;
	
	// An 8-bit character type. In-memory only. 8-bit representation. Should really be char8_t but making this the generic option is easier for compilers which don't fully support C++11 yet (i.e. MSVC).
	typedef uint8				char8;
	
	// A 16-bit character type. In-memory only.  16-bit representation. Should really be char16_t but making this the generic option is easier for compilers which don't fully support C++11 yet (i.e. MSVC).
	typedef uint16				char16;		
	
	// A 32-bit character type. In-memory only. 32-bit representation. Should really be char32_t but making this the generic option is easier for compilers which don't fully support C++11 yet (i.e. MSVC).
	typedef uint32				char32;
	
	// A switchable character. In-memory only. Either ANSICHAR or WIDECHAR, depending on a licensee's requirements.
	typedef widechar			tchar;

	// Unsigned int. The same size as a pointer.
	typedef SelectIntPointerType<uint32, uint64, sizeof(void*)>::TIntPointer uptrint;
	
	// Signed int. The same size as a pointer.
	typedef SelectIntPointerType<int32, int64, sizeof(void*)>::TIntPointer ptrint;
	
	// Unsigned int. The same size as a pointer.
	typedef uptrint usize;
	
	// Signed int. The same size as a pointer.
	typedef ptrint ssize;

	typedef int32					type_of_null;
	typedef decltype(nullptr)		type_of_nullptr;
};

#ifndef RESTRICT
#define RESTRICT __restrict						/* no alias hint */
#endif

// Prefetch
#define PLATFORM_CACHE_LINE_SIZE	64

#if defined(_WIN32)
	#define PLATFORM_WINDOWS 1
#elif defined(__APPLE__)
	#if defined(IOS)
		#define PLATFORM_IOS 1
	#else
		#define PLATFORM_MACOS 1
	#endif
	#define PLATFORM_APPLE 1
#elif defined(__ANDROID__)
	#define PLATFORM_ANDROID 1
	#define PLATFORM_UNIX 1
#elif defined(__linux__)
	#define PLATFORM_LINUX 1
	#define PLATFORM_UNIX 1
#endif

// 暂时不支持32bits
#define PLATFORM_64BITS 1
