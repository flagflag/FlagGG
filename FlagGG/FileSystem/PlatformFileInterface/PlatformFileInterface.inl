
namespace FlagGG
{

static bool ms_IgnoreHidden = true;

static bool is_reserved_dir(const char* fn)
{
	return (fn[0] == '.' && (fn[1] == 0 || (fn[1] == '.' && fn[2] == 0)));
}

static bool is_absolute_path(const char* path)
{
	return path[0] == '/' || path[0] == '\\';
}

static String concatenate_path(const String& base, const String& name)
{
	if (base.Empty() || is_absolute_path(name.CString()))
		return name;
	else
		return base + "/" + name;
}

void PlatformFileInterface::FindFiles(Vector<String>& foundFiles, const String& directoryPath, const String& pattern, bool recursive, FindFileModeFlags fileMode)
{
	PtrInt lHandle, res;
	struct _finddata_t tagData;

	// pattern can contain a directory name, separate it from mask
	auto pos1 = pattern.FindLast('/');
	auto pos2 = pattern.FindLast('\\');
	if (pos1 == String::NPOS || ((pos2 != String::NPOS) && (pos1 < pos2)))
		pos1 = pos2;
	String directory;
	if (pos1 != String::NPOS)
		directory = pattern.Substring(0, pos1 + 1);

	String full_pattern = concatenate_path(directoryPath, pattern);

	lHandle = (PtrInt)_findfirst(full_pattern.CString(), &tagData);
	res = 0;
	while (lHandle != -1 && res != -1)
	{
		if ((((fileMode & FindFileMode::DIRECTORY) != FindFileMode::NONE) == ((tagData.attrib & _A_SUBDIR) != 0)) &&
			(!ms_IgnoreHidden || (tagData.attrib & _A_HIDDEN) == 0) &&
			(((fileMode & FindFileMode::DIRECTORY) == FindFileMode::NONE) || !is_reserved_dir(tagData.name)))
		{
			foundFiles.Push(directory + tagData.name);
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
		String base_dir = directoryPath;
		if (!directory.Empty())
		{
			base_dir = concatenate_path(directoryPath, directory);
			// Remove the last '/'
			base_dir.Erase(base_dir.Length() - 1);
		}
		base_dir.Append("/*");

		// Remove directory name from pattern
		String mask("/");
		if (pos1 != String::NPOS)
			mask.Append(pattern.Substring(pos1 + 1));
		else
			mask.Append(pattern);

		lHandle = (PtrInt)_findfirst(base_dir.CString(), &tagData);
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
				FindFiles(foundFiles, directoryPath, base_dir, recursive, fileMode);
			}
			res = _findnext(lHandle, &tagData);
		}
		// Close if we found any files
		if (lHandle != -1)
			_findclose(lHandle);
	}
}

}
