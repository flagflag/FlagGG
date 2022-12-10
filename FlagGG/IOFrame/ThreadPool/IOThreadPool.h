#pragma once

#include "AsyncFrame/KernelObject/Runtime.h"
#include "Container/RefCounted.h"
#include "Container/Ptr.h"
#include "Export.h"

namespace FlagGG { namespace IOFrame {

class FlagGG_API IOThreadPool : public Runtime, public RefCounted
{
public:
	virtual void Start() = 0;
};

typedef SharedPtr<IOThreadPool> IOThreadPoolPtr;

}}
