#include "Resource/Resource.h"

namespace FlagGG
{

Resource::Resource()
{ }
		
void Resource::SetName(const String& name)
{
	name_ = name;
}

void Resource::SetMemoryUse(UInt32 memoryUse)
{
	memoryUse_ = memoryUse;
}

UInt32 Resource::GetMemoryUse() const
{
	return memoryUse_;
}

}
