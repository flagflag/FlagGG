#include "Ultralight/private/Path.h"
#include "Ultralight/private/util/RefCountedImpl.h"

namespace ultralight
{

class PathImpl : public Path, public RefCountedImpl<PathImpl>
{
public:
	REF_COUNTED_IMPL(PathImpl);

	// Management
	virtual void Set(RefPtr<Path> path) override
	{

	}

	virtual void Clear() override
	{

	}

	virtual bool empty() const override
	{
		return false;
	}

	virtual bool has_current_point() const override
	{
		return false;
	}

	virtual Point current_point() const override
	{
		return Point();
	}

	virtual Rect aabb() const override
	{
		return Rect();
	}

	// Transformation
	virtual void Transform(const Matrix& matrix) override
	{

	}

	// This matrix is applied with each command
	virtual Matrix& matrix() override
	{
		return mat_;
	}

	// Path Commands
	virtual void MoveTo(const Point& p) override
	{

	}

	virtual void LineTo(const Point& p) override
	{

	}

	virtual void ConicTo(const Point& p1, const Point& p2) override
	{

	}

	virtual void CubicTo(const Point& p1, const Point& p2, const Point& p3) override
	{

	}

	virtual void ArcTo(const Point& p, float radius, float angle1, float angle2, bool is_negative) override
	{

	}

	virtual void Close() override
	{

	}

	// Geometry Helpers
	virtual void AddRoundedRect(const RoundedRect& rrect) override
	{

	}

	// Hash
	virtual uint32_t Hash() const override
	{
		return 0;
	}

	// Sampling
	virtual bool IsPointFilled(const Point& p, FillRule fill_rule) const override
	{
		return false;
	}

	virtual float GetDistanceToPoint(const Point& p) const override
	{
		return 0.0f;
	}

	// Enumerate Path Commands
	virtual void GetCommands(PathCommandVisitor visitor, void* context) const override
	{

	}

private:
	Matrix mat_;
};

Path::Path()
{

}

Path::~Path()
{

}

Path::Path(const Path&)
{

}

void Path::operator=(const Path&)
{

}

RefPtr<Path> Path::Create()
{
	return RefPtr<Path>(new PathImpl());
}

}
