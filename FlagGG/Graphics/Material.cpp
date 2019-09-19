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
			"environment",
			"shadowmap"
		};

		static const char* RENDER_PASS_TYPE[MAX_RENDER_PASS_TYPE] =
		{
			"shadow",
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
			{ T_COLOR,		sizeof(Math::Color) },
			{ T_RECT,		sizeof(Math::Rect) },
			{ T_VECTOR2,	sizeof(Math::Vector2) },
			{ T_VECTOR3,	sizeof(Math::Vector3) },
			{ T_VECTOR4,	sizeof(Math::Vector4) },
			{ T_MATRIX3,	sizeof(Math::Matrix3) },
			{ T_MATRIX3X4,	sizeof(Math::Matrix3x4) },
			{ T_MATRIX4,	sizeof(Math::Matrix4) },
		};

		static const Container::HashMap<Container::StringHash, uint32_t> TYPE_COUNT =
		{
			{ T_INT,		1 },
			{ T_INT32,		1 },
			{ T_UINT32,		1 },
			{ T_FLOAT,		1 },
			{ T_COLOR,		4 },
			{ T_RECT,		4 },
			{ T_VECTOR2,	2 },
			{ T_VECTOR3,	3 },
			{ T_VECTOR4,	4 },
			{ T_MATRIX3,	3 * 3 },
			{ T_MATRIX3X4,	3 * 4 },
			{ T_MATRIX4,	4 * 4 },
		};

		template < class Type >
		void ToStream(const Config::LJSONValue& value, uint32_t count, IOFrame::Buffer::IOBuffer* stream)
		{
			if (value.IsNumber())
			{
				stream->WriteFloat(static_cast<Type>(value.GetDouble()));
			}
			else if (value.IsArray())
			{
				for (uint32_t i = 0; i < value.Size() && i < count; ++i)
				{
					stream->WriteFloat(static_cast<Type>(value[i].GetDouble()));
				}
			}
		}

		static void ToBuffer(const Container::String& typeStr, const Config::LJSONValue& value, Container::String& buffer)
		{
			uint32_t type = Container::StringHash(typeStr).ToHash();

			static IOFrame::Buffer::StringBuffer steam;
			steam.Clear();
			ToStream<float>(value, *TYPE_COUNT[type], &steam);
			buffer.Resize(*TYPE_SIZE[type]);
			assert(steam.GetSize() <= buffer.Length());
			steam.ReadStream(&buffer[0], steam.GetSize());
		}

		static TextureClass ToTextureClass(const Container::String& name)
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

		static RenderPassType ToRenderPassType(const Container::String& name)
		{
			for (uint32_t i = 0; i < MAX_RENDER_PASS_TYPE; ++i)
			{
				if (name == RENDER_PASS_TYPE[i])
				{
					return static_cast<RenderPassType>(i);
				}
			}
			return RENDER_PASS_TYPE_SHADOW;
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

		Container::HashMap<RenderPassType, RenderPass>& Material::GetRenderPass()
		{
			return renderPass_;
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

				if (root.Contains("shader_parameters"))
				{
					shaderParameters_ = new ShaderParameters();
					const Config::LJSONValue& parameter = root["shader_parameters"];
					if (parameter.IsArray())
					{
						Container::String buffer;
						for (uint32_t i = 0; i < parameter.Size(); ++i)
						{
							const Config::LJSONValue& item = parameter[i];
							auto name = item["name"].GetString();
							auto type = item["type"].GetString().ToLower();

							ToBuffer(type, item["value"], buffer);
							shaderParameters_->AddParametersDefineImpl(name, *TYPE_SIZE[type]);
							shaderParameters_->SetValueImpl(name, buffer.CString(), buffer.Length());
						}
					}
					else
					{
						FLAGGG_LOG_ERROR("Material ==> load shader parameter failed.");
						return false;
					}
				}

				if (root.Contains("pass"))
				{
					const Config::LJSONValue& pass = root["pass"];
					if (pass.IsArray())
					{
						for (uint32_t i = 0; i < pass.Size(); ++i)
						{
							RenderPass renderPass;
							vsShaderCode = cache->GetResource<ShaderCode>(pass[i]["vsshader"]["path"].GetString());
							if (vsShaderCode)
							{
								Config::ParseStringVector(pass[i]["vsshader"]["defines"], defines);
								renderPass.vertexShader_ = vsShaderCode->GetShader(VS, defines);
							}

							psShaderCode = cache->GetResource<ShaderCode>(pass[i]["psshader"]["path"].GetString());
							if (psShaderCode)
							{
								Config::ParseStringVector(pass[i]["psshader"]["defines"], defines);
								renderPass.pixelShader_ = psShaderCode->GetShader(PS, defines);
							}

							const Container::String& name = pass[i]["name"].GetString();
							renderPass_.Insert(Container::MakePair(ToRenderPassType(name), renderPass));
						}
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
