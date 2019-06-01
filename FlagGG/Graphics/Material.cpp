#include "Log.h"
#include "Code/Code.h"
#include "Graphics/Material.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include "Graphics/Texture2D.h"
#include "Config/LJSONFile.h"
#include "Config/LJSONAux.h"
#include "Resource/ResourceCache.h"

#include <fstream>

namespace FlagGG
{
	namespace Graphics
	{
		Material::Material(Core::Context* context) :
			Resource(context)
		{ }

		Container::SharedPtr<Texture> Material::GetTexture()
		{
			return texture_;
		}

		Container::SharedPtr<Shader> Material::GetVSShader()
		{
			return vsShader_;
		}

		Container::SharedPtr<Shader> Material::GetPSShader()
		{
			return psShader_;
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
				if (root["texture"].IsObject())
				{
					const Container::String& type = root["texture"]["type"].GetString();
					if (type == "texture2d")
					{
						texture_ = cache->GetResource<Texture2D>(root["texture"]["path"].GetString());
						if (!texture_)
						{
							FLAGGG_LOG_ERROR("Material ==> load texture failed.");
							return false;
						}
					}
					else
					{
						// Cube贴图
						return false; // 暂时没有Cube贴图，直接返回失败
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
