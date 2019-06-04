#pragma once

#include "Export.h"
#include "Container/RefCounted.h"
#include "Container/StringHash.h"

namespace FlagGG
{
	namespace Core
	{
		class FlagGG_API Object : public Container::RefCounted
		{
		public:
			virtual Container::StringHash Class() = 0;
		
			virtual const Container::String& ClassName() = 0;
		};
	}
}

#define OBJECT_OVERRIDE(SubClass) \
public: \
	FlagGG::Container::StringHash Class() override { return StaticClass(); } \
	const FlagGG::Container::String& ClassName() override { return StaticClassName(); } \
	static FlagGG::Container::StringHash StaticClass() \
	{ \
		static FlagGG::Container::StringHash staticClass(#SubClass); \
		return staticClass; \
	} \
	static const FlagGG::Container::String& StaticClassName() \
	{ \
		static const FlagGG::Container::String staticClassName(#SubClass); \
		return staticClassName; \
	}
