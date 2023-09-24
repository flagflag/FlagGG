#pragma once

#include "Utility/Singleton.h"

namespace FlagGG
{

template <typename T>
class Subsystem : public Singleton<T>
{
public:
	// TODO
};

template <typename T>
T* GetSubsystem()
{
	return T::InstancePtr();
}

}
