#pragma once

#include "Core/Object.h"
#include "Core/Subsystem.h"
#include "Container/IoC.h"

namespace FlagGG
{

class ObjectFactory : public Subsystem<ObjectFactory>, public IoC<Object>
{
};

template <typename ObjecType>
struct ObjectFactoryRegister
{
	ObjectFactoryRegister()
	{
		GetSubsystem<ObjectFactory>()->RegisterType<ObjecType>(ObjecType::GetTypeStatic());
	}
};

}

#define REGISTER_TYPE_FACTORY(ObjecType) static FlagGG::ObjectFactoryRegister<ObjecType> HELPER_NAME_CAT(ObjectFactoryRegisterVar, __COUNTER__)
