#include "Graphics/RenderPipline.h"

namespace FlagGG
{

ForwardRenderPipline::ForwardRenderPipline()
{

}

ForwardRenderPipline::~ForwardRenderPipline()
{

}

void ForwardRenderPipline::CollectBatch()
{
	CollectLitBatch();

	CollectUnlitBatch();
}

void ForwardRenderPipline::PrepareRender()
{

}

void ForwardRenderPipline::Render()
{

}

}
