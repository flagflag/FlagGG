#include "RenderContext.h"

namespace FlagGG
{

RenderBatch::RenderBatch()
{

}

RenderBatch::RenderBatch(const RenderContext& renderContext)
	: geometryType_(renderContext.geometryType_)
{

}

}
