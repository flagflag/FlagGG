#include "Log.h"
#include "Code/Code.h"
#include "Graphics/Material.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include "Graphics/Texture2D.h"
#include "Config/LJSONFile.h"

#include <fstream>

namespace FlagGG
{
	namespace Graphics
	{
		bool Material::BeginLoad(IOFrame::Buffer::IOBuffer* stream)
		{
			Config::LJSONFile file;
			if (!file.LoadFile(stream))
			{
				FLAGGG_LOG_ERROR("Load LJSON failed.");

				return false;
			}



			return true;
		}

		bool Material::EndLoad()
		{
			return true;
		}
	}
}
