#include <Graphics/AmbientProbeBaker.h>
#include <Config/LJSONFile.h>
#include <Utility/SystemHelper.h>
#include <FileSystem/FileSystem.h>
#include <Resource/Image.h>
#include <Resource/XMLFile.h>
#include <Memory/MemoryHook.h>

using namespace FlagGG;

static LJSONValue CommandParam;

int main(int argc, const char* argv[])
{
	if (ParseCommand(argv + 1, argc - 1, CommandParam))
	{
		const String type = CommandParam["t"].GetString();
		const String inputPath = CommandParam["i"].GetString();
		const String outputPath = CommandParam["o"].GetString();
		if (auto fileStream = GetLocalFileSystem()->OpenFile(inputPath, FileMode::FILE_READ))
		{
			if (type == "bake_sh")
			{
				Image cubemap;
				if (cubemap.LoadStream(fileStream))
				{
					SphericalHarmonicsL2 ambientProbe;
					if (BakeAmbientProbe(&cubemap, ambientProbe))
					{
						FLAGGG_LOG_INFO("success!!!");

						Vector4 shaderConstants[SphericalHarmonicsL2::kVec4Count];
						SphericalHarmonicsL2::GetShaderConstantsFromNormalizedSH(ambientProbe, shaderConstants);

						XMLFile xmlFile;
						XMLElement dest = xmlFile.CreateRoot("root");
						dest.SetVector4("SHAr", shaderConstants[0]);
						dest.SetVector4("SHAg", shaderConstants[1]);
						dest.SetVector4("SHAb", shaderConstants[2]);
						dest.SetVector4("SHBr", shaderConstants[3]);
						dest.SetVector4("SHBg", shaderConstants[4]);
						dest.SetVector4("SHBb", shaderConstants[5]);
						dest.SetVector4("SHC", shaderConstants[6]);
						xmlFile.SaveFile(outputPath);
					}
				}
			}
			else if (type == "bake_cubemap")
			{

			}
		}
	}
	return 0;
}

IMPLEMENT_MODULE_USD("SkyboxBaker");
