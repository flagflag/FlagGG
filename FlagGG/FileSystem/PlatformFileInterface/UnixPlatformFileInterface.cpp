#if !defined(WIN32)
#include "FileSystem/PlatformFileInterface.h"

#include <unistd.h>
#include <limits.h>
#include <dirent.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/param.h>
#include <stdio.h>
#include <dirent.h>
#include <fnmatch.h>
#include <string.h>
#include <stdlib.h>

namespace FlagGG
{

bool PlatformFileInterface::DirectoryExists(const String& directoryPath)
{
	struct stat st {};
	return !stat(directoryPath.CString(), &st) && (st.st_mode & S_IFDIR);
}

bool PlatformFileInterface::CreateDirectory(const String& directoryPath)
{
	return mkdir(directoryPath.CString(), S_IRWXU) == 0 || errno == EEXIST;
}

bool PlatformFileInterface::DeleteDirectory(const String& directoryPath)
{
	DIR* dir;
	struct dirent* de;
	struct stat st {};
	dir = opendir(GetNativePath(path).CString());
	if (dir)
	{
		while ((de = readdir(dir)))
		{
			String fileName(de->d_name);
			if (fileName == "." || fileName == "..")
				continue;
			String pathAndName = path + fileName;
			if (!stat(pathAndName.CString(), &st))
			{
				if (st.st_mode & S_IFDIR)
				{
					if (!PlatformFileInterface::DeleteDirectory(pathAndName))
						return false;
				}
				else
				{
					if (!PlatformFileInterface::DeleteFile(pathAndName))
						return false;
				}
			}
		}
		closedir(dir);
		return PlatformFileInterface::DeleteFile(input);
	}

	return false;
}

bool PlatformFileInterface::FileExists(const String& fileName)
{
	struct stat st {};
	return !stat(fileName.CString(), &st) && !(st.st_mode & S_IFDIR);
}

Int64 PlatformFileInterface::GetFileSize(const String& fileName)
{
	return -1;
}

bool PlatformFileInterface::GetFileAttributes(const String& fileName, FileAttributesFlags& attributes)
{
	return false;
}

bool PlatformFileInterface::SetFileAttributes(const String& fileName, const FileAttributesFlags& attributes)
{
	return false;
}

bool PlatformFileInterface::GetFileTimeStamp(const String& fileName, UInt64& timeStamp)
{
	return false;
}

bool PlatformFileInterface::CreateFile(const String& fileName)
{
	return false;
}

bool PlatformFileInterface::DeleteFile(const String& fileName)
{
	return remove(fileName.CString()) == 0;
}

bool PlatformFileInterface::MoveFile(const String& fileName, const String& newFileName)
{
	return rename(fileName.CString(), newFileName.CString()) == 0;
}

bool PlatformFileInterface::CopyFile(const String& fileName, const String& targetFileName)
{
	return copy(fileName.CString(), targetFileName.CString()) == 0;
}

}

/* Our simplified data entry structure */
struct _finddata_t
{
	char* name;
	int attrib;
	unsigned long size;
};

#define _A_NORMAL 0x00  /* Normalfile-Noread/writerestrictions */
#define _A_RDONLY 0x01  /* Read only file */
#define _A_HIDDEN 0x02  /* Hidden file */
#define _A_SYSTEM 0x04  /* System file */
#define _A_ARCH   0x20  /* Archive file */
#define _A_SUBDIR 0x10  /* Subdirectory */

struct _find_search_t
{
	char* pattern;
	char* curfn;
	char* directory;
	int dirlen;
	DIR* dirfd;
};

int _findnext(intptr_t id, struct _finddata_t* data);
int _findclose(intptr_t id);

intptr_t _findfirst(const char* pattern, struct _finddata_t* data)
{
	_find_search_t* fs = new _find_search_t;
	fs->curfn = NULL;
	fs->pattern = NULL;

	// Separate the mask from directory name
	const char* mask = strrchr(pattern, '/');
	if (mask)
	{
		fs->dirlen = static_cast<int>(mask - pattern);
		mask++;
		fs->directory = (char*)malloc(fs->dirlen + 1);
		memcpy(fs->directory, pattern, fs->dirlen);
		fs->directory[fs->dirlen] = 0;
	}
	else
	{
		mask = pattern;
		fs->directory = strdup(".");
		fs->dirlen = 1;
	}

	fs->dirfd = opendir(fs->directory);
	if (!fs->dirfd)
	{
		_findclose((intptr_t)fs);
		return -1;
	}

	/* Hack for "*.*" -> "*' from DOS/Windows */
	if (strcmp(mask, "*.*") == 0)
		mask += 2;
	fs->pattern = strdup(mask);

	/* Get the first entry */
	if (_findnext((intptr_t)fs, data) < 0)
	{
		_findclose((intptr_t)fs);
		return -1;
	}

	return (intptr_t)fs;
}

int _findnext(intptr_t id, struct _finddata_t* data)
{
	_find_search_t* fs = reinterpret_cast<_find_search_t*>(id);

	/* Loop until we run out of entries or find the next one */
	dirent* entry;
	for (;;)
	{
		if (!(entry = readdir(fs->dirfd)))
			return -1;

		/* See if the filename matches our pattern */
		if (fnmatch(fs->pattern, entry->d_name, 0) == 0)
			break;
	}

	if (fs->curfn)
		free(fs->curfn);
	data->name = fs->curfn = strdup(entry->d_name);

	size_t namelen = strlen(entry->d_name);
	char* xfn = new char[fs->dirlen + 1 + namelen + 1];
	sprintf(xfn, "%s/%s", fs->directory, entry->d_name);

	/* stat the file to get if it's a subdir and to find its length */
	struct stat stat_buf;
	if (stat(xfn, &stat_buf))
	{
		// Hmm strange, imitate a zero-length file then
		data->attrib = _A_NORMAL;
		data->size = 0;
	}
	else
	{
		if (S_ISDIR(stat_buf.st_mode))
			data->attrib = _A_SUBDIR;
		else
			/* Default type to a normal file */
			data->attrib = _A_NORMAL;

		data->size = (unsigned long)stat_buf.st_size;
	}

	delete[] xfn;

	/* Files starting with a dot are hidden files in Unix */
	if (data->name[0] == '.')
		data->attrib |= _A_HIDDEN;

	return 0;
}

int _findclose(intptr_t id)
{
	int ret;
	_find_search_t* fs = reinterpret_cast<_find_search_t*>(id);

	ret = fs->dirfd ? closedir(fs->dirfd) : 0;
	free(fs->pattern);
	free(fs->directory);
	if (fs->curfn)
		free(fs->curfn);
	delete fs;

	return ret;
}

#include "PlatformFileInterface.inl"

#endif
