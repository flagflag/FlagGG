#include "Log.h"
#include "Code/Code.h"
#include "Graphics/Material.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include "Graphics/Texture2D.h"
#include "Graphics/TextureCube.h"
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
	"forward_lit",
	"deferred_base",
	"deferred_lit",
};

static const UInt32 T_INT = StringHash("int").ToHash();
static const UInt32 T_INT32 = StringHash("int32").ToHash();
static const UInt32 T_UINT32 = StringHash("uint32").ToHash();
static const UInt32 T_FLOAT = StringHash("float").ToHash();
static const UInt32 T_COLOR = StringHash("color").ToHash();
static const UInt32 T_RECT = StringHash("rect").ToHash();
static const UInt32 T_VECTOR2 = StringHash("vector2").ToHash();
static const UInt32 T_VECTOR3 = StringHash("vector3").ToHash();
static const UInt32 T_VECTOR4 = StringHash("vector4").ToHash();
static const UInt32 T_MATRIX3 = StringHash("matrix3").ToHash();
static const UInt32 T_MATRIX3X4 = StringHash("matrix3x4").ToHash();
static const UInt32 T_MATRIX4 = StringHash("matrix4").ToHash();

static const HashMap<StringHash, UInt32> TYPE_SIZE =
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

static const HashMap<StringHash, UInt32> TYPE_COUNT =
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

static const HashMap<StringHash, FillMode> FILL_MODE =
{
	{ "FILL_WIREFRAME", FILL_WIREFRAME },
	{ "FILL_SOLID", FILL_SOLID },
};

static const HashMap<StringHash, CullMode> CULL_MODE =
{
	{ "CULL_NONE", CULL_NONE },
	{ "CULL_FRONT",CULL_FRONT },
	{ "CULL_BACK", CULL_BACK },
};

template < class Type >
void ToStream(const LJSONValue& value, UInt32 count, IOFrame::Buffer::IOBuffer* stream)
{
	if (value.IsNumber())
	{
		stream->WriteFloat(static_cast<Type>(value.GetDouble()));
	}
	else if (value.IsArray())
	{
		for (UInt32 i = 0; i < value.Size() && i < count; ++i)
		{
			stream->WriteFloat(static_cast<Type>(value[i].GetDouble()));
		}
	}
}

static void ToBuffer(const String& typeStr, const LJSONValue& value, String& buffer)
{
	UInt32 type = StringHash(typeStr).ToHash();

	static IOFrame::Buffer::StringBuffer steam;
	steam.Clear();
	ToStream<float>(value, *TYPE_COUNT[type], &steam);
	buffer.Resize(*TYPE_SIZE[type]);
	assert(steam.GetSize() <= buffer.Length());
	steam.ClearIndex();
	steam.ReadStream(&buffer[0], steam.GetSize());
}

static TextureClass ToTextureClass(const String& name)
{
	for (UInt32 i = 0; i < MAX_TEXTURE_CLASS; ++i)
	{
		if (name == TEXTURE_CLASS[i])
		{
			return static_cast<TextureClass>(i);
		}
	}
	if (IsInterger(name))
	{
		UInt32 index = ToInt(name);
		if (index < MAX_TEXTURE_CLASS)
		{
			return static_cast<TextureClass>(index);
		}
	}
	return TEXTURE_CLASS_UNIVERSAL;
}

static RenderPassType ToRenderPassType(const String& name)
{
	for (UInt32 i = 0; i < MAX_RENDER_PASS_TYPE; ++i)
	{
		if (name == RENDER_PASS_TYPE[i])
		{
			return static_cast<RenderPassType>(i);
		}
	}
	return RENDER_PASS_TYPE_SHADOW;
}

RenderPassInfo::~RenderPassInfo()
{

}

Material::Material(Context* context) :
	Resource(context),
	textures_{}
{ }

Material::~Material()
{
}

void Material::SetTexture(Texture* texture)
{
	textures_[0] = texture;
}

void Material::SetTexture(TextureClass textureClass, Texture* texture)
{
	textures_[textureClass] = texture;
}

void Material::SetVertexShader(Shader* vertexShader)
{
	vsShader_ = vertexShader;
}

void Material::SetPixelShader(Shader* pixelShader)
{
	psShader_ = pixelShader;
}

void Material::SetRenderPass(RenderPassType type, const RenderPassInfo& renderPass)
{
	renderPass_[type] = renderPass;
}

void Material::SetFillMode(FillMode fillMode)
{
	rasterizerState_.fillMode_ = fillMode;
}

void Material::SetCullMode(CullMode cullMode)
{
	rasterizerState_.cullMode_ = cullMode;
}

void Material::SetDepthWrite(bool depthWrite)
{
	rasterizerState_.depthWrite_ = depthWrite;
}

SharedPtr<Texture> Material::GetTexture()
{
	return textures_[TEXTURE_CLASS_UNIVERSAL];
}

SharedPtr<Texture> Material::GetTexture(UInt32 index)
{
	return index < MAX_TEXTURE_CLASS ? textures_[index] : nullptr;
}

SharedPtr<Shader> Material::GetVertexShader()
{
	return vsShader_;
}

SharedPtr<Shader> Material::GetPixelShader()
{
	return psShader_;
}

HashMap<UInt32, RenderPassInfo>& Material::GetRenderPass()
{
	return renderPass_;
}

SharedPtr<ShaderParameters> Material::GetShaderParameters()
{
	return shaderParameters_;
}

FillMode Material::GetFillMode() const
{
	return rasterizerState_.fillMode_;
}

CullMode Material::GetCullMode() const
{
	return rasterizerState_.cullMode_;
}

bool Material::GetDepthWrite() const
{
	return rasterizerState_.depthWrite_;
}

const RasterizerState Material::GetRasterizerState() const
{
	return rasterizerState_;
}

void Material::CreateShaderParameters()
{
	shaderParameters_ = new ShaderParameters();
}

static SharedPtr<Texture> LoadTexture(ResourceCache* cache, const LJSONValue& textureConfig)
{
	SharedPtr<Texture> texture;
	const String& type = textureConfig["type"].GetString();
	if (type == "texture2d")
	{
		texture = cache->GetResource<Texture2D>(textureConfig["path"].GetString());
		if (!texture)
		{
			FLAGGG_LOG_ERROR("Material ==> load texture2d failed.");
		}
	}
	else if (type == "texturecube")
	{
		texture = cache->GetResource<TextureCube>(textureConfig["path"].GetString());
		if (!texture)
		{
			FLAGGG_LOG_ERROR("Material ==> load textureCube failed.");
		}
	}
	return texture;
}

bool Material::BeginLoad(IOFrame::Buffer::IOBuffer* stream)
{
	LJSONFile file(context_);
	if (!file.LoadFile(stream))
	{
		FLAGGG_LOG_ERROR("Material ==> load config failed.");

		return false;
	}

	const LJSONValue& root = file.GetRoot();
	if (root.IsObject())
	{
		auto* cache = context_->GetVariable<ResourceCache>("ResourceCache");
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
				const String& name = it->first_;
				TextureClass TC = ToTextureClass(name);
				textures_[TC] = LoadTexture(cache, it->second_);
			}
		}

		Vector<String> defines;

		ShaderCode* vsShaderCode = cache->GetResource<ShaderCode>(root["vsshader"]["path"].GetString());
		if (vsShaderCode)
		{
			ParseStringVector(root["vsshader"]["defines"], defines);
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
			ParseStringVector(root["psshader"]["defines"], defines);
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
			const LJSONValue& parameter = root["shader_parameters"];
			if (parameter.IsArray())
			{
				String buffer;
				for (UInt32 i = 0; i < parameter.Size(); ++i)
				{
					const LJSONValue& item = parameter[i];
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
			const LJSONValue& pass = root["pass"];
			if (pass.IsArray())
			{
				for (UInt32 i = 0; i < pass.Size(); ++i)
				{
					RenderPassInfo renderPass;
					vsShaderCode = cache->GetResource<ShaderCode>(pass[i]["vsshader"]["path"].GetString());
					if (vsShaderCode)
					{
						ParseStringVector(pass[i]["vsshader"]["defines"], defines);
						renderPass.vertexShader_ = vsShaderCode->GetShader(VS, defines);
					}

					psShaderCode = cache->GetResource<ShaderCode>(pass[i]["psshader"]["path"].GetString());
					if (psShaderCode)
					{
						ParseStringVector(pass[i]["psshader"]["defines"], defines);
						renderPass.pixelShader_ = psShaderCode->GetShader(PS, defines);
					}

					const String& name = pass[i]["name"].GetString();
					renderPass_.Insert(MakePair(static_cast<UInt32>(ToRenderPassType(name)), renderPass));
				}
			}
		}

		if (root.Contains("depthwrite"))
		{
			rasterizerState_.depthWrite_ = root["depthwrite"].GetBool();
		}

		if (root.Contains("fillmode"))
		{
			rasterizerState_.fillMode_ = *FILL_MODE[root["fillmode"].GetString()];
		}

		if (root.Contains("cullmode"))
		{
			rasterizerState_.cullMode_ = *CULL_MODE[root["cullmode"].GetString()];
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
