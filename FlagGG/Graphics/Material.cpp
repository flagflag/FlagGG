#include "Log.h"
#include "Code/Code.h"
#include "Graphics/Material.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include "Graphics/Texture2D.h"
#include "Config/LJSONFile.h"
#include "Resource/ResourceCache.h"

#include <fstream>

namespace FlagGG
{
	namespace Graphics
	{
		bool Material::BeginLoad(IOFrame::Buffer::IOBuffer* stream)
		{
			Config::LJSONFile file(context_);
			if (!file.LoadFile(stream))
			{
				FLAGGG_LOG_ERROR("Load LJSON failed.");

				return false;
			}

			const Config::LJSONValue& root = file.GetRoot();
			if (root.IsObject())
			{
				auto* cache = context_->GetVariable<FlagGG::Resource::ResourceCache>("ResourceCache");
				texture_ = cache->GetResource<Texture>(root["texture"].GetString());
				shader_ = cache->GetResource<Shader>(root["shader"].GetString());
			}
			else
			{
				FLAGGG_LOG_ERROR("Material config has something wrong.");
			}

			return true;
		}

		bool Material::EndLoad()
		{
			return true;
		}
	}
}
