#include "Ultralight/RenderTarget.h"

namespace ultralight
{

RenderTarget::RenderTarget()
	: is_empty(true)
	, width(0u)
	, height(0u)
	, texture_id(0u)
	, texture_width(0u)
	, texture_height(0u)
	, texture_format(BitmapFormat::BGRA8_UNORM_SRGB)
	, uv_coords{}
	, render_buffer_id(0u)
{

}

}
