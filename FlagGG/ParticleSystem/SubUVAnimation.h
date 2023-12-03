#pragma once

#include "Core/BaseTypes.h"
#include "Graphics/Texture2D.h"
#include "Container/Vector.h"
#include "Container/EnumAsByte.h"

namespace FlagGG
{

/**
	* More bounding vertices results in reduced overdraw, but adds more triangle overhead.
	* The eight vertex mode is best used when the SubUV texture has a lot of space to cut out that is not captured by the four vertex version,
	* and when the particles using the texture will be few and large.
	*/
enum SubUVBoundingVertexCount
{
	BVC_FourVertices,
	BVC_EightVertices
};

enum OpacitySourceMode
{
	OSM_Alpha,
	OSM_ColorBrightness,
	OSM_RedChannel,
	OSM_GreenChannel,
	OSM_BlueChannel
};

class FlagGG_API SubUVDerivedData
{
public:
	PODVector<Vector2> boundingGeometry_;

	//static FString GetDDCKeyString(const FGuid& StateId, Int32 SizeX, Int32 SizeY, Int32 Mode, float AlphaThreshold, Int32 OpacitySourceMode);
	//static void GetFeedback(UTexture2D* SubUVTexture, Int32 SubImages_Horizontal, Int32 SubImages_Vertical, ESubUVBoundingVertexCount BoundingMode, float AlphaThreshold, EOpacitySourceMode OpacitySourceMode,
	//	TArray<FText>& OutErrors, TArray<FText>& OutWarnings, TArray<FText>& OutInfo);
	//void Serialize(FStructuredArchive::FSlot Slot);
	//void Build(UTexture2D* SubUVTexture, Int32 SubImages_Horizontal, Int32 SubImages_Vertical, ESubUVBoundingVertexCount BoundingMode, float AlphaThreshold, EOpacitySourceMode OpacitySourceMode);
};

//class SubUVBoundingGeometryBuffer : public FVertexBuffer
//{
//public:
//	TArray<FVector2f>* Vertices;
//	FShaderResourceViewRHIRef ShaderResourceView;
//
//	SubUVBoundingGeometryBuffer(TArray<FVector2f>* InVertices)
//	{
//		Vertices = InVertices;
//	}
//
//	virtual void InitRHI() override;
//
//	virtual void ReleaseRHI() override
//	{
//		FVertexBuffer::ReleaseRHI();
//		ShaderResourceView.SafeRelease();
//	}
//};

/**
 * SubUV animation asset, which caches bounding geometry for regions in the SubUVTexture with non-zero opacity.
 * Particle emitters with a SubUV module which use this asset leverage the optimal bounding geometry to reduce overdraw.
 */
class SubUVAnimation : public Object
{
	OBJECT_OVERRIDE(SubUVAnimation, Object);
public:

	/**
	 * Texture to generate bounding geometry from.
	 */
	SharedPtr<Texture2D> subUVTexture_;

	/** The number of sub-images horizontally in the texture							*/
	Int32 subImages_Horizontal_;

	/** The number of sub-images vertically in the texture								*/
	Int32 subImages_Vertical_;

	/**
	 * More bounding vertices results in reduced overdraw, but adds more triangle overhead.
	 * The eight vertex mode is best used when the SubUV texture has a lot of space to cut out that is not captured by the four vertex version,
	 * and when the particles using the texture will be few and large.
	 */
	EnumAsByte<SubUVBoundingVertexCount> boundingMode_;

	EnumAsByte<OpacitySourceMode> opacitySourceMode_;

	/**
	 * Alpha channel values larger than the threshold are considered occupied and will be contained in the bounding geometry.
	 * Raising this threshold slightly can reduce overdraw in particles using this animation asset.
	 */
	float alphaThreshold_;

private:

	/** Derived data for this asset, generated off of SubUVTexture. */
	SubUVDerivedData derivedData_;

	/** Tracks progress of BoundingGeometryBuffer release during destruction. */
	// RenderCommandFence releaseFence_;

	/** Used on platforms that support instancing, the bounding geometry is fetched from a vertex shader instead of on the CPU. */
	// SubUVBoundingGeometryBuffer* boundingGeometryBuffer_;

public:

	inline Int32 GetNumBoundingVertices() const
	{
		if (boundingMode_ == BVC_FourVertices)
		{
			return 4;
		}

		return 8;
	}

	inline Int32 GetNumBoundingTriangles() const
	{
		if (boundingMode_ == BVC_FourVertices)
		{
			return 2;
		}

		return 6;
	}

	inline Int32 GetNumFrames() const
	{
		return subImages_Vertical_ * subImages_Horizontal_;
	}

	inline bool IsBoundingGeometryValid() const
	{
		return derivedData_.boundingGeometry_.Size() != 0;
	}

	inline const Vector2* GetFrameData(Int32 FrameIndex) const
	{
		return &derivedData_.boundingGeometry_[FrameIndex * GetNumBoundingVertices()];
	}

	//inline RHIShaderResourceView* GetBoundingGeometrySRV() const
	//{
	//	return BoundingGeometryBuffer->ShaderResourceView;
	//}

private:
	void CacheDerivedData()
	{
		// TODO
	}
};

}
