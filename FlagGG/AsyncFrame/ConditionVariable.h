#pragma once

#include "Export.h"
#include "AsyncFrame/KernelObject/HandleObject.h"

#include <stdint.h>

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

			void Wait(KernelObject::HandleObject* object, uint32_t waitTime);

			void Trigger();
		};
	}
}
