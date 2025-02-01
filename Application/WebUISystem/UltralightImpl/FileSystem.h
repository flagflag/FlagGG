#pragma once

#include <Ultralight/platform/FileSystem.h>

namespace ultralight
{

class UltralightFileSystem : public FileSystem
{
public:
	UltralightFileSystem();

	///
	/// Check if a file exists within the file system.
	/// 
	/// @param  file_path  Relative file path (the string following the file:/// prefix)
	/// 
	/// @return Returns whether or not a file exists at the path specified.
	///
	virtual bool FileExists(const String& file_path) override;

	///
	/// Get the mime-type of a file (eg "text/html").
	/// 
	/// This is usually determined by analyzing the file extension.
	/// 
	/// If a mime-type cannot be determined, this should return "application/unknown".
	/// 
	/// @param  file_path  Relative file path (the string following the file:/// prefix)
	///
	/// @return Returns whether or not a file exists at the path specified.
	///
	virtual String GetFileMimeType(const String& file_path) override;

	///
	/// Get the charset / encoding of a file (eg "utf-8", "iso-8859-1").
	/// 
	/// @note This is only applicable for text-based files (eg, "text/html", "text/plain") and is
	///       usually determined by analyzing the contents of the file.
	/// 
	/// @param  file_path  Relative file path (the string following the file:/// prefix)
	///
	/// @return Returns the charset of the specified file. If a charset cannot be determined, a safe
	///         default to return is "utf-8".
	/// 
	virtual String GetFileCharset(const String& file_path) override;

	///
	/// Open a file for reading and map it to a Buffer.
	/// 
	/// To minimize copies, you should map the requested file into memory and use Buffer::Create()
	/// to wrap the data pointer (unmapping should be performed in the destruction callback).
	///
	/// @note  
	/// \parblock
	/// File data addresses returned from this function should generally be aligned to 16-byte
	/// boundaries (the default alignment on most operating systems-- if you're using C stdlib or
	/// C++ STL functions this is already handled for you).
	/// 
	/// This requirement is currently necessary when loading the ICU data file (eg, icudt67l.dat),
	/// and may be relaxed for  other files (but you may still see a performance benefit due to cache
	/// line alignment).
	/// 
	/// If you can't guarantee alignment or are unsure, you can use Buffer::CreateFromCopy to copy
	/// the file data content to an aligned block (at the expense of data duplication).
	/// \endparblock
	/// 
	/// @param  file_path  Relative file path (the string following the file:/// prefix)
	/// 
	/// @return If the file was able to be opened, this returns a Buffer object representing the
	///         contents of the file. If the file was unable to be opened, you should return nullptr.
	/// 
	virtual RefPtr<Buffer> OpenFile(const String& file_path) override;
};

}
