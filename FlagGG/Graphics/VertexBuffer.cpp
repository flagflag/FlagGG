#include "Graphics/VertexBuffer.h"
#include "Log.h"
#include "bgfx/bgfx.h"

namespace FlagGG
{
	namespace Graphics
	{
		static bgfx::Attrib::Enum BGFX_VERTEX_ELEMENT_SEMANTIC[]=
		{
			bgfx::Attrib::Position,
			bgfx::Attrib::Normal,
			bgfx::Attrib::Normal,
			bgfx::Attrib::Tangent,
			bgfx::Attrib::TexCoord0,
			bgfx::Attrib::Color0,
			bgfx::Attrib::Weight,
			bgfx::Attrib::Indices,
			bgfx::Attrib::TexCoord1,
		};

		static UInt16 BGFX_VERTEX_ELEMENT_COUNT[] =
		{
			4,
			1,
			2,
			3,
			4,
			4,
			4
		};

		static bgfx::AttribType::Enum BGFX_VERTEX_ELEMENT_TYPE[] =
		{
			bgfx::AttribType::Uint8,
			bgfx::AttribType::Float,
			bgfx::AttribType::Float,
			bgfx::AttribType::Float,
			bgfx::AttribType::Float,
			bgfx::AttribType::Uint8,
			bgfx::AttribType::Uint8,
		};

		bool VertexBuffer::SetSize(UInt32 vertexCount, const Container::PODVector<VertexElement>& vertexElements)
		{
			vertexSize_ = GetVertexSize(vertexElements);
			vertexCount_ = vertexCount;
			vertexElements_ = vertexElements;

			if (!GPUBuffer::SetSize(vertexSize_ * vertexCount_))
			{
				return false;
			}

			UpdateOffset();
		}

		void VertexBuffer::UpdateOffset()
		{
			UInt32 offset = 0;
			for (auto& element : vertexElements_)
			{
				element.offset_ = offset;
				offset += VERTEX_ELEMENT_TYPE_SIZE[element.vertexElementType_];
			}
		}

		UInt32 VertexBuffer::GetVertexSize() const
		{
			return vertexSize_;
		}

		UInt32 VertexBuffer::GetVertexCount() const
		{
			return vertexCount_;
		}

		const Container::PODVector<VertexElement>& VertexBuffer::GetElements() const
		{
			return vertexElements_;
		}

		Container::PODVector<VertexElement> VertexBuffer::GetElements(UInt32 elementMask)
		{
			Container::PODVector<VertexElement> vertexElements;
			for (UInt32 i = 0; i < MAX_DEFAULT_VERTEX_ELEMENT; ++i)
			{
				if (elementMask & (1u << i))
				{
					vertexElements.Push(DEFAULT_VERTEX_ELEMENT[i]);
				}
			}
			return vertexElements;
		}

		UInt32 VertexBuffer::GetVertexSize(const Container::PODVector<VertexElement>& elements)
		{
			UInt32 vertexSize = 0u;
			for (const auto& element : elements)
			{
				vertexSize += VERTEX_ELEMENT_TYPE_SIZE[element.vertexElementType_];
			}
			return vertexSize;
		}

		void VertexBuffer::Create(const bgfx::Memory* mem, bool dynamic)
		{
			bgfx::VertexLayout layout;
			layout.begin();
			for (auto it : vertexElements_)
			{
				layout.add(
					BGFX_VERTEX_ELEMENT_SEMANTIC[it.vertexElementSemantic_],
					BGFX_VERTEX_ELEMENT_COUNT[it.vertexElementType_],
					BGFX_VERTEX_ELEMENT_TYPE[it.vertexElementType_]);
			}
			layout.end();

			if (dynamic)
			{
				bgfx::DynamicVertexBufferHandle handle = bgfx::createDynamicVertexBuffer(mem, layout);
				ResetHandler(handle);
			}
			else
			{
				bgfx::VertexBufferHandle handle = bgfx::createVertexBuffer(mem, layout);
				ResetHandler(handle);
			}
		}

		void VertexBuffer::UpdateBuffer(const bgfx::Memory* mem)
		{
			bgfx::update(GetSrcHandler<bgfx::DynamicVertexBufferHandle>(), 0u, mem);
		}
	}
}
