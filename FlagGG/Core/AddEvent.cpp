#include "Core/EventDefine.h"
#include "Container/HashMap.h"
#include "Container/Str.h"
#include "Utility/SystemHelper.h"

namespace FlagGG
{
	namespace Core
	{
		UInt32 AddEvent(const char* fileName, const char* eventName)
		{
			static Container::HashMap<Container::String, UInt32> eventMap;
			static UInt32 eventCount = 0u;
			static Container::String key;

			key.Clear();
			key.Append(Utility::SystemHelper::FormatPath(fileName).ToLower());
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
}
