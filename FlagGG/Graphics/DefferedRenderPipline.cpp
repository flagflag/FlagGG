#include "Graphics/RenderPipline.h"

namespace FlagGG
{

DefferedRenderPipline::DefferedRenderPipline()
{

}

DefferedRenderPipline::~DefferedRenderPipline()
{

}

void DefferedRenderPipline::CollectBatch()
{
	CollectLitBatch();

	CollectUnlitBatch();
}

void DefferedRenderPipline::PrepareRender()
{

}

void DefferedRenderPipline::Render()
{

}

}
