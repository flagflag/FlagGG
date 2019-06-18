#include "Log.h"
#include "Code/Code.h"
#include "Graphics/Material.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include "Graphics/Texture2D.h"
#include "Config/LJSONFile.h"
#include "Config/LJSONAux.h"
#include "Resource/ResourceCache.h"
#include "Utility/Format.h"

#include <fstream>

namespace FlagGG
{
	namespace Graphics
	{
		static const char* TEXTURE_CLASS[MAX_TEXTURE_CLASS] =
		{
			"universal",
			"diffuse",
			"normal",
			"specular",
			"emissive",
			"environment"
		};

		TextureClass ToTextureClass(const Container::String& name)
		{
			for (uint32_t i = 0; i < MAX_TEXTURE_CLASS; ++i)
			{
				if (name == TEXTURE_CLASS[i])
				{
					return static_cast<TextureClass>(i);
				}
			}
			if (Utility::Format::IsInterger(name))
			{
				uint32_t index = Utility::Format::ToInt(name);
				if (index < MAX_TEXTURE_CLASS)
				{
					return static_cast<TextureClass>(index);
				}
			}
			return TEXTURE_CLASS_UNIVERSAL;
		}

		Material::Material(Core::Context* context) :
			Resource(context),
			textures_{}
		{ }

		Container::SharedPtr<Texture> Material::GetTexture()
		{
			return textures_[TEXTURE_CLASS_UNIVERSAL];
		}

		Container::SharedPtr<Texture> Material::GetTexture(uint32_t index)
		{
			return index < MAX_TEXTURE_CLASS ? textures_[index] : nullptr;
		}

		Container::SharedPtr<Shader> Material::GetVSShader()
		{
			return vsShader_;
		}

		Container::SharedPtr<Shader> Material::GetPSShader()
		{
			return psShader_;
		}

		static Container::SharedPtr<Texture> LoadTexture(Resource::ResourceCache* cache, const Config::LJSONValue& textureConfig)
		{
			Container::SharedPtr<Texture> texture;
			const Container::String& type = textureConfig["type"].GetString();
			if (type == "texture2d")
			{
				texture = cache->GetResource<Texture2D>(textureConfig["path"].GetString());
				if (!texture)
				{
					FLAGGG_LOG_ERROR("Material ==> load texture failed.");
				}
			}
			else
			{
				// CubeÌùÍ¼
			}
			return texture;
		}

		bool Material::BeginLoad(IOFrame::Buffer::IOBuffer* stream)
		{
			Config::LJSONFile file(context_);
			if (!file.LoadFile(stream))
			{
				FLAGGG_LOG_ERROR("Material ==> load config failed.");

				return false;
			}

			const Config::LJSONValue& root = file.GetRoot();
			if (root.IsObject())
			{
				auto* cache = context_->GetVariable<FlagGG::Resource::ResourceCache>("ResourceCache");
				const auto& textureConfig = root["texture"];
				if (textureConfig.IsObject())
				{
					textures_[TEXTURE_CLASS_UNIVERSAL] = LoadTexture(cache, textureConfig);
				}
				
				const auto& texturesConfig = root["textures"];
				if (texturesConfig.IsObject())
				{
					for (auto& it = texturesConfig.Begin(); it != texturesConfig.End(); ++it)
					{
						const Container::String& name = it->first_;
						TextureClass TC = ToTextureClass(name);
						textures_[TC] = LoadTexture(cache, it->second_);
					}
				}

				Container::Vector<Container::String> defines;

				ShaderCode* vsShaderCode = cache->GetResource<ShaderCode>(root["vsshader"]["path"].GetString());
				if (vsShaderCode)
				{
					Config::ParseStringVector(root["vsshader"]["defines"], defines);
					vsShader_ = vsShaderCode->GetShader(VS, defines);
				}
				else
				{
					FLAGGG_LOG_ERROR("Material ==> load vs shader failed.");
					return false;
				}

				ShaderCode* psShaderCode = cache->GetResource<ShaderCode>(root["psshader"]["path"].GetString());
				if (psShaderCode)
				{
					Config::ParseStringVector(root["psshader"]["defines"], defines);
					psShader_ = psShaderCode->GetShader(PS, defines);
				}
				else
				{
					FLAGGG_LOG_ERROR("Material ==> load ps shader failed.");
					return false;
				}
			}
			else
			{
				FLAGGG_LOG_ERROR("Material ==> config has something wrong.");
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
