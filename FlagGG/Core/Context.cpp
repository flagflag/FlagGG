#include "Core/Context.h"

namespace FlagGG
{
	namespace Core
	{
#ifdef WIN32
        template <> void RegisterVariable<int8_t>(int8_t* variable, const Container::String& variableName)
        {
            
        }

        template <> void RegisterVariable<uint8_t>(uint8_t* variable, const Container::String& variableName)
        {

        }

        template <> void RegisterVariable<int16_t>(int16_t* variable, const Container::String& variableName)
        {

        }

        template <> void RegisterVariable<uint16_t>(uint16_t* variable, const Container::String& variableName)
        {

        }

        template <> void RegisterVariable<int32_t>(int32_t* variable, const Container::String& variableName)
        {

        }

        template <> void RegisterVariable<uint32_t>(uint32_t* variable, const Container::String& variableName)
        {

        }

        template <> void RegisterVariable<int64_t>(int64_t* variable, const Container::String& variableName)
        {

        }

        template <> void RegisterVariable<uint64_t>(uint64_t* variable, const Container::String& variableName)
        {

        }
#endif
    }
}
