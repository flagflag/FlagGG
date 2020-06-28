#ifndef __GPU_OBJECT__
#define __GPU_OBJECT__

#include "Export.h"
#include "Define.h"
#include "Core/BaseTypes.h"
#include "bgfx/bgfx.h"

enum GPUHandlerType
{
	GPU_Invalid = 0,
	GPU_DynamicIndexBufferHandle,
	GPU_DynamicVertexBufferHandle,
	GPU_FrameBufferHandle,
	GPU_IndexBufferHandle,
	GPU_IndirectBufferHandle,
	GPU_ProgramHandle,
	GPU_ShaderHandle,
	GPU_TextureHandle,
	GPU_UniformHandle,
	GPU_VertexBufferHandle,
	GPU_VertexLayoutHandle,
};

namespace FlagGG
{
	namespace Graphics
	{
		struct GPUHandler
		{
			GPUHandler();

			GPUHandler(bgfx::DynamicIndexBufferHandle handle);

			GPUHandler(bgfx::DynamicVertexBufferHandle handle);

			GPUHandler(bgfx::FrameBufferHandle handle);

			GPUHandler(bgfx::IndexBufferHandle handle);

			GPUHandler(bgfx::IndirectBufferHandle handle);

			GPUHandler(bgfx::ProgramHandle handle);

			GPUHandler(bgfx::ShaderHandle handle);

			GPUHandler(bgfx::TextureHandle handle);

			GPUHandler(bgfx::UniformHandle handle);

			GPUHandler(bgfx::VertexBufferHandle handle);

			GPUHandler(bgfx::VertexLayoutHandle handle);

			GPUHandler(const GPUHandler& rhs);

			~GPUHandler();

			GPUHandler& operator=(const GPUHandler& rhs);

			operator bool() { return type_ != GPU_Invalid; }

			void AddRef();

			void ReleaseRef();

			void Destroy();

			UInt16 idx_;
			GPUHandlerType type_;
			UInt16* refCount_;

			static GPUHandler INVALID;
		};

		class FlagGG_API GPUObject
		{
		public:
			GPUObject();

			GPUObject(GPUHandler handler);

			virtual ~GPUObject();

			GPUHandler GetHandler();

			virtual bool IsValid() = 0;

			virtual void Initialize() = 0;

			template < typename Type >
			Type GetSrcHandler()
			{
				return Type{ gpuHandler_.idx_ };
			}

			void ResetHandler(GPUHandler handler);

		private:
			GPUHandler gpuHandler_;
		};
	}
}

#endif