#include "WebUISystem.h"
#include "UltralightImpl/FileSystem.h"
#include "UltralightImpl/GPUDriver.h"
#include "UltralightImpl/Clipboard.h"
#include "UltralightImpl/FontLoader.h"
#include "UltralightImpl/Monitor.h"
#include "Ultralight/platform/Platform.h"

#include <Ultralight/platform/Logger.h>
#include <Ultralight/RefPtr.h>
#include <Ultralight/Renderer.h>

#include <Core/EventManager.h>
#include <Log.h>
#include <Memory/MemoryHook.h>

namespace FlagGG
{

class LoggerImpl : public ultralight::Logger
{
public:
	///
	/// Called when the library wants to display a log message.
	///
	virtual void LogMessage(ultralight::LogLevel log_level, const ultralight::String& message) override
	{
		switch (log_level)
		{
		case ultralight::LogLevel::Error:
			FLAGGG_LOG_ERROR(message.utf8().data());
			break;
		
		case ultralight::LogLevel::Warning:
			FLAGGG_LOG_WARN(message.utf8().data());
			break;
				
		case ultralight::LogLevel::Info:
			FLAGGG_LOG_INFO(message.utf8().data());
			break;
		}
	}
};

WebUISystem::WebUISystem()
{
	ultralight::Platform::instance().set_logger(new LoggerImpl());
	ultralight::Platform::instance().set_gpu_driver(new ultralight::UltralightGPUDriver());
	ultralight::Platform::instance().set_font_loader(new ultralight::UltralightFontLoader());
	ultralight::Platform::instance().set_file_system(new ultralight::UltralightFileSystem());
	ultralight::Platform::instance().set_clipboard(new ultralight::UltralightClipboard());
	ultralight::Platform::instance().set_surface_factory(nullptr);

	auto renderer = ultralight::Renderer::Create();
	renderer_ = renderer.get();
	renderer_->AddRef();

	GetSubsystem<EventManager>()->RegisterEvent(EVENT_HANDLER(Frame::LOGIC_UPDATE, WebUISystem::Update, this));
}

WebUISystem::~WebUISystem()
{
	ultralight::Platform::instance().set_logger(nullptr);
	ultralight::Platform::instance().set_gpu_driver(nullptr);
	ultralight::Platform::instance().set_font_loader(nullptr);
	ultralight::Platform::instance().set_file_system(nullptr);
	ultralight::Platform::instance().set_clipboard(nullptr);
	ultralight::Platform::instance().set_surface_factory(nullptr);

	if (renderer_)
	{
		renderer_->Release();
		renderer_ = nullptr;
	}
}

void WebUISystem::Initialize()
{
	
}

void WebUISystem::Update(Real timeStep)
{
	renderer_->Update();
}

}

IMPLEMENT_MODULE_USD("WebUISystem");
