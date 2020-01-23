#ifndef __BATCH__
#define __BATCH__

#include "Export.h"
#include "Container/RefCounted.h"
#include "Container/Ptr.h"
#include "Container/Vector.h"

#include <vector>

namespace FlagGG
{
	namespace Graphics
	{
		class Texture;

		typedef Container::Vector <float> VertexVector;

		enum BatchType
		{
			DRAW_LINE = 0,
			DRAW_TRIANGLE = 1,
		};

		class FlagGG_API Batch : public Container::RefCounted
		{
		public:
			Batch(BatchType type, Texture* texture, VertexVector* vertexs, unsigned vertexSize);

			virtual ~Batch();

			const char* GetVertexs() const;

			unsigned GetVertexSize() const;

			unsigned GetVertexCount() const;

			Texture* GetTexture() const;

			void SetTexture(Texture* texture);

			BatchType GetType() const;

		protected:
			VertexVector* vertexs_;
			bool owner_;

			unsigned vertexSize_;
			
			unsigned vertexStart_;

			unsigned vertexEnd_;

			Container::SharedPtr<Texture> texture_;

			BatchType type_;
		};
	}
}

#endif