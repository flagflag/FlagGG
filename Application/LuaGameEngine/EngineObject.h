#pragma once

#include <Core/Object.h>

namespace LuaGameEngine
{
	class LuaGameEngine_API EngineObject : public FlagGG::Core::RTTIObject
	{
		OBJECT_OVERRIDE(EngineObject, RTTIObject);
	public:
		virtual ~EngineObject() = default;

		inline void AddRef() { ++refCount_; }

		inline void ReleaseRef() { --refCount_; }

		inline int GetRef() const { return refCount_; }

		inline bool IsValid() const { return isValid_; }

		inline void SetValid(bool isValid) { isValid_ = isValid; }

	private:
		bool isValid_;

		int refCount_;
	};
}
