#pragma once

#include <stdint.h>

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
	MAX_VERTEX_ELEMENT_SEMANTIC
};

extern uint32_t VERTEX_ELEMENT_TYPE_SIZE[MAX_VERTEX_ELEMENT_TYPE];
