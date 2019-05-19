#include "Graphics/GraphicsDef.h"

VertexElement::VertexElement() :
vertexElementType_(VE_INT),
vertexElementSemantic_(SEM_POSITION)
{ }

VertexElement::VertexElement(VertexElementType vertexElementType, VertexElementSemantic vertexElementSemantic, uint8_t index, bool perInstance) :
vertexElementType_(vertexElementType),
vertexElementSemantic_(vertexElementSemantic),
index_(index),
perInstance_(perInstance)
{ }

uint32_t VERTEX_ELEMENT_TYPE_SIZE[MAX_VERTEX_ELEMENT_TYPE] =
{
	sizeof(int32_t),
	sizeof(float),
	2 * sizeof(float),
	3 * sizeof(float),
	4 * sizeof(float),
	4 * sizeof(uint8_t),
	4 * sizeof(uint8_t),
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
	VertexElement(VE_VECTOR3, SEM_POSITION, 0),			// position
	VertexElement(VE_VECTOR3, SEM_NORMAL, 0),			// normal
	VertexElement(VE_UBYTE4, SEM_COLOR, 0),				// color
	VertexElement(VE_VECTOR2, SEM_TEXCOORD, 0),			// 2d texcoord1
	VertexElement(VE_VECTOR2, SEM_TEXCOORD, 1),			// 2d texcoord2
	VertexElement(VE_VECTOR3, SEM_TEXCOORD, 0),			// 3d texcoord1
	VertexElement(VE_VECTOR3, SEM_TEXCOORD, 1),			// 3d texcoord2
	VertexElement(VE_VECTOR4, SEM_TANGENT, 0),			// tangent
	VertexElement(VE_VECTOR4, SEM_BLEND_WEIGHTS, 0),	// blend weights
	VertexElement(VE_UBYTE4, SEM_BLEND_INDICES, 0),		// blend indices
	VertexElement(VE_VECTOR4, SEM_TEXCOORD, 4),			// instance matrix1
	VertexElement(VE_VECTOR4, SEM_TEXCOORD, 5),			// instance matrix2
	VertexElement(VE_VECTOR4, SEM_TEXCOORD, 6),			// instance matrix3
	VertexElement(VE_INT, SEM_OBJECT_INDEX, 0),			// object index
};
