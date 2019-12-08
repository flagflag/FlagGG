#ifndef __WIN_VIEWPORT__
#define __WIN_VIEWPORT__

#include "Export.h"

#include "Graphics/Viewport.h"
#include "Graphics/Batch.h"
#include "Graphics/RenderContext.h"
#include "Core/DeviceEvent.h"
#include "Container/Ptr.h"
#include "Container/Vector.h"

#include <stdint.h>

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

		class Window;

		class FlagGG_API WindowDevice
		{
		public:
			static void Initialize();

			static void Uninitialize();

			static void Update();

			static void Render();

			static void RegisterWinMessage(Window* wv);

			static void UnregisterWinMessage(Window* wv);

			static const wchar_t* className_;

			static Container::Vector<Container::SharedPtr<Window>> recivers_;

			static Container::PODVector<DefferedMessage> defferedMsgs_;
		};

		class FlagGG_API Window : public GPUObject, public Container::RefCounted
		{
		public:
			Window(Core::Context* context, void* parentWindow, const Math::IntRect& rect);

			~Window() override;

			uint32_t GetWidth();

			uint32_t GetHeight();

			Math::IntVector2 GetMousePos() const;

			void Resize(uint32_t width, uint32_t height);

			void* GetWindow();

			void Show();

			void Hide();

			void Render();

			Viewport* GetViewport() const;

			void WinProc(UINT message, WPARAM wParam, LPARAM lParam);

			bool IsValid() override;

		protected:

			void Initialize() override;

			bool Create(void* parentWindow, const Math::IntRect& rect);

			void CreateSwapChain();

			void UpdateSwapChain(uint32_t width, uint32_t height);

		private:
			Core::Context* context_;
			Core::Input* input_;

			ID3D11Texture2D* depthTexture_{ nullptr };
			Container::SharedPtr<Viewport> viewport_;

			uint32_t multiSample_{ 1 };

			bool sRGB_{ true };

			void* window{ nullptr };

			void* parentWindow_{ nullptr };

			Math::IntRect rect_;

			uint32_t vertexSize_{ 0 };
			uint32_t vertexCount_{ 0 };

			POINT mousePos_;
		};
	}
}

#endif
