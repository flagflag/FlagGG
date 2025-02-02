#pragma once

#include <Core/Subsystem.h>
#include <Core/BaseTypes.h>
#include <Core/EventDefine.h>

#include "WebUISystem/WebUISystemExport.h"

namespace ultralight
{

class Renderer;

}

namespace FlagGG
{

class WebUISystem_API WebUISystem : public Subsystem<WebUISystem>
{
public:
	WebUISystem();

	~WebUISystem() override;

	void Initialize();

	void Update(Real timeStep);

	ultralight::Renderer* GetRenderer() const { return renderer_; }

private:
	ultralight::Renderer* renderer_;
};

}
