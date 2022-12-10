#include "Config/LJSONAux.h"

namespace FlagGG
{

bool ParseStringVector(const LJSONValue& value, Vector<String>& result)
{
	result.Clear();

	if (!value.IsArray())
	{
		return false;
	}

	for (UInt32 i = 0; i < value.Size(); ++i)
	{
		const LJSONValue& item = value[i];
		result.Push(item.GetString());
	}
}

}
