#pragma once

#include "Export.h"
#include "Core/BaseTypes.h"
#include "AsyncFrame/Mutex.h"

struct PRNGState;
typedef struct PRNGState PRNGState;

namespace FlagGG
{

class FlagGG_API AllocatorLJInstance
{
public:
	AllocatorLJInstance();

	~AllocatorLJInstance();

	void* Malloc(USize size);

	void* Realloc(void* ptr, USize size);

	void Free(void* ptr);

private:
	PRNGState* PRNGState_;
	void* alloctorHandle_;

	Mutex mutex_;
};

}
