#ifndef __DIRECT3D_WINDOW__
#define __DIRECT3D_WINDOW__

#include <d3d11.h>
#include <windows.h>

namespace FlagGG
{
	namespace TestGraphics
	{
		class WindowDevice
		{
		public:
			static void Initialize();

			static void Uninitialize();

			static void Update();

			static const wchar_t* className_;
		};

		class Direct3DWindow
		{
		public:
			Direct3DWindow(HWND parentHandler, unsigned x, unsigned y, unsigned width, unsigned height);

			virtual ~Direct3DWindow() = default;

			void Update();

			void Show();

			void Hide();

			ID3D11Device* GetDevice() { return device_; }

			ID3D11DeviceContext* GetDeviceContext() { return deviceContext_; }

			void AddDefaultGraphics();

		protected:
			void StartupWindow(unsigned x, unsigned y, unsigned width, unsigned height);

			void CreateDevice(unsigned width, unsigned height, bool sRGB);

			void UpdateSwapChain(unsigned width, unsigned height);

			void SetViewport(const RECT& rect);

			void CreateShader();

			void CreateSamper();

			void Draw();

		private:
			// window:
			HWND parentHandler_{ nullptr };
			HWND handler_{ nullptr };

			//dx:
			ID3D11Device* device_{ nullptr };

			ID3D11DeviceContext* deviceContext_{ nullptr };

			IDXGISwapChain* swapChain_{ nullptr };

			ID3D11RenderTargetView* renderTargetView_{ nullptr };

			ID3D11Texture2D* depthTexture_{ nullptr };

			ID3D11DepthStencilView* depthStencilView_{ nullptr };

			ID3D11Buffer* d3d11Buffer_{ nullptr };

			ID3D11SamplerState* sampler_{ false };

			ID3D11VertexShader* vertexShader_{ nullptr };

			ID3D11PixelShader* pixelShader_{ nullptr };

			ID3D11InputLayout* inputLayout_{ nullptr };

			bool isRenderTargetDirty_{ false };

			bool isD3d11BufferDirty_{ false };

			bool isTextureDirty_{ false };

			bool isShaderDirty_{ false };
		};
	}
}

#endif
