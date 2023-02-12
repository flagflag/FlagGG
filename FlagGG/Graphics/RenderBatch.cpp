#include "RenderBatch.h"

namespace FlagGG
{

RenderBatch::RenderBatch()
{

}

RenderBatch::RenderBatch(const RenderContext& renderContext)
	: geometryType_(renderContext.geometryType_)
	, geometry_(renderContext.geometry_)
	, worldTransform_(renderContext.worldTransform_)
	, numWorldTransform_(renderContext.numWorldTransform_)
	, material_(renderContext.material_)
{

}

}