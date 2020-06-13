#include "SystemHelper.h"
#include "Container/Str.h"

#if _WIN32
#include <windows.h>
#include <shlwapi.h>
#include <io.h>
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "winmm.lib")
#else
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
#define MAX_PATH NAME_MAX
#endif

namespace FlagGG
{
	namespace Utility
	{
		namespace SystemHelper
		{
#ifdef _WIN32
			const char* const PATH_SEPARATOR = "\\";
#else
			const char* const PATH_SEPARATOR = "/";
#endif

			Container::String FormatPath(const Container::String& path)
			{
				Container::WString wPath(path);
				UInt32 size = path.Length();

				wchar_t buffer[MAX_PATH] = { 0 };
				UInt32 cur = 0;
				bool lastMatch = false;
#ifdef _WIN32
				char match1 = wchar_t('\\');
				char match2 = wchar_t('/');
#else
				char match1 = wchar_t('/');
				char match2 = wchar_t('\\');
#endif

				for (UInt32 i = 0; i < wPath.Length(); ++i)
				{
					bool curMatch = wPath[i] == match1 || wPath[i] == match2;

					if (i == 0 && curMatch)
					{
						while (i < wPath.Length())
						{
							curMatch = wPath[i] == match1 || wPath[i] == match2;
							if (!curMatch)
							{
								break;
							}
							++i;
						}
					}

					if (!curMatch)
					{
						buffer[cur++] = wPath[i];
					}
					else if (!lastMatch)
					{
						buffer[cur++] = match1;
					}

					lastMatch = curMatch;
					}

				return std::move(Container::String(buffer));
			}

			void Sleep(uint64_t time)
			{
#if _WIN32
				::Sleep(time);
#else
				usleep(time * 1000);
#endif
			}

			UInt32 Tick()
			{
#if _WIN32
				return (UInt32)timeGetTime();
#else
				struct timeval time{};
				gettimeofday(&time, nullptr);
				return (UInt32)(time.tv_sec * 1000 + time.tv_usec / 1000);
#endif
			}

			UInt64 HiresTick()
			{
#ifdef _WIN32
				if (HiresTimer::IsSupported())
				{
					LARGE_INTEGER counter;
					QueryPerformanceCounter(&counter);
					return counter.QuadPart;
				}
				else
					return timeGetTime();
#else
				struct timeval time {};
				gettimeofday(&time, nullptr);
				return time.tv_sec * 1000000LL + time.tv_usec;
#endif
			}

			bool ParseCommand(const char** argv, UInt32 argc, Config::LJSONValue& result)
			{
				for (UInt32 i = 0; i < argc; ++i)
				{
					Container::String command = argv[i];
					if (command.Length() > 0 && command[0] == '-')
					{
						UInt32 pos = command.Find('=');
						if (pos != Container::String::NPOS)
						{
							const Container::String key = command.Substring(1, pos - 1);
							const Container::String value = command.Substring(pos + 1);
							result[key] = value;
						}
						else
						{
							result[command.Substring(1)] = true;
						}
					}
				}

				return true;
			}

			bool DirExists(const Container::String& path)
			{
#ifdef _WIN32
				DWORD attributes = GetFileAttributesW(Container::WString(path).CString());
				if (attributes == INVALID_FILE_ATTRIBUTES || attributes & FILE_ATTRIBUTE_DIRECTORY)
					return false;
#else
				struct stat st{};
				if (stat(path.CString(), &st) || st.st_mode & S_IFDIR)
					return false;
#endif
				return true;
			}

			bool FileExists(const Container::String& path)
			{
#ifdef _WIN32
				DWORD attributes = GetFileAttributesW(Container::WString(path).CString());
				if (attributes == INVALID_FILE_ATTRIBUTES || (attributes & FILE_ATTRIBUTE_DIRECTORY))
					return false;
#else
				struct stat st{};
				if (stat(path.CString(), &st) || !(st.st_mode & S_IFDIR))
					return false;
#endif
				return true;
			}

			bool CreateDir(const Container::String& path)
			{
#ifdef _WIN32
				Container::WString wPath(path);
				bool success = CreateDirectoryW(wPath.CString(), nullptr) == TRUE || GetLastError() == ERROR_ALREADY_EXISTS;
#else
				bool success = mkdir(path.CString(), S_IRWXU) == 0 || errno == EEXIST;
#endif
				return success;
			}

			bool HasAccess(const Container::String& path)
			{
				return true;
			}

#ifndef _WIN32
			/* Our simplified data entry structure */
			struct _finddata_t
			{
				char *name;
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
				char *pattern;
				char *curfn;
				char *directory;
				int dirlen;
				DIR *dirfd;
			};

			int _findnext(intptr_t id, struct _finddata_t *data);
			int _findclose(intptr_t id);

			intptr_t _findfirst(const char *pattern, struct _finddata_t *data)
			{
				_find_search_t *fs = new _find_search_t;
				fs->curfn = NULL;
				fs->pattern = NULL;

				// Separate the mask from directory name
				const char *mask = strrchr(pattern, '/');
				if (mask)
				{
					fs->dirlen = static_cast<int>(mask - pattern);
					mask++;
					fs->directory = (char *)malloc(fs->dirlen + 1);
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

			int _findnext(intptr_t id, struct _finddata_t *data)
			{
				_find_search_t *fs = reinterpret_cast<_find_search_t *>(id);

				/* Loop until we run out of entries or find the next one */
				dirent *entry;
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
				char *xfn = new char[fs->dirlen + 1 + namelen + 1];
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
				_find_search_t *fs = reinterpret_cast<_find_search_t *>(id);

				ret = fs->dirfd ? closedir(fs->dirfd) : 0;
				free(fs->pattern);
				free(fs->directory);
				if (fs->curfn)
					free(fs->curfn);
				delete fs;

				return ret;
			}
#endif
			static bool ms_IgnoreHidden = true;
			
			static bool is_reserved_dir(const char *fn)
			{
				return (fn[0] == '.' && (fn[1] == 0 || (fn[1] == '.' && fn[2] == 0)));
			}

			static bool is_absolute_path(const char* path)
			{
				return path[0] == '/' || path[0] == '\\';
			}

			static Container::String concatenate_path(const Container::String& base, const Container::String& name)
			{
				if (base.Empty() || is_absolute_path(name.CString()))
					return name;
				else
					return base + "/" + name;
			}

			void FindFiles(const Container::String& dirPath, const Container::String& pattern, bool recursive, int fileMode, Container::Vector<Container::String>& fileNames)
			{
				long lHandle, res;
				struct _finddata_t tagData;

				// pattern can contain a directory name, separate it from mask
				auto pos1 = pattern.FindLast('/');
				auto pos2 = pattern.FindLast('\\');
				if (pos1 == Container::String::NPOS || ((pos2 != Container::String::NPOS) && (pos1 < pos2)))
					pos1 = pos2;
				Container::String directory;
				if (pos1 != Container::String::NPOS)
					directory = pattern.Substring(0, pos1 + 1);

				Container::String full_pattern = concatenate_path(dirPath, pattern);

				lHandle = (long)_findfirst(full_pattern.CString(), &tagData);
				res = 0;
				while (lHandle != -1 && res != -1)
				{
					if ((((fileMode & FindFilesMode_Dir) != 0) == ((tagData.attrib & _A_SUBDIR) != 0)) &&
						(!ms_IgnoreHidden || (tagData.attrib & _A_HIDDEN) == 0) &&
						(((fileMode & FindFilesMode_Dir) == 0) || !is_reserved_dir(tagData.name)))
					{
						fileNames.Push(directory + tagData.name);
						//if (detailList)
						//{
						//	ArchiveFileInfo fi;
						//	fi.filename = directory + tagData.name;
						//	fi.basename = tagData.name;
						//	fi.path = directory;
						//	fi.compressedSize = tagData.size;
						//	fi.uncompressedSize = tagData.size;
						//	detailList->push_back(fi);
						//}
					}
					res = _findnext(lHandle, &tagData);
				}
				// Close if we found any files
				if (lHandle != -1)
					_findclose(lHandle);

				// Now find directories
				if (recursive)
				{
					Container::String base_dir = dirPath;
					if (!directory.Empty())
					{
						base_dir = concatenate_path(dirPath, directory);
						// Remove the last '/'
						base_dir.Erase(base_dir.Length() - 1);
					}
					base_dir.Append("/*");

					// Remove directory name from pattern
					Container::String mask("/");
					if (pos1 != Container::String::NPOS)
						mask.Append(pattern.Substring(pos1 + 1));
					else
						mask.Append(pattern);

					lHandle = (long)_findfirst(base_dir.CString(), &tagData);
					res = 0;
					while (lHandle != -1 && res != -1)
					{
						if ((tagData.attrib & _A_SUBDIR) &&
							(!ms_IgnoreHidden || (tagData.attrib & _A_HIDDEN) == 0) &&
							!is_reserved_dir(tagData.name))
						{
							// recurse
							base_dir = directory;
							base_dir.Append(tagData.name).Append(mask);
							FindFiles(dirPath, base_dir, recursive, fileMode, fileNames);
						}
						res = _findnext(lHandle, &tagData);
					}
					// Close if we found any files
					if (lHandle != -1)
						_findclose(lHandle);
				}
			}

			Math::IntRect GetDesktopRect()
			{
#if _WIN32
				RECT rect;
				SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
				return Math::IntRect(Math::IntVector2(rect.left, rect.top), Math::IntVector2(rect.right, rect.bottom));
#else
				return Mach::IntRect::ZERO;
#endif
			}


			Timer::Timer()
			{
				Reset();
			}

			UInt32 Timer::GetMilliSeconds(bool reset)
			{
				UInt32 curTime = Tick();
				UInt32 deltaTime = curTime - startTime_;
				if (reset)
				{
					startTime_ = curTime;
				}
				return deltaTime;
			}

			void Timer::Reset()
			{
				startTime_ = Tick();
			}

			bool HiresTimer::supported(false);
			UInt64 HiresTimer::frequency(1000);

			void HiresTimer::InitSupported()
			{
#ifdef _WIN32
				LARGE_INTEGER frequency;
				if (QueryPerformanceFrequency(&frequency))
				{
					HiresTimer::frequency = frequency.QuadPart;
					HiresTimer::supported = true;
				}
#else
				HiresTimer::frequency = 1000000;
				HiresTimer::supported = true;
#endif
			}

			HiresTimer::HiresTimer()
			{
				Reset();
			}

			UInt64 HiresTimer::GetUSec(bool reset)
			{
				UInt64 currentTime = HiresTick();
				UInt64 elapsedTime = currentTime - startTime_;

				if (elapsedTime < 0)
					elapsedTime = 0;

				if (reset)
					startTime_ = currentTime;

				return (elapsedTime * 1000000LL) / frequency;
			}

			void HiresTimer::Reset()
			{
				startTime_ = HiresTick();
			}
		}
	}
}