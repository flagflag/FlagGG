#include "FileSystem.h"

#include <FileSystem/FileManager.h>
#include <FileSystem/FileSystemArchive/DefaultFileSystemArchive.h>
#include <Utility/SystemHelper.h>
#include <string>
#include <io.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <windows.h>

#pragma comment(lib, "Shlwapi.lib")

namespace ultralight
{

UltralightFileSystem::UltralightFileSystem()
{
	using namespace FlagGG;

	GetSubsystem<AssetFileManager>()->AddArchive(new DefaultFileSystemArchive(GetLocalFileSystem(), GetProgramDir() + "Assets"));
}

bool UltralightFileSystem::FileExists(const String& file_path)
{
	using namespace FlagGG;

	return GetSubsystem<AssetFileManager>()->FileExists(file_path.utf8().data());
}

#if PLATFORM_WINDOWS
static std::wstring GetMimeType(const std::wstring& szExtension)
{
	// return mime type for extension
	HKEY hKey = NULL;
	std::wstring szResult = L"application/unknown";

	// open registry key
	if (RegOpenKeyExW(HKEY_CLASSES_ROOT, szExtension.c_str(), 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
		// define buffer
		wchar_t szBuffer[256] = { 0 };
		DWORD dwBuffSize = sizeof(szBuffer);

		// get content type
		if (RegQueryValueExW(hKey, L"Content Type", NULL, NULL, (LPBYTE)szBuffer, &dwBuffSize)
			== ERROR_SUCCESS) {
			// success
			szResult = szBuffer;
		}

		// close key
		RegCloseKey(hKey);
	}

	// return result
	return szResult;
}
#endif

String UltralightFileSystem::GetFileMimeType(const String& file_path)
{
#if PLATFORM_WINDOWS
	String16 path16 = file_path.utf16();
	LPWSTR ext = ::PathFindExtensionW(path16.data());
	std::wstring mimetype = GetMimeType(ext);
	return String16(mimetype.c_str(), mimetype.length());
#else
	return String();
#endif
}

String UltralightFileSystem::GetFileCharset(const String& file_path)
{
	return "utf-8";
}

RefPtr<Buffer> UltralightFileSystem::OpenFile(const String& file_path)
{
	using namespace FlagGG;

	auto buffer = GetSubsystem<AssetFileManager>()->OpenFileReader(file_path.utf8().data());
	
	FlagGG::String fileContent;
	buffer->ToString(fileContent);

	return Buffer::CreateFromCopy(fileContent.CString(), fileContent.Length());
}

}
