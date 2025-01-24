#pragma once

#include "Core/Function.h"
#include "Core/BaseTypes.h"
#include "Core/AddEvent.h"

#define DEFINE_EVENT(eventId, eventNative) \
	static const UInt32 eventId = FlagGG::AddEvent(__FILE__, #eventId); \
	struct eventId##_HANDLER \
	{ \
		typedef FlagGG::Function<eventNative> FunctionType; \
		static UInt32 GetID() \
		{ \
			static const UInt32 ID = eventId; \
			return ID; \
		} \
	};

#define EVENT_HANDLER(eventId, func, ...) eventId, FlagGG::Function<eventId##_HANDLER::FunctionType::NativeType>(&func, ##__VA_ARGS__)
