#pragma once

#include "Export.h"
#include "AsyncFrame/KernelObject/HandleObject.h"
#include "Core/BaseTypes.h"

namespace FlagGG
{

class FlagGG_API ConditionVariable : public HandleObject
{
public:
	ConditionVariable();

	~ConditionVariable() override;

	void Wait(HandleObject* object);

	void Wait(HandleObject* object, UInt32 waitTime);

	void Trigger();
};

}
