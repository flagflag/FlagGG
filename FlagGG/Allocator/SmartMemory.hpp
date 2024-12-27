#pragma once

#include "Export.h"
#include "Memory/Memory.h"

#include <stdlib.h>

namespace FlagGG
{

template <typename Type = char>
class FlagGG_API SmartMemory
{
public:
	SmartMemory(size_t count,
	Type* defaultMemory = nullptr)
		: memory_(nullptr)
	{
		if (count > 0)
		{
			if (defaultMemory)
			{
				memory_ = defaultMemory;
			}
			else
			{
				memory_ = (Type*)GetSubsystem<Memory>()->Malloc(count * (sizeof (Type)));
			}

			default_ = !!defaultMemory;
		}
	}

	virtual ~SmartMemory()
	{
		if (memory_ && !default_)
		{
			GetSubsystem<Memory>()->Free(memory_);
		}
	}

	Type* Get()
	{
		return memory_;
	}

private:
	Type* memory_;

	bool default_;
};

}
