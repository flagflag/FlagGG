#include <ResourceTranslation/ResourceTranslation.h>
#include <Resource/ResourceCache.h>
#include <Utility/SystemHelper.h>
#include <Math/SphericalHarmonicsL2.h>
#include <Resource/XMLFile.h>

using namespace FlagGG;

LJSONValue commandParam;

static void BufferToSH(const PODVector<unsigned char>& buffer, SphericalHarmonicsL2& outSH)
{
	outSH.SetZero();
	char* shBuffer = (char*)outSH.sh;
	unsigned byteCount = SphericalHarmonicsL2::kFloatCount * sizeof(float);
	for (unsigned i = 0; i < byteCount; ++i)
	{
		if (i < buffer.Size())
			shBuffer[i] = buffer[i];
	}
}

void Run()
{
	//const String sourceResDir = commandParam["src_res_dir"].GetString();
	//const String targetResDir = commandParam["target_res_dir"].GetString();

	//GetSubsystem<ResourceCache>()->AddResourceDir("");

	//ResourceTranslation* translation = CreateResourceTranslation("SCE");

	//Vector<String> fileNames;
	//FindFiles(sourceResDir, "*", true, FindFilesMode_File, fileNames);

	//for (const auto& relativePath : fileNames)
	//{
	//	const String absolutePath = sourceResDir + "/" + relativePath;
	//	const String extension = GetExtension(absolutePath, true);
	//	if (extension == ".png" || extension == ".tga" || extension == ".jpg" || extension == ".dds")
	//	{
	//		Copy(absolutePath, targetResDir + "/" + relativePath);
	//	}
	//	if (extension == ".material")
	//	{
	//		GenericMaterialDescription desc;
	//		if (translation->LoadMaterial(absolutePath, desc))
	//		{

	//		}
	//	}
	//}

	String temp = "139 48 48 63 252 65 123 62 52 40 126 189 186 106 140 62 172 162 210 61 96 34 132 189 180 110 84 188 60 68 185 189 253 226 213 61 186 30 73 63 185 253 186 62 248 191 151 189 113 22 181 62 57 205 248 61 2 128 133 189 24 115 45 188 214 215 222 189 251 106 231 61 6 110 82 63 223 105 1 63 218 32 139 189 129 70 195 62 196 150 2 62 97 126 106 189 176 30 83 187 92 189 242 189 81 185 195 61";
	Vector<String> temp2 = temp.Split(' ');
	PODVector<unsigned char> buffer;
	for (auto& it : temp2)
	{
		buffer.Push(ToInt(it));
	}

	SphericalHarmonicsL2 sh;
	BufferToSH(buffer, sh);

	Vector4 shaderConstants[SphericalHarmonicsL2::kVec4Count];
	SphericalHarmonicsL2::GetShaderConstantsFromNormalizedSH(sh, shaderConstants);

	XMLFile xmlFile;
	XMLElement dest = xmlFile.CreateRoot("root");
	dest.SetVector4("SHAr", shaderConstants[0]);
	dest.SetVector4("SHAg", shaderConstants[1]);
	dest.SetVector4("SHAb", shaderConstants[2]);
	dest.SetVector4("SHBr", shaderConstants[3]);
	dest.SetVector4("SHBg", shaderConstants[4]);
	dest.SetVector4("SHBb", shaderConstants[5]);
	dest.SetVector4("SHC", shaderConstants[6]);
	xmlFile.SaveFile("E:\\GitProject\\FlagGG\\Res\\SH.xml");
	printf("x");
}

int main(int argc, const char* argv[])
{
	if (ParseCommand(argv + 1, argc - 1, commandParam))
	{
		Run();
	}

	return 0;
}
