#ifndef __BATCH__
#define __BATCH__

#include "Math/Vector2.h"

#include <vector>

namespace FlagGG
{
	namespace Graphics
	{
		class Texture;

		typedef std::vector<unsigned char> VertexVector;

		class Batch
		{
		public:
			Batch(Texture* texture, VertexVector* vertexs = nullptr);

			virtual ~Batch();

			void AddTriangle(const Math::Vector2& v1, const Math::Vector2& v2, const Math::Vector2& v3,
				const Math::Vector2& uv1, const Math::Vector2& uv2, const Math::Vector2& uv3, unsigned color);

			VertexVector* GetVertexs() const;

			unsigned GetVertexSize() const;

			unsigned GetVertexCount() const;

			Texture* GetTexture() const;

		private:
			VertexVector* vertexs_;
			bool owner_;

			unsigned vertexSize_{ 24 };
			
			unsigned vertexStart_;

			unsigned vertexEnd_;

			Texture* texture_;
		};
	}
}

#endif