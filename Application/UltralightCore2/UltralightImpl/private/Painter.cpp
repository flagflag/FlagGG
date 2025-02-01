#include "Ultralight/private/Painter.h"
#include "UltralightImpl/ThreadCall.h"

#include <Container/Vector.h>
#include <FileSystem/FileManager.h>
#include <FileSystem/FileSystemArchive/DefaultFileSystemArchive.h>
#include <Utility/SystemHelper.h>
#include <Graphics/Window.h>
#include <Graphics/RenderEngine.h>
#include <AsyncFrame/Thread/UniqueThread.h>
#include <AsyncFrame/Mutex.h>
#include <Core/Forwarder.h>
#include <Core/EventManager.h>
#include <Core/EventDefine.h>
#include <GameEngine.h>

using namespace FlagGG;

namespace ultralight
{

class PainterImpl : public Painter
{
public:
	// Attach Paintable, Paint() will call Paintable::Paint so that
	// top-level canvases can perform all painting in lock-step with the timer.
	virtual void AttachPaintable(Paintable* paintable) override
	{
		if (!paintables_.Contains(paintable))
			paintables_.Push(paintable);
	}

	// Detach Paintable
	virtual void DetachPaintable(Paintable* paintable) override
	{
		paintables_.Remove(paintable);
	}

	virtual void Paint() override
	{
		for (auto* paintable : paintables_)
		{
			if (paintable->needs_paint())
			{
				paintable->Paint(0, 16);
			}
		}
	}

	virtual void PaintOnly(Paintable** paintable_array, size_t len) override
	{
		for (size_t i = 0; i < len; ++i)
		{
			auto* paintable = paintable_array[i];
			if (paintable->needs_paint())
			{
				paintable->Paint(0, 16);
			}
		}
	}

	virtual void Recycle() override
	{

	}

	// Purge memory from cache and pools (set critical to true to release all).
	virtual void PurgeMemory(bool critical) override
	{

	}

	virtual void LogStatistics() override
	{

	}

	virtual BitmapImageHandler* bitmap_image_handler() override
	{
		return nullptr;
	}

private:
	FlagGG::PODVector<Paintable*> paintables_;
};

UExport void InitPainter()
{
#if !APP_CORE_FOR_ENGINE
	if (!FlagGG::GetSubsystem<ThreadRenderCall>()->IsMultiThread())
	{
		GetSubsystem<AssetFileManager>()->AddArchive(new DefaultFileSystemArchive(GetLocalFileSystem(), GetProgramDir() + "Res"));

		WindowDevice::Initialize();
		RenderEngine::Instance().Initialize();
	}
#endif
}

UExport void DestroyPainter()
{

}

Painter& Painter::instance()
{
	static PainterImpl inst;
	return inst;
}

}
