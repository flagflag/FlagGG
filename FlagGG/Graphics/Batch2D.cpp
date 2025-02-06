#include "Batch2D.h"
#include "Texture.h"
#include "ShaderParameter.h"

namespace FlagGG
{

template <typename T>
static UInt32 GetVertexElementsSize()
{
	struct VertexElementsSizeInfo
	{
		VertexElementsSizeInfo()
		{
			const auto& elements = T::GetVertexElements();
			value_ = 0;
			for (const auto& element : elements)
			{
				value_ += VERTEX_ELEMENT_TYPE_SIZE[element.vertexElementType_];
			}
		}

		UInt32 value_;
	};

	static VertexElementsSizeInfo info;
	return info.value_;
}

Batch2D::Batch2D(VertexVector* vertexs, Texture* texture) :
	Batch(DRAW_TRIANGLE, vertexs, Batch2D::GetVertexElementsSize(), texture)
{

}

Batch2D::~Batch2D() = default;

void Batch2D::AddTriangle(const Vector2& v1, const Vector2& v2, const Vector2& v3,
	const Vector2& uv1, const Vector2& uv2, const Vector2& uv3,
	UInt32 color1, UInt32 color2, UInt32 color3)
{
	UInt32 begin = vertexs_->Size();
	vertexEnd_ = vertexs_->Size() + vertexSize_ / 4u * 3;
	vertexs_->Resize(vertexEnd_);

	float* dest = &(vertexs_->At(begin));
	dest[0] = v1.x_;
	dest[1] = v1.y_;
	dest[2] = 0.0f;
	dest[3] = uv1.x_;
	dest[4] = uv1.y_;
	((UInt32&)dest[5]) = color1;

	dest[6] = v2.x_;
	dest[7] = v2.y_;
	dest[8] = 0.0f;
	dest[9] = uv2.x_;
	dest[10] = uv2.y_;
	((UInt32&)dest[11]) = color2;

	dest[12] = v3.x_;
	dest[13] = v3.y_;
	dest[14] = 0.0f;
	dest[15] = uv3.x_;
	dest[16] = uv3.y_;
	((UInt32&)dest[17]) = color3;
}

const PODVector<VertexElement>& Batch2D::GetVertexElements()
{
	static PODVector<VertexElement> vertexElement =
	{
		VertexElement(VE_VECTOR3, SEM_POSITION, 0),
		VertexElement(VE_VECTOR2, SEM_TEXCOORD, 0),
		VertexElement(VE_UBYTE4_UNORM, SEM_COLOR, 0)
	};
	return vertexElement;
}

UInt32 Batch2D::GetVertexElementsSize()
{
	return FlagGG::GetVertexElementsSize<Batch2D>();
}

BatchWebKit::BatchWebKit(VertexVector* vertexs, Texture* texture)
	: Batch(DRAW_TRIANGLE, vertexs, BatchWebKit::GetVertexElementsSize(), texture)
	, vector_{}
{

}

BatchWebKit::~BatchWebKit() = default;

void BatchWebKit::AddTriangle(const Vector2& v1, const Vector2& v2, const Vector2& v3,
	const Vector2& uv1, const Vector2& uv2, const Vector2& uv3,
	UInt32 color1, UInt32 color2, UInt32 color3,
	const Vector4& data0,
	const Vector4& data1,
	const Vector4& data2,
	const Vector4& data3,
	const Vector4& data4,
	const Vector4& data5,
	const Vector4& data6,
	const Vector2& objectCoord1, const Vector2& objectCoord2, const Vector2& objectCoord3)
{
	UInt32 begin = vertexs_->Size();
	vertexEnd_ = vertexs_->Size() + vertexSize_ / 4u * 3;
	vertexs_->Resize(vertexEnd_);

	UInt32 idx = 0;

	float* dest = &(vertexs_->At(begin));
	{
		dest[idx++] = v1.x_;
		dest[idx++] = v1.y_;
		dest[idx++] = uv1.x_;
		dest[idx++] = uv1.y_;
		dest[idx++] = objectCoord1.x_;
		dest[idx++] = objectCoord1.y_;
		((UInt32&)dest[idx++]) = color1;

		dest[idx++] = data0.x_;
		dest[idx++] = data0.y_;
		dest[idx++] = data0.z_;
		dest[idx++] = data0.w_;

		dest[idx++] = data1.x_;
		dest[idx++] = data1.y_;
		dest[idx++] = data1.z_;
		dest[idx++] = data1.w_;

		dest[idx++] = data2.x_;
		dest[idx++] = data2.y_;
		dest[idx++] = data2.z_;
		dest[idx++] = data2.w_;

		dest[idx++] = data3.x_;
		dest[idx++] = data3.y_;
		dest[idx++] = data3.z_;
		dest[idx++] = data3.w_;

		dest[idx++] = data4.x_;
		dest[idx++] = data4.y_;
		dest[idx++] = data4.z_;
		dest[idx++] = data4.w_;

		dest[idx++] = data5.x_;
		dest[idx++] = data5.y_;
		dest[idx++] = data5.z_;
		dest[idx++] = data5.w_;

		dest[idx++] = data6.x_;
		dest[idx++] = data6.y_;
		dest[idx++] = data6.z_;
		dest[idx++] = data6.w_;
	}

	{
		dest[idx++] = v2.x_;
		dest[idx++] = v2.y_;
		dest[idx++] = uv2.x_;
		dest[idx++] = uv2.y_;
		dest[idx++] = objectCoord2.x_;
		dest[idx++] = objectCoord2.y_;
		((UInt32&)dest[idx++]) = color2;

		dest[idx++] = data0.x_;
		dest[idx++] = data0.y_;
		dest[idx++] = data0.z_;
		dest[idx++] = data0.w_;

		dest[idx++] = data1.x_;
		dest[idx++] = data1.y_;
		dest[idx++] = data1.z_;
		dest[idx++] = data1.w_;

		dest[idx++] = data2.x_;
		dest[idx++] = data2.y_;
		dest[idx++] = data2.z_;
		dest[idx++] = data2.w_;

		dest[idx++] = data3.x_;
		dest[idx++] = data3.y_;
		dest[idx++] = data3.z_;
		dest[idx++] = data3.w_;

		dest[idx++] = data4.x_;
		dest[idx++] = data4.y_;
		dest[idx++] = data4.z_;
		dest[idx++] = data4.w_;

		dest[idx++] = data5.x_;
		dest[idx++] = data5.y_;
		dest[idx++] = data5.z_;
		dest[idx++] = data5.w_;

		dest[idx++] = data6.x_;
		dest[idx++] = data6.y_;
		dest[idx++] = data6.z_;
		dest[idx++] = data6.w_;
	}

	{
		dest[idx++] = v3.x_;
		dest[idx++] = v3.y_;
		dest[idx++] = uv3.x_;
		dest[idx++] = uv3.y_;
		dest[idx++] = objectCoord3.x_;
		dest[idx++] = objectCoord3.y_;
		((UInt32&)dest[idx++]) = color3;

		dest[idx++] = data0.x_;
		dest[idx++] = data0.y_;
		dest[idx++] = data0.z_;
		dest[idx++] = data0.w_;

		dest[idx++] = data1.x_;
		dest[idx++] = data1.y_;
		dest[idx++] = data1.z_;
		dest[idx++] = data1.w_;

		dest[idx++] = data2.x_;
		dest[idx++] = data2.y_;
		dest[idx++] = data2.z_;
		dest[idx++] = data2.w_;

		dest[idx++] = data3.x_;
		dest[idx++] = data3.y_;
		dest[idx++] = data3.z_;
		dest[idx++] = data3.w_;

		dest[idx++] = data4.x_;
		dest[idx++] = data4.y_;
		dest[idx++] = data4.z_;
		dest[idx++] = data4.w_;

		dest[idx++] = data5.x_;
		dest[idx++] = data5.y_;
		dest[idx++] = data5.z_;
		dest[idx++] = data5.w_;

		dest[idx++] = data6.x_;
		dest[idx++] = data6.y_;
		dest[idx++] = data6.z_;
		dest[idx++] = data6.w_;
	}
}

void BatchWebKit::ApplyShaderParameters(ShaderParameters* shaderParameters)
{
	shaderParameters->SetValueImpl("Vector", vector_, sizeof(Vector4) * 8u);
	shaderParameters->SetValue<UInt32>("ClipSize", clipArray_.Size());
	shaderParameters->SetValueImpl("Clip", clipArray_.Buffer(), clipArray_.Size() * sizeof(Matrix4));
}

const PODVector<VertexElement>& BatchWebKit::GetVertexElements()
{
	static PODVector<VertexElement> vertexElement =
	{
		VertexElement(VE_VECTOR2, SEM_POSITION, 0),
		VertexElement(VE_VECTOR2, SEM_TEXCOORD, 0),
		VertexElement(VE_VECTOR2, SEM_TEXCOORD, 1),
		VertexElement(VE_UBYTE4_UNORM, SEM_COLOR, 0),
		VertexElement(VE_VECTOR4, SEM_COLOR, 1),
		VertexElement(VE_VECTOR4, SEM_COLOR, 2),
		VertexElement(VE_VECTOR4, SEM_COLOR, 3),
		VertexElement(VE_VECTOR4, SEM_COLOR, 4),
		VertexElement(VE_VECTOR4, SEM_COLOR, 5),
		VertexElement(VE_VECTOR4, SEM_COLOR, 6),
		VertexElement(VE_VECTOR4, SEM_COLOR, 7),
	};
	return vertexElement;
}

UInt32 BatchWebKit::GetVertexElementsSize()
{
	return FlagGG::GetVertexElementsSize<BatchWebKit>();
}

}
