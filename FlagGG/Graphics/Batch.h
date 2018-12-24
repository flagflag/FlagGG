#ifndef __BATCH__
#define __BATCH__

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
			Batch(Texture* texture, VertexVector* vertexs, unsigned vertexSize);

			virtual ~Batch();

			VertexVector* GetVertexs() const;

			unsigned GetVertexSize() const;

			unsigned GetVertexCount() const;

			Texture* GetTexture() const;

		protected:
			VertexVector* vertexs_;
			bool owner_;

			unsigned vertexSize_;
			
			unsigned vertexStart_;

			unsigned vertexEnd_;

			Texture* texture_;
		};
	}
}

#endif