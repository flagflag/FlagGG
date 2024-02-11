#pragma once

#include "Container/Str.h"
#include "ResourceTranslation/TextureTranslation.h"
#include "ResourceTranslation/ShaderTranslation.h"
#include "ResourceTranslation/MaterialTranslation.h"

namespace FlagGG
{

class FlagGG_API ResourceTranslation
{
public:
	virtual ~ResourceTranslation() {}

	virtual SharedPtr<Image> LoadTexture(const String& path) = 0;

	virtual bool LoadShader(const String& path, GenericPassDescription& desc) = 0;

	virtual bool LoadShader(IOFrame::Buffer::IOBuffer* stream, GenericPassDescription& desc) = 0;

	virtual bool LoadMaterial(const String& path, GenericMaterialDescription& desc) = 0;

	virtual bool LoadMaterial(IOFrame::Buffer::IOBuffer* stream, GenericMaterialDescription& desc) = 0;
};

FlagGG_API ResourceTranslation* CreateResourceTranslation(const String& targetEngine);

}
