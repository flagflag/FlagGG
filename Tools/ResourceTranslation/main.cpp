#include <ResourceTranslation/ResourceTranslation.h>
#include <Resource/ResourceCache.h>
#include <Utility/SystemHelper.h>

using namespace FlagGG;

LJSONValue commandParam;

void Run()
{
	const String sourceResDir = commandParam["src_res_dir"].GetString();
	const String targetResDir = commandParam["target_res_dir"].GetString();

	GetSubsystem<ResourceCache>()->AddResourceDir("");

	ResourceTranslation* translation = CreateResourceTranslation("SCE");

	Vector<String> fileNames;
	FindFiles(sourceResDir, "*", true, FindFilesMode_File, fileNames);

	for (const auto& relativePath : fileNames)
	{
		const String absolutePath = sourceResDir + "/" + relativePath;
		const String extension = GetExtension(absolutePath, true);
		if (extension == ".png" || extension == ".tga" || extension == ".jpg" || extension == ".dds")
		{
			Copy(absolutePath, targetResDir + "/" + relativePath);
		}
		if (extension == ".material")
		{
			GenericMaterialDescription desc;
			if (translation->LoadMaterial(absolutePath, desc))
			{

			}
		}
	}
}

int main(int argc, const char* argv[])
{
	if (ParseCommand(argv + 1, argc - 1, commandParam))
	{
		Run();
	}

	return 0;
}
