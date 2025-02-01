#include "Ultralight/platform/Platform.h"
#include "Ultralight/platform/Config.h"

namespace ultralight
{

class PlatformImpl : public Platform
{
public:
	PlatformImpl()
	{

	}

	~PlatformImpl() override
	{

	}

	///
	/// Set the Config
	///
	virtual void set_config(const Config& config) override
	{
		config_ = config;
	}

	///
	/// Get the Config
	///
	virtual const Config& config() const override
	{
		return config_;
	}

	///
	/// Set the Logger (to handle error messages and debug output).
	///
	/// @param  logger  A user-defined Logger implementation, ownership remains with the caller.
	///
	virtual void set_logger(Logger* logger) override
	{
		logger_ = logger;
	}

	///
	/// Get the Logger
	///
	virtual Logger* logger() const override
	{
		return logger_;
	}

	///
	/// Set the GPU Driver (will handle all rendering)
	///
	/// @param  gpu_driver  A user-defined GPUDriver implementation, ownership remains with the
	///                     caller.
	///
	virtual void set_gpu_driver(GPUDriver* gpu_driver) override
	{
		gpuDriver_ = gpu_driver;
	}

	///
	/// Get the GPU Driver
	///
	virtual GPUDriver* gpu_driver() const override
	{
		return gpuDriver_;
	}

	///
	/// Set the Font Loader (will be used to map font families to actual fonts)
	///
	/// @param  font_loader  A user-defined FontLoader implementation, ownership remains with the
	///                      caller.
	///
	virtual void set_font_loader(FontLoader* font_loader) override
	{
		fontLoader_ = font_loader;
	}

	///
	/// Get the Font Loader
	///
	virtual FontLoader* font_loader() const override
	{
		return fontLoader_;
	}

	///
	/// Set the File System (will be used for all file system operations)
	///
	/// @param  file_system  A user-defined FileSystem implementation, ownership remains with the
	///                      caller.
	///
	virtual void set_file_system(FileSystem* file_system) override
	{
		fileSystem_ = file_system;
	}

	///
	/// Get the File System
	///
	virtual FileSystem* file_system() const override
	{
		return fileSystem_;
	}

	///
	/// Set the Clipboard (will be used for all clipboard operations)
	///
	/// @param  clipboard  A user-defined Clipboard implementation, ownership remains with the
	///                    caller.
	///
	virtual void set_clipboard(Clipboard* clipboard) override
	{
		clipboard_ = clipboard;
	}

	///
	/// Get the Clipboard
	///
	virtual Clipboard* clipboard() const override
	{
		return clipboard_;
	}

	///
	/// Set the SurfaceFactory
	///
	/// This can be used to provide a platform-specific bitmap surface for View to paint into when
	/// the CPU renderer is enabled. See View::surface().
	///
	/// @param  surface_factory  A user-defined SurfaceFactory implementation, ownership remains with
	///                          the caller.
	///
	/// @note  A default BitmapSurfaceFactory is defined if you never call this, View::surface() can
	///        be safely cast to BitmapSurface.
	///
	virtual void set_surface_factory(SurfaceFactory* surface_factory) override
	{
		surfaceFactory_ = surface_factory;
	}

	///
	/// Get the SurfaceFactory
	///
	/// @note  A default BitmapSurfaceFactory is set by default, View::surface() can be safely cast
	///        to BitmapSurface if you don't define your own.
	///
	virtual SurfaceFactory* surface_factory() const override
	{
		return surfaceFactory_;
	}

private:
	Config config_;

	Logger* logger_;

	GPUDriver* gpuDriver_;

	FontLoader* fontLoader_;

	FileSystem* fileSystem_;

	Clipboard* clipboard_;

	SurfaceFactory* surfaceFactory_;
};

Platform::~Platform() = default;

Platform& Platform::instance()
{
	static PlatformImpl inst;
	return inst;
}

}
