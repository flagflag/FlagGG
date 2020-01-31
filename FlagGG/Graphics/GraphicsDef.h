#pragma once

#include "Export.h"
#include "Container/StringHash.h"
#include "Core/BaseTypes.h"

enum PrimitiveType
{
	PRIMITIVE_TRIANGLE	= 0,
	PRIMITIVE_LINE,
};

enum ShaderType
{
	None = 0,
	VS = 1,
	PS = 2
};

enum TextureClass : UInt32
{
	TEXTURE_CLASS_UNIVERSAL = 0,
	TEXTURE_CLASS_DIFFUSE,
	TEXTURE_CLASS_NORMAL,
	TEXTURE_CLASS_SPECULAR,
	TEXTURE_CLASS_EMISSIVE,
	TEXTURE_CLASS_ENVIRONMENT,
	TEXTURE_CLASS_SHADOWMAP,
	MAX_TEXTURE_CLASS
};

enum CubeMapFace
{
	FACE_POSITIVE_X = 0,
	FACE_NEGATIVE_X,
	FACE_POSITIVE_Y,
	FACE_NEGATIVE_Y,
	FACE_POSITIVE_Z,
	FACE_NEGATIVE_Z,
	MAX_CUBEMAP_FACES
};

enum RenderPassType : UInt32
{
	RENDER_PASS_TYPE_SHADOW = 0,
	MAX_RENDER_PASS_TYPE,
};

enum MaterialQuality : UInt32
{
	QUALITY_LOW = 0,
	QUALITY_MEDIUM = 1,
	QUALITY_HIGH = 2,
	QUALITY_MAX = 15,
};

enum VertexElementType
{
	VE_INT = 0,
	VE_FLAOT,
	VE_VECTOR2,
	VE_VECTOR3,
	VE_VECTOR4,
	VE_UBYTE4,
	VE_UBYTE4_UNORM,
	MAX_VERTEX_ELEMENT_TYPE
};

enum VertexElementSemantic
{
	SEM_POSITION = 0,
	SEM_NORMAL,
	SEN_BINORMAL,
	SEM_TANGENT,
	SEM_TEXCOORD,
	SEM_COLOR,
	SEM_BLEND_WEIGHTS,
	SEM_BLEND_INDICES,
	SEM_OBJECT_INDEX,
	MAX_VERTEX_ELEMENT_SEMANTIC
};

enum GeometryType : UInt32
{
	GEOMETRY_STATIC = 0,
	GEOMETRY_SKINNED,
	GEOMETRY_INSTANCE,
	GEOMETRY_BILLBOARD,
};

enum FillMode
{
	FILL_WIREFRAME = 0,
	FILL_SOLID,
};

enum CullMode
{
	CULL_NONE = 0,
	CULL_FRONT,
	CULL_BACK,
};

struct FlagGG_API VertexElement
{
	VertexElement();

	VertexElement(VertexElementType vertexElementType, VertexElementSemantic vertexElementSemantic, UInt8 index = 0, bool perInstance = false);

	bool operator ==(const VertexElement& rhs) const
	{
		return vertexElementType_ == rhs.vertexElementType_ &&
			vertexElementSemantic_ == rhs.vertexElementSemantic_ &&
			index_ == rhs.index_ &&
			perInstance_ == rhs.perInstance_;
	}

	bool operator !=(const VertexElement& rhs) const { return !(*this == rhs); }

	VertexElementType		vertexElementType_;
	VertexElementSemantic	vertexElementSemantic_;
	UInt8					index_;
	bool					perInstance_;
	UInt32					offset_;
};

struct FlagGG_API RasterizerState
{
	bool depthWrite_{ true };
	bool scissorTest_{ false };
	FillMode fillMode_{ FILL_SOLID };
	CullMode cullMode_{ CULL_NONE };

	UInt32 GetHash() const;
};

extern FlagGG_API UInt32 VERTEX_ELEMENT_TYPE_SIZE[MAX_VERTEX_ELEMENT_TYPE];

extern FlagGG_API const char* VERTEX_ELEMENT_SEM_NAME[MAX_VERTEX_ELEMENT_SEMANTIC];

enum
{
	MAX_DEFAULT_VERTEX_ELEMENT = 14,
	MAX_VERTEX_BUFFER_COUNT = 5,
	MAX_CONST_BUFFER_COUNT = 5,
};

extern FlagGG_API VertexElement DEFAULT_VERTEX_ELEMENT[MAX_DEFAULT_VERTEX_ELEMENT];

extern FlagGG_API const FlagGG::Container::StringHash SP_WORLD_MATRIX;
extern FlagGG_API const FlagGG::Container::StringHash SP_VIEW_MATRIX;
extern FlagGG_API const FlagGG::Container::StringHash SP_PROJVIEW_MATRIX;
extern FlagGG_API const FlagGG::Container::StringHash SP_SKIN_MATRICES;
extern FlagGG_API const FlagGG::Container::StringHash SP_DELTA_TIME;
extern FlagGG_API const FlagGG::Container::StringHash SP_ELAPSED_TIME;
extern FlagGG_API const FlagGG::Container::StringHash SP_CAMERA_POS;
extern FlagGG_API const FlagGG::Container::StringHash SP_LIGHT_POS;
extern FlagGG_API const FlagGG::Container::StringHash SP_LIGHT_DIR;
extern FlagGG_API const FlagGG::Container::StringHash SP_LIGHT_VIEW_MATRIX;
extern FlagGG_API const FlagGG::Container::StringHash SP_LIGHT_PROJVIEW_MATRIX;
