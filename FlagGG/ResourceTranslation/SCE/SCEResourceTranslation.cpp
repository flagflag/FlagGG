#include "SCEResourceTranslation.h"
#include "Resource/XMLFile.h"
#include "Resource/ResourceCache.h"
#include "Resource/Image.h"
#include "IOFrame/Stream/FileStream.h"
#include "Utility/SystemHelper.h"
#include "Utility/Format.h"
#include "Log.h"

namespace FlagGG
{

static const HashMap<StringHash, BlendMode> SCE_BLEND_MODE =
{
	{ "replace",                 BLEND_REPLACE },
	{ "add",                     BLEND_ADD },
	{ "multiply",                BLEND_UNKNOWN },
	{ "alpha",                   BLEND_ALPHA },
	{ "addalpha",                BLEND_ALPHA_ADD},
	{ "premulalpha",             BLEND_UNKNOWN },
	{ "invdestalpha",            BLEND_UNKNOWN},
	{ "subtract",                BLEND_UNKNOWN},
	{ "subtractalpha",           BLEND_UNKNOWN },
	{ "srcdestalpha",	         BLEND_UNKNOWN }, // 自定义混合模式（ColorA.RGB * ColorA.A + ColorB.RGB * ColorB.A）
	{ "off_screen_alpha_src",    BLEND_UNKNOWN },
	{ "off_screen_alpha_dst",    BLEND_UNKNOWN },
	{ "off_screen_additive_dst", BLEND_UNKNOWN },
};

static const HashMap<StringHash, CullMode> SCE_CULL_MODE =
{
	{ "none", CULL_NONE },
	{ "ccw",  CULL_FRONT },
	{ "cw",   CULL_BACK },
};

static const HashMap<StringHash, TextureAddressMode> SCE_TEXTURE_ADDRESS_MODE =
{
	{ "wrap",   TEXTURE_ADDRESS_WRAP },
	{ "mirror", TEXTURE_ADDRESS_MIRROR },
	{ "clamp",  TEXTURE_ADDRESS_CLAMP },
	{ "border", TEXTURE_ADDRESS_BORDER },
};

static const HashMap<StringHash, TextureFilterMode> SCE_TEXTURE_FILTER_MODE =
{
	{ "nearest",            TEXTURE_FILTER_NEAREST },
	{ "bilinear",           TEXTURE_FILTER_BILINEAR },
	{ "trilinear",          TEXTURE_FILTER_TRILINEAR },
	{ "anisotropic",        TEXTURE_FILTER_ANISOTROPIC },
	{ "nearestanisotropic", TEXTURE_FILTER_NEAREST_ANISOTROPIC },
};

static const HashMap<StringHash, ComparisonFunc> SCE_COMPARISON_FUNC =
{
	{ "always",       COMPARISON_ALWAYS },
	{ "equal",        COMPARISON_EQUAL },
	{ "notequal",     COMPARISON_NOT_EQUAL },
	{ "less",         COMPARISON_LESS },
	{ "lessequal",    COMPARISON_LESS_EQUAL },
	{ "greater",      COMPARISON_GREATER },
	{ "greaterequal", COMPARISON_GREATER_EQUAL },
};

static const char* textureUnitNames[] =
{
	"diffuse",          // 0
	"normal",           // 1
	"specular",         // 2
	"emissive",         // 3
	// "environment",
	"envdiffuse",       // 4
	"envspecular",      // 5
	// "volume",
	"custom1",          // 6
	// "custom2",
	"cloudshadowmap",   // 7
	"lightramp",        // 8
	"lightshape",       // 9
	"shadowmap",        // 10
	"faceselect",       // 11
	"indirection",      // 12
	"depth",            // 13
	// "light",
	"aomap",            // 14
	"zone",             // 15
	nullptr
};

static int ParseTextureUnitName(String name)
{
	name = name.ToLower().Trimmed();
	int unit = GetStringListIndex(name.CString(), textureUnitNames, 16);

	if (unit == 16)
	{
		// Check also for shorthand names
		if (name == "diff")
			unit = 0;
		else if (name == "albedo")
			unit = 0;
		else if (name == "norm")
			unit = 1;
		else if (name == "spec")
			unit = 2;
		else if (name == "env")
			unit = 4;
		// Finally check for specifying the texture unit directly as a number
		else if (name.Length() < 3)
			unit = Clamp(ToInt(name), 0, 15);
	}

	if (unit == 16)
		FLAGGG_LOG_ERROR("Unknown texture unit name " + name);
	return unit;
}

SCEResourceTranslation::SCEResourceTranslation()
{

}

SCEResourceTranslation::~SCEResourceTranslation()
{

}

SharedPtr<Image> SCEResourceTranslation::LoadTexture(const String& path)
{
	return GetSubsystem<ResourceCache>()->GetResource<Image>(path);
}

bool SCEResourceTranslation::LoadShader(const String& path, GenericPassDescription& desc)
{
	return false;
}

bool SCEResourceTranslation::LoadShader(IOFrame::Buffer::IOBuffer* stream, GenericPassDescription& desc)
{
	return false;
}

bool SCEResourceTranslation::LoadMaterial(const String& path, GenericMaterialDescription& desc)
{
	IOFrame::Stream::FileStream fileStream;
	fileStream.Open(path, FileMode::FILE_READ);
	if (!fileStream.IsOpen())
	{
		FLAGGG_LOG_STD_ERROR("Failed to open file[%s].", path.CString());
		return false;
	}

	return LoadMaterial(&fileStream, desc);
}

bool SCEResourceTranslation::LoadMaterial(IOFrame::Buffer::IOBuffer* stream, GenericMaterialDescription& desc)
{
	XMLFile xmlFile;
	if (!xmlFile.LoadStream(stream))
	{
		FLAGGG_LOG_STD_ERROR("Failed to load xml file.");
		return false;
	}

	return LoadMaterialImpl(xmlFile.GetRoot(), desc);
}

bool SCEResourceTranslation::LoadTextureDesc(const XMLElement& source, GenericTextureDescription& desc)
{
	if (XMLElement srgbParam = source.GetChild("srgb"))
	{
		desc.srgb_ = srgbParam.GetBool("enable");
	}

	for (XMLElement addressParam = source.GetChild("address"); addressParam; addressParam = addressParam.GetNext("address"))
	{
		const String coord = addressParam.GetAttribute("coord");
		const String mode = addressParam.GetAttribute("mode");
		if (coord == "u")
		{
			desc.addresMode_[TEXTURE_COORDINATE_U] = *SCE_TEXTURE_ADDRESS_MODE[mode];
		}
		else if (coord == "v")
		{
			desc.addresMode_[TEXTURE_COORDINATE_V] = *SCE_TEXTURE_ADDRESS_MODE[mode];
		}
		else if (coord == "w")
		{
			desc.addresMode_[TEXTURE_COORDINATE_W] = *SCE_TEXTURE_ADDRESS_MODE[mode];
		}
	}

	if (XMLElement filterParam = source.GetChild("filter"))
	{
		const String mode = filterParam.GetAttribute("mode");
		desc.filterMode_ = *SCE_TEXTURE_FILTER_MODE[mode];
	}

	return true;
}

bool SCEResourceTranslation::LoadMaterialImpl(const XMLElement& source, GenericMaterialDescription& desc)
{
	if (source.IsNull())
	{
		FLAGGG_LOG_ERROR("Can not load material from null XML element");
		return false;
	}

	auto* cache = GetSubsystem<ResourceCache>();

	XMLElement shaderElem = source.GetChild("shader");
	if (shaderElem)
	{
		const String vertexShaderDefines = shaderElem.GetAttribute("vsdefines");
		const String pixelShaderDefines = shaderElem.GetAttribute("psdefines");
	}

	XMLElement techniqueElem = source.GetChild("technique");
	if (techniqueElem)
	{
		const String techniquePath = techniqueElem.GetAttribute("name");
		auto techXmlFile = cache->GetResource<XMLFile>(techniquePath);
		if (techXmlFile)
		{
			if (!LoadMaterialImpl_PassStep1(techXmlFile->GetRoot(), desc))
			{
				return false;
			}
		}
	}

	XMLElement passShaderElem = source.GetChild("passshader");
	while (passShaderElem)
	{
		if (!LoadMaterialImpl_PassStep2(passShaderElem, desc))
		{
			return false;
		}
		passShaderElem = passShaderElem.GetNext("passshader");
	}

	XMLElement textureElem = source.GetChild("texture");
	while (textureElem)
	{
		UInt32 unit = 0;
		if (textureElem.HasAttribute("unit"))
			unit = ParseTextureUnitName(textureElem.GetAttribute("unit"));
		if (unit < 16)
		{
			const String name = textureElem.GetAttribute("name");
			auto image = cache->GetResource<Image>(name);
			if (image)
			{
				auto& textureDesc = desc.textureDescs_[unit];
				textureDesc.image_ = image;
				if (auto textureXml = cache->GetResource<XMLFile>(ReplaceExtension(name, ".xml")))
				{
					LoadTextureDesc(textureXml->GetRoot(), textureDesc);
				}
			}
			else
			{
				ASSERT_MESSAGE(false, "Can not load image");
				FLAGGG_LOG_STD_ERROR("Can not load image[%s].", name.CString());
			}
		}
		textureElem = textureElem.GetNext("texture");
	}

	XMLElement param = source.GetChild("parameter");
	while (param)
	{
		const String name = param.GetAttribute("name");
		const String value = param.GetAttribute("value");
		desc.shaderParameters_.Insert(MakePair(name, value));
		param = param.GetNext("parameter");
	}

	return true;
}

static GenericPassDescription& GetOrCreatePassDesc(GenericMaterialDescription& desc, const String& name)
{
	for (auto& passDesc : desc.passDescs_)
	{
		if (passDesc.name_ == name)
			return passDesc;
	}
	auto& passDesc = desc.passDescs_.EmplaceBack();
	passDesc.name_ = name;
	return passDesc;
}

bool SCEResourceTranslation::LoadMaterialImpl_PassStep1(const XMLElement& source, GenericMaterialDescription& desc)
{
	String globalVS = source.GetAttribute("vs");
	String globalPS = source.GetAttribute("ps");
	String globalVSDefines = source.GetAttribute("vsdefines");
	String globalPSDefines = source.GetAttribute("psdefines");

	XMLElement passElem = source.GetChild("pass");
	while (passElem)
	{
		if (!passElem.HasAttribute("name"))
		{
			FLAGGG_LOG_ERROR("Missing pass name");
			return false;
		}

		GenericPassDescription& passDesc = GetOrCreatePassDesc(desc, passElem.GetAttribute("name"));

		// Append global defines only when pass does not redefine the shader
		if (passElem.HasAttribute("vs"))
		{
			passDesc.vertexShaderName_ = passElem.GetAttribute("vs");
			passDesc.vertexDefines_ = passElem.GetAttribute("vsdefines");
		}
		else
		{
			passDesc.vertexShaderName_ = globalVS;
			passDesc.vertexDefines_ = globalVSDefines + passElem.GetAttribute("vsdefines");
		}
		if (passElem.HasAttribute("ps"))
		{
			passDesc.pixelShaderName_ = passElem.GetAttribute("ps");
			passDesc.pixelDefines_ = passElem.GetAttribute("psdefines");
		}
		else
		{
			passDesc.pixelShaderName_ = globalPS;
			passDesc.pixelDefines_ = globalPSDefines + passElem.GetAttribute("psdefines");
		}

		// passElem.GetAttribute("vsexcludes");
		// passElem.GetAttribute("psexcludes");

		if (passElem.HasAttribute("blend"))
		{
			const String blend = passElem.GetAttributeLower("blend");
			if (auto* blendMode = SCE_BLEND_MODE[blend])
				passDesc.rasterizerState_.blendMode_ = *blendMode;
			else
				passDesc.rasterizerState_.blendMode_ = BLEND_REPLACE;
		}
		else
			passDesc.rasterizerState_.blendMode_ = BLEND_REPLACE;

		if (passElem.HasAttribute("cull"))
		{
			const String cull = passElem.GetAttributeLower("cull");
			if (auto* cullMode = SCE_CULL_MODE[cull])
				passDesc.rasterizerState_.cullMode_ = *cullMode;
			else
				passDesc.rasterizerState_.cullMode_ = CULL_BACK;
		}
		else
		{
			passDesc.rasterizerState_.cullMode_ = CULL_BACK;
		}

		if (passElem.HasAttribute("depthtest"))
		{
			const String depthTest = passElem.GetAttributeLower("depthtest");
			if (depthTest == "false")
				passDesc.depthStencilState_.depthTestMode_ = COMPARISON_ALWAYS;
			else if (auto* compFunc = SCE_COMPARISON_FUNC[depthTest])
				passDesc.depthStencilState_.depthTestMode_ = *compFunc;
			else
				passDesc.depthStencilState_.depthTestMode_ = COMPARISON_LESS;
		}
		else
		{
			passDesc.depthStencilState_.depthTestMode_ = COMPARISON_LESS;
		}

		if (passElem.HasAttribute("colorwrite"))
			passDesc.rasterizerState_.colorWrite_ = passElem.GetBool("colorwrite");

		if (passElem.HasAttribute("depthwrite"))
			passDesc.depthStencilState_.depthWrite_ = passElem.GetBool("depthwrite");

		passElem = passElem.GetNext("pass");
	}

	return true;
}

bool SCEResourceTranslation::LoadMaterialImpl_PassStep2(const XMLElement& source, GenericMaterialDescription& desc)
{
	String passName = source.GetAttribute("pass");
	if (passName.Empty())
	{
		return false;
	}

	String vsDefines = source.GetAttribute("vsdefines");
	String psDefines = source.GetAttribute("psdefines");

	GenericPassDescription& passDesc = GetOrCreatePassDesc(desc, source.GetAttribute("pass"));

	passDesc.vertexDefines_ = passDesc.vertexDefines_ + " " + vsDefines;
	passDesc.pixelDefines_ = passDesc.pixelDefines_ + " " + psDefines;

	//if (source.HasAttribute("blendmode"))
	//{
	//	passDesc.rasterizerState_.blendMode_ = (BlendMode)source.GetInt("blendmode");
	//}

	if (source.HasAttribute("depthwrite"))
	{
		passDesc.depthStencilState_.depthWrite_ = source.GetBool("depthwrite");
	}

	return true;
}

}
