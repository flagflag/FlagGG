#ifndef __DEVICE_EVENT__
#define __DEVICE_EVENT__

#include "Export.h"
#include "Core/Contex.h"
#include "Core/EventDefine.h"
#include "Math/Vector2.h"
#include "Container/RefCounted.h"

namespace FlagGG
{
	namespace Core
	{
		class Context;

		class FlagGG_API KeyState
		{
		public:
			bool OnCtrl();

			bool OnAlt();

			bool OnShift();

		private:

		};

		class FlagGG_API Input : public Container::RefCounted
		{
		public:
			Input(Context* context);

			~Input() override = default;

			void OnKeyDown(KeyState* keyState, uint32_t keyCode);

			void OnKeyUp(KeyState* keyState, uint32_t keyCode);

			void OnMouseDown(KeyState* keyState, MouseKey mouseKey);

			void OnMouseUp(KeyState* keyState, MouseKey mouseKey);

			void OnMouseMove(KeyState* keyState, const Math::Vector2& delta);

		private:
			Context* context_;
		};
	}
}

#endif
