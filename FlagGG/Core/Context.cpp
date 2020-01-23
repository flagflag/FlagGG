#include "Core/Context.h"

namespace FlagGG
{
	namespace Core
	{
#ifdef _WIN32
        template <> void Context::RegisterVariable<Int8>(Int8* variable, const Container::String& variableName)
        {
            
        }

        template <> void Context::RegisterVariable<UInt8>(UInt8* variable, const Container::String& variableName)
        {

        }

        template <> void Context::RegisterVariable<Int16>(Int16* variable, const Container::String& variableName)
        {

        }

        template <> void Context::RegisterVariable<UInt16>(UInt16* variable, const Container::String& variableName)
        {

        }

        template <> void Context::RegisterVariable<Int32>(Int32* variable, const Container::String& variableName)
        {

        }

        template <> void Context::RegisterVariable<UInt32>(UInt32* variable, const Container::String& variableName)
        {

        }

        template <> void Context::RegisterVariable<Int64>(Int64* variable, const Container::String& variableName)
        {

        }

        template <> void Context::RegisterVariable<UInt64>(UInt64* variable, const Container::String& variableName)
        {

        }
#endif
    }
}
