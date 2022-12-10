#include "Core/EventDefine.h"
#include "Container/HashMap.h"
#include "Container/Str.h"
#include "Utility/SystemHelper.h"

namespace FlagGG
{

UInt32 AddEvent(const char* fileName, const char* eventName)
{
	static HashMap<String, UInt32> eventMap;
	static UInt32 eventCount = 0u;
	static String key;

	key.Clear();
	key.Append(FormatPath(fileName).ToLower());
	key.Append('#');
	key.Append(eventName);

	auto it = eventMap.Find(key);
	if (it != eventMap.End())
		return it->second_;

	++eventCount;
	eventMap.Insert(MakePair(key, eventCount));
	return eventCount;
}

}
