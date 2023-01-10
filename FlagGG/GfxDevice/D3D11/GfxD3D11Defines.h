#pragma once

#include <d3d11.h>

#define D3D11_SAFE_RELEASE(p) \
	if ((p)) \
	{ \
		((IUnknown*)p)->Release();  p = nullptr; \
	}
