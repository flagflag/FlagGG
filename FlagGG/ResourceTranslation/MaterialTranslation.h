#pragma once

#include "Container/Str.h"
#include "Container/Ptr.h"
#include "Container/Vector.h"
#include "Container/HashMap.h"
#include "Graphics/GraphicsDef.h"
#include "ResourceTranslation/TextureTranslation.h"
#include "Resource/Image.h"

namespace FlagGG
{

struct GenericPassDescription
{
	String name_;
	
	String vertexShaderName_;
	String pixelShaderName_;
	String vertexDefines_;
	String pixelDefines_;

	RasterizerState rasterizerState_;
	DepthStencilState depthStencilState_;
};

struct GenericTextureDescription
{
	SharedPtr<Image> image_;
	bool srgb_{};
	TextureFilterMode filterMode_{ TEXTURE_FILTER_DEFAULT };
	TextureAddressMode addresMode_[MAX_TEXTURE_COORDINATE]{};
};

struct GenericMaterialDescription
{
	Vector<GenericPassDescription> passDescs_;
	HashMap<UInt32, GenericTextureDescription> textureDescs_;
	HashMap<String, String> shaderParameters_;
};

}
