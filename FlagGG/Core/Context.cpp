#include "Core/Context.h"

namespace FlagGG
{
	namespace Core
	{
#ifdef _WIN32
        template <> void Context::RegisterVariable<int8_t>(int8_t* variable, const Container::String& variableName)
        {
            
        }

        template <> void Context::RegisterVariable<uint8_t>(uint8_t* variable, const Container::String& variableName)
        {

        }

        template <> void Context::RegisterVariable<int16_t>(int16_t* variable, const Container::String& variableName)
        {

        }

        template <> void Context::RegisterVariable<uint16_t>(uint16_t* variable, const Container::String& variableName)
        {

        }

        template <> void Context::RegisterVariable<int32_t>(int32_t* variable, const Container::String& variableName)
        {

        }

        template <> void Context::RegisterVariable<uint32_t>(uint32_t* variable, const Container::String& variableName)
        {

        }

        template <> void Context::RegisterVariable<int64_t>(int64_t* variable, const Container::String& variableName)
        {

        }

        template <> void Context::RegisterVariable<uint64_t>(uint64_t* variable, const Container::String& variableName)
        {

        }
#endif
    }
}
