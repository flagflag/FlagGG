﻿#pragma once

#include "Export.h"

typedef void* KernelHandle;
#define NULL_HANDLE nullptr

namespace FlagGG
{

class FlagGG_API HandleObject
{
public:
	HandleObject();

	virtual ~HandleObject();

	KernelHandle GetHandler();

	template < typename Type >
	Type* GetObject()
	{
		return static_cast<Type*>(GetHandler());
	}

protected:
	KernelHandle handle_;
};

}
