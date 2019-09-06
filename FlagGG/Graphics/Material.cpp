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

#include "Math/Color.h"
#include "Math/Rect.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Matrix3.h"
#include "Math/Matrix3x4.h"
#include "Math/Matrix4.h"

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

		static const uint32_t T_INT = Container::StringHash("int").ToHash();
		static const uint32_t T_INT32 = Container::StringHash("int32").ToHash();
		static const uint32_t T_UINT32 = Container::StringHash("uint32").ToHash();
		static const uint32_t T_FLOAT = Container::StringHash("float").ToHash();
		static const uint32_t T_COLOR = Container::StringHash("color").ToHash();
		static const uint32_t T_RECT = Container::StringHash("rect").ToHash();
		static const uint32_t T_VECTOR2 = Container::StringHash("vector2").ToHash();
		static const uint32_t T_VECTOR3 = Container::StringHash("vector3").ToHash();
		static const uint32_t T_VECTOR4 = Container::StringHash("vector3").ToHash();
		static const uint32_t T_MATRIX3 = Container::StringHash("matrix3").ToHash();
		static const uint32_t T_MATRIX3X4 = Container::StringHash("matrix3x4").ToHash();
		static const uint32_t T_MATRIX4 = Container::StringHash("matrix4").ToHash();

		static const Container::HashMap<Container::StringHash, uint32_t> TYPE_SIZE =
		{
			{ T_INT,		4 },
			{ T_INT32,		4 },
			{ T_UINT32,		4 },
			{ T_FLOAT,		4 },
			{ T_COLOR,		sizeof(Color) },
			{ T_RECT,		sizeof(Rect) },
			{ T_VECTOR2,	sizeof(Vector2) },
			{ T_VECTOR3,	sizeof(Vector3) },
			{ T_VECTOR4,	sizeof(Vector4) },
			{ T_MATRIX3,	sizeof(Matrix3) },
			{ T_MATRIX3X4,	sizeof(Matrix3x4) },
			{ T_MATRIX4,	sizeof(Matrix4) },
		};

		static void ToBuffer(const Container::String& typeStr, const Config::LJSONValue& value, Container::String& buffer)
		{
			uint32_t type = Container::StringHash(typeStr).ToHash();
			uint32_t size = *TYPE_SIZE[type];
			if (type == T_INT)
			{

			}
			else if (type == T_INT32)
			{

			}
			else if (type == T_UINT32)
			{

			}
			else if (type == T_FLOAT)
			{

			}
			else if (type == T_COLOR)
			{

			}
			else if (type == T_RECT)
			{

			}
			else if()
		}

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

		Container::SharedPtr<Shader> Material::GetVertexShader()
		{
			return vsShader_;
		}

		Container::SharedPtr<Shader> Material::GetPixelShader()
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

				shaderParameters_ = new ShaderParameters();
				if (root.Contains("shader_parameters"))
				{
					const Config::LJSONValue& parameter = root["shader_parameters"];
					if (parameter.IsObject())
					{
						Container::String buffer;
						for (uint32_t i = 0; i < parameter.Size(); ++i)
						{
							const Config::LJSONValue& item = parameter[i];
							auto name = item["name"].GetString();
							auto type = item["type"].GetString().ToLower();

							ToBuffer(type, item["value"], buffer);
							shaderParameters_->AddParametersDefine(name, *TYPE_SIZE[type]);
							shaderParameters_->SetValue(name, buffer.CString(), buffer.Length());
						}
					}
					else
					{
						FLAGGG_LOG_ERROR("Material ==> load shader parameter failed.");
						return false;
					}
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
