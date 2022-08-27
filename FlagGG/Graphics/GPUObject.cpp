#include "GPUObject.h"

#include "RenderEngine.h"

namespace FlagGG
{
	namespace Graphics
	{
		GPUHandler GPUHandler::INVALID;

		GPUHandler::GPUHandler() :
			idx_(bgfx::kInvalidHandle),
			type_(GPU_Invalid),
			refCount_(nullptr)
		{}

		GPUHandler::GPUHandler(bgfx::DynamicIndexBufferHandle handle) :
			idx_(handle.idx),
			type_(GPU_DynamicIndexBufferHandle),
			refCount_(new UInt16(1u))
		{}

		GPUHandler::GPUHandler(bgfx::DynamicVertexBufferHandle handle) :
			idx_(handle.idx),
			type_(GPU_DynamicVertexBufferHandle),
			refCount_(new UInt16(1u))
		{}

		GPUHandler::GPUHandler(bgfx::FrameBufferHandle handle) :
			idx_(handle.idx),
			type_(GPU_FrameBufferHandle),
			refCount_(new UInt16(1u))
		{}

		GPUHandler::GPUHandler(bgfx::IndexBufferHandle handle) :
			idx_(handle.idx),
			type_(GPU_IndexBufferHandle),
			refCount_(new UInt16(1u))
		{}

		GPUHandler::GPUHandler(bgfx::IndirectBufferHandle handle) :
			idx_(handle.idx),
			type_(GPU_IndirectBufferHandle),
			refCount_(new UInt16(1u))
		{}

		GPUHandler::GPUHandler(bgfx::ProgramHandle handle) :
			idx_(handle.idx),
			type_(GPU_ProgramHandle),
			refCount_(new UInt16(1u))
		{}

		GPUHandler::GPUHandler(bgfx::ShaderHandle handle) :
			idx_(handle.idx),
			type_(GPU_ShaderHandle),
			refCount_(new UInt16(1u))
		{}

		GPUHandler::GPUHandler(bgfx::TextureHandle handle) :
			idx_(handle.idx),
			type_(GPU_TextureHandle),
			refCount_(new UInt16(1u))
		{}

		GPUHandler::GPUHandler(bgfx::UniformHandle handle) :
			idx_(handle.idx),
			type_(GPU_UniformHandle),
			refCount_(new UInt16(1u))
		{}

		GPUHandler::GPUHandler(bgfx::VertexBufferHandle handle) :
			idx_(handle.idx),
			type_(GPU_VertexBufferHandle),
			refCount_(new UInt16(1u))
		{}

		GPUHandler::GPUHandler(bgfx::VertexLayoutHandle handle) :
			idx_(handle.idx),
			type_(GPU_VertexLayoutHandle),
			refCount_(new UInt16(1u))
		{}

		GPUHandler::GPUHandler(const GPUHandler& rhs) :
			idx_(rhs.idx_),
			type_(rhs.type_),
			refCount_(rhs.refCount_)
		{
			AddRef();
		}

		GPUHandler::~GPUHandler()
		{
			ReleaseRef();
		}

		GPUHandler& GPUHandler::operator=(const GPUHandler& rhs)
		{
			ReleaseRef();

			idx_ = rhs.idx_;
			type_ = rhs.type_;
			refCount_ = rhs.refCount_;

			AddRef();

			return *this;
		}

		void GPUHandler::AddRef()
		{
			if (refCount_)
				++(*refCount_);
		}

		void GPUHandler::ReleaseRef()
		{
			if (refCount_)
			{
				--(*refCount_);
				if (*refCount_ == 0u)
				{
					Destroy();
				}
			}
		}

		void GPUHandler::Destroy()
		{
			switch (type_)
			{
			case GPU_DynamicIndexBufferHandle:
				bgfx::destroy(bgfx::DynamicIndexBufferHandle{ idx_ });
				break;

			case GPU_DynamicVertexBufferHandle:
				bgfx::destroy(bgfx::DynamicVertexBufferHandle{ idx_ });
				break;

			case GPU_FrameBufferHandle:
				bgfx::destroy(bgfx::FrameBufferHandle{ idx_ });
				break;

			case GPU_IndexBufferHandle:
				bgfx::destroy(bgfx::IndexBufferHandle{ idx_ });
				break;

			case GPU_IndirectBufferHandle:
				bgfx::destroy(bgfx::IndirectBufferHandle{ idx_ });
				break;

			case GPU_ProgramHandle:
				bgfx::destroy(bgfx::ProgramHandle{ idx_ });
				break;

			case GPU_ShaderHandle:
				bgfx::destroy(bgfx::ShaderHandle{ idx_ });
				break;

			case GPU_TextureHandle:
				bgfx::destroy(bgfx::TextureHandle{ idx_ });
				break;

			case GPU_UniformHandle:
				bgfx::destroy(bgfx::UniformHandle{ idx_ });
				break;

			case GPU_VertexBufferHandle:
				bgfx::destroy(bgfx::VertexBufferHandle{ idx_ });
				break;

			case GPU_VertexLayoutHandle:
				bgfx::destroy(bgfx::VertexLayoutHandle{ idx_ });
				break;

			default:
				FLAGGG_LOG_ERROR("invalid gpu handler type[%d];", (Int32)type_);
				break;
			}

			idx_ = 0u;
			type_ = GPU_Invalid;

			delete refCount_;
			refCount_ = nullptr;
		}

		GPUObject::GPUObject() :
			gpuHandler_(GPUHandler::INVALID)
		{
		}

		GPUObject::GPUObject(GPUHandler handler) :
			gpuHandler_(handler)
		{
		}

		GPUObject::~GPUObject()
		{
			gpuHandler_.Destroy();
		}

		GPUHandler GPUObject::GetHandler()
		{
			return gpuHandler_;
		}

		void GPUObject::ResetHandler(GPUHandler handler)
		{
			gpuHandler_ = handler;
		}
	}
}
