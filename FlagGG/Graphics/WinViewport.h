#ifndef __WIN_VIEWPORT__
#define __WIN_VIEWPORT__

#include "Viewport.h"
#include "Batch.h"
#include "RenderContext.h"

#include <set>

namespace FlagGG
{
	namespace Graphics
	{
		struct DefferedMessage
		{
			HWND handler_;
			UINT message_;
			WPARAM wParam_;
			LPARAM lParam_;
		};

		class WinViewport;

		class WindowDevice
		{
		public:
			static void Initialize();

			static void Uninitialize();

			static void Update();

			static void RegisterWinMessage(WinViewport* wv);

			static void UnregisterWinMessage(WinViewport* wv);

			static const wchar_t* className_;

			static std::set<WinViewport*> recivers_;

			static std::vector<DefferedMessage> defferedMsgs_;
		};

		class KeyState
		{
		public:
			bool OnCtrl();

			bool OnAlt();

			bool OnShift();

		private:
			
		};

		enum MouseKey
		{
			MOUSE_LEFT = 0,
			MOUSE_MID,
			MOUSE_RIGHT,
		};

		class Input
		{
		public:
			virtual ~Input() = default;

			virtual void OnKeyDown(KeyState* keyState, unsigned keyCode) = 0;

			virtual void OnKeyUp(KeyState* keyState, unsigned keyCode) = 0;

			virtual void OnMouseDown(KeyState* keyState, MouseKey mouseKey) = 0;

			virtual void OnMouseUp(KeyState* keyState, MouseKey mouseKey) = 0;

			virtual void OnMouseMove(KeyState* keyState, const Math::Vector2& delta) = 0;
		};

		class WinViewport : public Viewport
		{
		public:
			WinViewport(void* parentWindow, unsigned x, unsigned y, unsigned width, unsigned height);

			~WinViewport() override = default;

			unsigned GetWidth() override;

			unsigned GetHeight() override;

			void Resize(unsigned width, unsigned height) override;

			void* GetWindow() override;

			void Show();

			void Hide();

			void Render(const RenderContext* context);

			void WinProc(UINT message, WPARAM wParam, LPARAM lParam);

			void SetInput(Input* input);

		private:
			void UpdateVertexData(const unsigned char* vertexs, unsigned vertexSize, unsigned vertexCount);

			void* window;

			void* parentWindow_;

			ID3D11Buffer* d3d11Buffer_;

			unsigned vertexSize_;
			unsigned vertexCount_;

			Input* input_{ nullptr };

			POINT mousePos_;
		};
	}
}

#endif
