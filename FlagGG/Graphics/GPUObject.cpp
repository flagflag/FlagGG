#include "GPUObject.h"

#include "RenderEngine.h"

namespace FlagGG
{

GPUObject::GPUObject() :
	gpuHandler_(nullptr)
{
}

GPUObject::GPUObject(GPUHandler handler) :
	gpuHandler_(handler)
{
}

GPUObject::~GPUObject()
{
	SAFE_RELEASE(gpuHandler_);
}

GPUHandler GPUObject::GetHandler()
{
	return gpuHandler_;
}

void GPUObject::ResetHandler(GPUHandler handler)
{
	SAFE_RELEASE(gpuHandler_);
	gpuHandler_ = handler;
}

}
