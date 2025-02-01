#pragma once

#include <Graphics/Batch.h>
#include <Core/Subsystem.h>

#include "UltralightCore2/Ultralight/Defines.h"

namespace FlagGG
{

class Texture2D;
class Window;

}

namespace ultralight
{

class UExport RenderContext : public FlagGG::Subsystem<RenderContext>
{
public:
	void SetCallStackRenderBatch(FlagGG::VertexVector* vertexVector, FlagGG::Vector<FlagGG::SharedPtr<FlagGG::Batch>>* uiBatches)
	{
		vertexVector_ = vertexVector;
		uiBatches_ = uiBatches;
	}

	FlagGG::VertexVector* GetCallStackVertexVector() const { return vertexVector_; }

	FlagGG::Vector<FlagGG::SharedPtr<FlagGG::Batch>>* GetCallStackBatches() const { return uiBatches_; }

private:
	FlagGG::VertexVector* vertexVector_;
	FlagGG::Vector<FlagGG::SharedPtr<FlagGG::Batch>>* uiBatches_;
};

}
