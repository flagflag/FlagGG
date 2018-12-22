#ifndef __WIN_VIEWPORT__
#define __WIN_VIEWPORT__

#include "Viewport.h"

namespace FlagGG
{
	namespace Graphics
	{
		class WindowDevice
		{
		public:
			static void Initialize();

			static void Uninitialize();

			static void Update();

			static const wchar_t* className_;
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

			void Render(const unsigned char* vertexs, unsigned vertexSize, unsigned vertexCount);

		private:
			void UpdateVertexData(const unsigned char* vertexs, unsigned vertexSize, unsigned vertexCount);

			void* window;

			void* parentWindow_;

			ID3D11Buffer* d3d11Buffer_;

			unsigned vertexSize_;
			unsigned vertexCount_;
		};
	}
}

#endif
