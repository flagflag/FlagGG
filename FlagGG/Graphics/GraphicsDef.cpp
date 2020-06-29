#include "Graphics/GraphicsDef.h"

VertexElement::VertexElement() :
	vertexElementType_(VE_INT),
	vertexElementSemantic_(SEM_POSITION),
	index_(0),
	perInstance_(false),
	offset_(0)
{ }

VertexElement::VertexElement(VertexElementType vertexElementType, VertexElementSemantic vertexElementSemantic, UInt8 index, bool perInstance) :
	vertexElementType_(vertexElementType),
	vertexElementSemantic_(vertexElementSemantic),
	index_(index),
	perInstance_(perInstance),
	offset_(0)
{ }

UInt32 RasterizerState::GetHash() const
{
	return (depthWrite_ ? 1 : 0) |
		((scissorTest_ ? 1 : 0) << 1) |
		((UInt32)fillMode_ << 2) |
		((UInt32)cullMode_ << 4);
}

UInt32 VERTEX_ELEMENT_TYPE_SIZE[MAX_VERTEX_ELEMENT_TYPE] =
{
	sizeof(Int32),
	sizeof(float),
	2 * sizeof(float),
	3 * sizeof(float),
	4 * sizeof(float),
	4 * sizeof(UInt8),
	4 * sizeof(UInt8),
};

FlagGG_API const char* VERTEX_ELEMENT_SEM_NAME[MAX_VERTEX_ELEMENT_SEMANTIC] =
{
	"POSITION",
	"NORMAL",
	"BINORMAL",
	"TANGENT",
	"TEXCOORD",
	"COLOR",
	"BLEND_WEIGHTS",
	"BLEND_INDICES",
	"OBJECTINDEX"
};

VertexElement DEFAULT_VERTEX_ELEMENT[] =
{
	VertexElement(VE_VECTOR3, SEM_POSITION, 0, false),			// position
	VertexElement(VE_VECTOR3, SEM_NORMAL, 0, false),			// normal
	VertexElement(VE_UBYTE4, SEM_COLOR, 0, false),				// color
	VertexElement(VE_VECTOR2, SEM_TEXCOORD, 0, false),			// 2d texcoord1
	VertexElement(VE_VECTOR2, SEM_TEXCOORD, 1, false),			// 2d texcoord2
	VertexElement(VE_VECTOR3, SEM_TEXCOORD, 0, false),			// 3d texcoord1
	VertexElement(VE_VECTOR3, SEM_TEXCOORD, 1, false),			// 3d texcoord2
	VertexElement(VE_VECTOR4, SEM_TANGENT, 0, false),			// tangent
	VertexElement(VE_VECTOR4, SEM_BLEND_WEIGHTS, 0, false),		// blend weights
	VertexElement(VE_UBYTE4, SEM_BLEND_INDICES, 0, false),		// blend indices
	VertexElement(VE_VECTOR4, SEM_TEXCOORD, 4, true),			// instance matrix1
	VertexElement(VE_VECTOR4, SEM_TEXCOORD, 5, true),			// instance matrix2
	VertexElement(VE_VECTOR4, SEM_TEXCOORD, 6, true),			// instance matrix3
	VertexElement(VE_INT, SEM_OBJECT_INDEX, 0, false),			// object index
};

const FlagGG::Container::String SP_WORLD_MATRIX("worldMatrix");
const FlagGG::Container::String SP_VIEW_MATRIX("viewMatrix");
const FlagGG::Container::String SP_PROJVIEW_MATRIX("projviewMatrix");
const FlagGG::Container::String SP_SKIN_MATRICES("skinMatrices");
const FlagGG::Container::String SP_DELTA_TIME("deltaTime");
const FlagGG::Container::String SP_ELAPSED_TIME("elapsedTime");
const FlagGG::Container::String SP_CAMERA_POS("cameraPos");
const FlagGG::Container::String SP_LIGHT_POS("lightPos");
const FlagGG::Container::String SP_LIGHT_DIR("lightDir");
const FlagGG::Container::String SP_LIGHT_VIEW_MATRIX("lightViewMatrix");
const FlagGG::Container::String SP_LIGHT_PROJVIEW_MATRIX("lightProjviewMatrix");

