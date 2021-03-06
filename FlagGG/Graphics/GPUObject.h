#ifndef __GPU_OBJECT__
#define __GPU_OBJECT__

#include "Export.h"

#ifdef _WIN32
#include <d3d11.h>
#endif

#include "Define.h"

#ifdef USE_DIRECT3D
typedef void* GPUHandler;
#else
typedef std::string GPUHandler;
#endif

namespace FlagGG
{
	namespace Graphics
	{
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
			Type* GetObject()
			{
				return static_cast<Type*>(GetHandler());
			}

			void ResetHandler(GPUHandler handler);

		private:
			GPUHandler gpuHandler_;
		};
	}
}

#endif