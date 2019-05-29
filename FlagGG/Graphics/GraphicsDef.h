#pragma once

#include "Export.h"

#include <stdint.h>

enum PrimitiveType
{
	PRIMITIVE_LINE		= 0,
	PRIMITIVE_TRIANGLE,
};

enum ShaderType
{
	None = 0,
	VS = 1,
	PS = 2
};

enum MaterialQuality : unsigned
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

struct VertexElement
{
	VertexElement();

	VertexElement(VertexElementType vertexElementType, VertexElementSemantic vertexElementSemantic, uint8_t index, bool perInstance = false);

	VertexElementType		vertexElementType_;
	VertexElementSemantic	vertexElementSemantic_;
	uint8_t					index_;
	bool					perInstance_;
	uint32_t				offset_;
};

extern FlagGG_API uint32_t VERTEX_ELEMENT_TYPE_SIZE[MAX_VERTEX_ELEMENT_TYPE];

extern FlagGG_API const char* VERTEX_ELEMENT_SEM_NAME[MAX_VERTEX_ELEMENT_SEMANTIC];

enum
{
	MAX_DEFAULT_VERTEX_ELEMENT = 14,
	MAX_VERTEX_BUFFER_COUNT = 5,
	MAX_CONST_BUFFER_COUNT = 5,
};

extern FlagGG_API VertexElement DEFAULT_VERTEX_ELEMENT[MAX_DEFAULT_VERTEX_ELEMENT];
