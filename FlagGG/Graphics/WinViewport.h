#ifndef __WIN_VIEWPORT__
#define __WIN_VIEWPORT__

#include "Export.h"

#include "Viewport.h"
#include "Batch.h"
#include "RenderContext.h"
#include "Core/DeviceEvent.h"

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

		class FlagGG_API WindowDevice
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

		class FlagGG_API WinViewport : public Viewport
		{
		public:
			WinViewport(Core::Context* context, void* parentWindow, unsigned x, unsigned y, unsigned width, unsigned height);

			~WinViewport() override = default;

			unsigned GetWidth() override;

			unsigned GetHeight() override;

			void Resize(unsigned width, unsigned height) override;

			void* GetWindow() override;

			void Show();

			void Hide();

			void Render(const RenderContext* context);

			void WinProc(UINT message, WPARAM wParam, LPARAM lParam);

		private:
			void UpdateVertexData(const unsigned char* vertexs, unsigned vertexSize, unsigned vertexCount);

			void* window{ nullptr };

			void* parentWindow_{ nullptr };

			ID3D11Buffer* d3d11Buffer_{ nullptr };

			unsigned vertexSize_{ 0 };
			unsigned vertexCount_{ 0 };

			Core::Input* input_;

			POINT mousePos_;
		};
	}
}

#endif
