#include "OctreeQuery.h"
#include "Scene/DrawableComponent.h"

namespace FlagGG
{

Intersection PointOctreeQuery::TestOctant(const BoundingBox& box, bool inside)
{
	if (inside)
		return INSIDE;
	else
		return box.IsInside(point_);
}

void PointOctreeQuery::TestDrawables(DrawableComponent** start, DrawableComponent** end, bool inside)
{
	while (start != end)
	{
		DrawableComponent* drawable = *start++;

		if ((drawable->GetDrawableFlags() & drawableFlags_) && (drawable->GetViewMask() & viewMask_))
		{
			if (inside || drawable->GetWorldBoundingBox().IsInside(point_))
				result_.Push(drawable);
		}
	}
}

Intersection SphereOctreeQuery::TestOctant(const BoundingBox& box, bool inside)
{
	if (inside)
		return INSIDE;
	else
		return sphere_.IsInside(box);
}

void SphereOctreeQuery::TestDrawables(DrawableComponent** start, DrawableComponent** end, bool inside)
{
	while (start != end)
	{
		DrawableComponent* drawable = *start++;

		if ((drawable->GetDrawableFlags() & drawableFlags_) && (drawable->GetViewMask() & viewMask_))
		{
			if (inside || sphere_.IsInsideFast(drawable->GetWorldBoundingBox()))
				result_.Push(drawable);
		}
	}
}

Intersection BoxOctreeQuery::TestOctant(const BoundingBox& box, bool inside)
{
	if (inside)
		return INSIDE;
	else
		return box_.IsInside(box);
}

void BoxOctreeQuery::TestDrawables(DrawableComponent** start, DrawableComponent** end, bool inside)
{
	while (start != end)
	{
		DrawableComponent* drawable = *start++;

		if ((drawable->GetDrawableFlags() & drawableFlags_) && (drawable->GetViewMask() & viewMask_))
		{
			if (inside || box_.IsInsideFast(drawable->GetWorldBoundingBox()))
				result_.Push(drawable);
		}
	}
}

Intersection FrustumOctreeQuery::TestOctant(const BoundingBox& box, bool inside)
{
	if (inside)
		return INSIDE;
	else
		return frustum_.IsInside(box);
}

void FrustumOctreeQuery::TestDrawables(DrawableComponent** start, DrawableComponent** end, bool inside)
{
	while (start != end)
	{
		DrawableComponent* drawable = *start++;

		if ((drawable->GetDrawableFlags() & drawableFlags_) && (drawable->GetViewMask() & viewMask_))
		{
			if (inside || frustum_.IsInsideFast(drawable->GetWorldBoundingBox()))
				result_.Push(drawable);
		}
	}
}

}
