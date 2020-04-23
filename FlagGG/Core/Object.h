#pragma once

#include "Export.h"
#include "Core/RTTI.h"

namespace FlagGG
{
	namespace Core
	{
		class FlagGG_API Object : public Container::RefCounted, public RTTIObject
		{
			OBJECT_OVERRIDE(Object, RTTIObject);
		public:
			~Object() override;
		};
	}
}


