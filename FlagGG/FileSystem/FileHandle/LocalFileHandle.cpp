#include "LocalFileHandle.h"
#include "Log.h"

#include <cstdio>

namespace FlagGG
{

#if PLATFORM_WINDOWS
static const wchar_t* openMode[] =
{
	L"rb",
	L"wb",
	L"ab",
	L"r+b",
	L"w+b"
};
#else
static const char* openMode[] =
{
	"rb",
	"wb",
	"ab",
	"r+b",
	"w+b"
};
#endif

const Int32 FSEEK_ORIGIN[] =
{
	SEEK_SET,
	SEEK_CUR,
	SEEK_END,
};

// 对标准库接口进行轻量封装，适配x86和x64平台的ftell、fseek等接口
struct FileInterfaceBase
{
#if PLATFORM_WINDOWS
	FORCEINLINE static FILE* fopen(const wchar_t* fileName, const wchar_t* mode)
	{
		return ::_wfopen(fileName, mode);
	}
#else
	FORCEINLINE static FILE* fopen(const char* fileName, const char* mode)
	{
		return ::fopen(fileName, mode);
	}
#endif

	FORCEINLINE static size_t fread(void* buffer, size_t elementSize, size_t elementCount, FILE* stream)
	{
		return ::fread(buffer, elementSize, elementCount, stream);
	}

	FORCEINLINE static size_t fwrite(void const* buffer, size_t elementSize, size_t elementCount, FILE* stream)
	{
		return ::fwrite(buffer, elementSize, elementCount, stream);
	}

	FORCEINLINE static int fclose(FILE* stream)
	{
		return ::fclose(stream);
	}

	FORCEINLINE static int fflush(FILE* stream)
	{
		return ::fflush(stream);
	}
};

template<int PointerSize>
struct TFileInterface : public FileInterfaceBase {};

template<>
struct TFileInterface<4> : public FileInterfaceBase
{
	FORCEINLINE static int fseek(FILE* stream, long offset, int origin)
	{
		return ::fseek(stream, offset, origin);
	}

	FORCEINLINE static Int32 ftell(FILE* stream)
	{
		return ::ftell(stream);
	}
};

template<>
struct TFileInterface<8> : public FileInterfaceBase
{
	FORCEINLINE static int fseek(FILE* stream, Int64 offset, int origin)
	{
		return ::_fseeki64(stream, offset, origin);
	}

	FORCEINLINE static Int64 ftell(FILE* stream)
	{
		return ::_ftelli64(stream);
	}
};

typedef TFileInterface<sizeof(void*)> std_file;

LocalFileHandle::LocalFileHandle()
	: handle_{}
	, size_(0u)
	, sizeDirty_(false)
{

}

LocalFileHandle::~LocalFileHandle()
{
	Close();
}

bool LocalFileHandle::Open(const String& fileName, FileMode mode)
{
	sizeDirty_ = true;

#if PLATFORM_WINDOWS
	WString fileNameW(fileName);
	handle_ = std_file::fopen(fileNameW.CString(), openMode[Int32(mode)]);
#else
	handle_ = std_file::fopen(fileName.CString(), openMode[Int32(mode)]);
#endif
	
	// If file did not exist in readwrite mode, retry with write-update mode
	if (mode == FileMode::FILE_READWRITE && !handle_)
	{
#if PLATFORM_WINDOWS
		handle_ = std_file::fopen(fileNameW.CString(), openMode[Int32(mode) + 1]);
#else
		handle_ = std_file::fopen(fileName.CString(), openMode[Int32(mode) + 1]);
#endif
	}

	return !!handle_;
}

bool LocalFileHandle::IsOpen() const
{
	return !!handle_;
}

bool LocalFileHandle::Read(void* buffer, USize size)
{
	return handle_ ? std_file::fread(buffer, size, 1, (FILE*)handle_) == 1 : false;
}

bool LocalFileHandle::Write(const void* buffer, USize size)
{
	return handle_ ? std_file::fwrite(buffer, size, 1, (FILE*)handle_) == 1 : false;
}

bool LocalFileHandle::Seek(SSize offset, SeekFrom origin)
{
	return handle_ ? fseek((FILE*)handle_, offset, FSEEK_ORIGIN[Int32(origin)]) == 0 : false;
}

void LocalFileHandle::Close()
{
	if (handle_)
	{
		std_file::fclose((FILE*)handle_);
		handle_ = nullptr;
		size_ = 0;
	}
}

void LocalFileHandle::Flush()
{
	if (handle_)
		std_file::fflush((FILE*)handle_);
}

USize LocalFileHandle::Position() const
{
	return handle_ ? std_file::ftell((FILE*)handle_) : 0u;
}

USize LocalFileHandle::Size() const
{
	if (sizeDirty_)
	{
		USize offset = std_file::ftell((FILE*)handle_);
		std_file::fseek((FILE*)handle_, 0, SEEK_END);
		size_ = std_file::ftell((FILE*)handle_);
		std_file::fseek((FILE*)handle_, offset, SEEK_SET);
		sizeDirty_ = false;
	}
	return size_;
}

}
