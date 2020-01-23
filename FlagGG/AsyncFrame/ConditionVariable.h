#pragma once

#include "Export.h"
#include "AsyncFrame/KernelObject/HandleObject.h"
#include "Core/BaseTypes.h"

namespace FlagGG
{
	namespace AsyncFrame
	{
		class FlagGG_API ConditionVariable : public KernelObject::HandleObject
		{
		public:
			ConditionVariable();

			~ConditionVariable() override;

			void Wait(KernelObject::HandleObject* object);

			void Wait(KernelObject::HandleObject* object, UInt32 waitTime);

			void Trigger();
		};
	}
}
