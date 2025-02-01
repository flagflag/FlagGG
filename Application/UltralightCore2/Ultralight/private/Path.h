/******************************************************************************
 *  This file is a part of Ultralight, an ultra-portable web-browser engine.  *
 *                                                                            *
 *  See <https://ultralig.ht> for licensing and more.                         *
 *                                                                            *
 *  (C) 2023 Ultralight, Inc.                                                 *
 *****************************************************************************/
#pragma once
#include <Ultralight/Defines.h>
#include <Ultralight/Geometry.h>
#include <Ultralight/Matrix.h>
#include <Ultralight/private/Paint.h>
#include <Ultralight/RefPtr.h>

namespace ultralight {

enum PathCommand {
  kPathCommand_MoveTo,       // params: (x1, y1)
  kPathCommand_QuadraticTo,  // params: (x1, y1, x2, y2)
  kPathCommand_Close,        // params: (none)
};

typedef void(*PathCommandVisitor)(void* context, PathCommand command, float x1, float y1, float x2, float y2);

class UExport Path : public RefCounted {
public:
  // Factory Creation
  static RefPtr<Path> Create();

  // Management
  virtual void Set(RefPtr<Path> path) = 0;
  virtual void Clear() = 0;
  virtual bool empty() const = 0;
  virtual bool has_current_point() const = 0;
  virtual Point current_point() const = 0;
  virtual Rect aabb() const = 0;

  // Transformation
  virtual void Transform(const Matrix& matrix) = 0;

  // This matrix is applied with each command
  virtual Matrix& matrix() = 0;

  // Path Commands
  virtual void MoveTo(const Point& p) = 0;
  virtual void LineTo(const Point& p) = 0;
  virtual void ConicTo(const Point& p1, const Point& p2) = 0;
  virtual void CubicTo(const Point& p1, const Point& p2, const Point& p3) = 0;
  virtual void ArcTo(const Point& p, float radius, float angle1, float angle2, bool is_negative) = 0;
  virtual void Close() = 0;

  // Geometry Helpers
  virtual void AddRoundedRect(const RoundedRect& rrect) = 0;

  // Hash
  virtual uint32_t Hash() const = 0;

  // Sampling
  virtual bool IsPointFilled(const Point& p, FillRule fill_rule) const = 0;
  virtual float GetDistanceToPoint(const Point& p) const = 0;

  // Enumerate Path Commands
  virtual void GetCommands(PathCommandVisitor visitor, void* context) const = 0;

protected:
  Path();
  virtual ~Path();
  Path(const Path&);
  void operator=(const Path&);
};

}  // namespace ultralight
