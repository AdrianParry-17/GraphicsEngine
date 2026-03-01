#pragma once

#include <math.h>
#include <algorithm>

namespace Engine {
    /// @brief Integer 2D point.
    class Point {
    public:
        /// @brief X coordinate.
        int x = 0, y = 0;

        Point() = default;
        Point(int _xy) : x(_xy), y(_xy) {}
        Point(int _x, int _y) : x(_x), y(_y) {}

        Point operator-() const { return Point(-x, -y); }

        Point& operator+=(const Point& _p) { x += _p.x; y += _p.y; return *this; }
        Point& operator-=(const Point& _p) { x -= _p.x; y -= _p.y; return *this; }
        Point& operator*=(const Point& _p) { x *= _p.x; y *= _p.y; return *this; }
        Point& operator+=(int _xy) { x += _xy; y += _xy; return *this; }
        Point& operator-=(int _xy) { x -= _xy; y -= _xy; return *this; }
        Point& operator*=(int _xy) { x *= _xy; y *= _xy; return *this; }

        Point operator+(const Point& _p) const { return Point(x + _p.x, y + _p.y); }
        Point operator+(int _xy) const { return Point(x + _xy, y + _xy); }
        friend Point operator+(int _xy, const Point& _p) { return Point(_xy + _p.x, _xy + _p.y); }
        Point operator-(const Point& _p) const { return Point(x - _p.x, y - _p.y); }
        Point operator-(int _xy) const { return Point(x - _xy, y - _xy); }
        friend Point operator-(int _xy, const Point& _p) { return Point(_xy - _p.x, _xy - _p.y); }
        Point operator*(const Point& _p) const { return Point(x * _p.x, y * _p.y); }
        Point operator*(int _xy) const { return Point(x * _xy, y * _xy); }
        friend Point operator*(int _xy, const Point& _p) { return Point(_xy * _p.x, _xy * _p.y); }

        bool operator==(const Point& _p) const { return x == _p.x && y == _p.y; }
        bool operator!=(const Point& _p) const { return !(*this == _p); }
        bool operator==(int _xy) const { return x == _xy && y == _xy; }
        bool operator!=(int _xy) const { return !(*this == _xy); }
        friend bool operator==(int _xy, const Point& _p) { return (_p == _xy); }
        friend bool operator!=(int _xy, const Point& _p) { return (_p != _xy); }

        /// @brief Flips point sign on selected axes.
        Point Flip(bool xAxis = false, bool yAxis = false) const { return Point(xAxis ? -x : x, yAxis ? -y : y); }
        /// @brief Returns point with absolute components.
        Point Absolute() const { return Point(std::abs(x), std::abs(y)); }

        /// @brief Euclidean distance between two points.
        static double Distance(const Point& a, const Point& b) { return std::hypot(b.x - a.x, b.y - a.y); }
        /// @brief Manhattan distance between two points.
        static int ManhattanDistance(const Point& a, const Point& b) { return std::abs(b.x - a.x) + std::abs(b.y - a.y); }

        /// @brief Euclidean distance from this point to another.
        double DistanceFrom(const Point& p) const { return Point::Distance(*this, p); }
        /// @brief Manhattan distance from this point to another.
        double ManhattanDistanceFrom(const Point& p) const { return Point::ManhattanDistance(*this, p); }
    };

    /// @brief Integer 2D size.
    class Size {
    public:
        /// @brief Width and height.
        int w = 0, h = 0;

        Size() = default;
        Size(int _wh) : w(_wh), h(_wh) {}
        Size(int _w, int _h) : w(_w), h(_h) {}
        Size(const Point& _p) : w(_p.x), h(_p.y) {}

        operator Point() const { return Point(w, h); }
        Size operator-() const { return Size(-w, -h); }

        Size& operator+=(const Size& _s) { w += _s.w; h += _s.h; return *this; }
        Size& operator-=(const Size& _s) { w -= _s.w; h -= _s.h; return *this; }
        Size& operator*=(const Size& _s) { w *= _s.w; h *= _s.h; return *this; }
        Size& operator+=(int _wh) { w += _wh; h += _wh; return *this; }
        Size& operator-=(int _wh) { w -= _wh; h -= _wh; return *this; }
        Size& operator*=(int _wh) { w *= _wh; h *= _wh; return *this; }

        Size operator+(const Size& _s) const { return Size(w + _s.w, h + _s.h); }
        Size operator+(int _wh) const { return Size(w + _wh, h + _wh); }
        friend Size operator+(int _wh, const Size& _s) { return Size(_wh + _s.w, _wh + _s.h); }
        Size operator-(const Size& _s) const { return Size(w - _s.w, h - _s.h); }
        Size operator-(int _wh) const { return Size(w - _wh, h - _wh); }
        friend Size operator-(int _wh, const Size& _s) { return Size(_wh - _s.w, _wh - _s.h); }
        Size operator*(const Size& _s) const { return Size(w * _s.w, h * _s.h); }
        Size operator*(int _wh) const { return Size(w * _wh, h * _wh); }
        friend Size operator*(int _wh, const Size& _s) { return Size(_wh * _s.w, _wh * _s.h); }

        bool operator==(const Size& _s) const { return w == _s.w && h == _s.h; }
        bool operator==(int _wh) const { return w == _wh && h == _wh; }
        friend bool operator==(int _wh, const Size& _s) { return _wh == _s.w && _wh == _s.h; }
        bool operator!=(const Size& _s) const { return !(*this == _s); }
        bool operator!=(int _wh) const { return !(*this == _wh); }
        friend bool operator!=(int _wh, const Size& _s) { return !(_wh == _s); }

        /// @brief Absolute area.
        int Area() const { return std::abs(w) * std::abs(h); }
        /// @brief Returns true if width or height is zero.
        bool IsEmptyArea() const { return w == 0 || h == 0; }
        /// @brief Returns size with absolute components.
        Size Absolute() const { return Size(std::abs(w), std::abs(h)); }
    };

    /// @brief Integer rectangle with support for negative extents.
    class Rectangle {
    public:
        /// @brief Origin and size components.
        int x = 0, y = 0, w = 0, h = 0;

        Rectangle() = default;
        Rectangle(int _xywh) : x(_xywh), y(_xywh), w(_xywh), h(_xywh) {}
        Rectangle(int _xy, int _wh) : x(_xy), y(_xy), w(_wh), h(_wh) {}
        Rectangle(int _x, int _y, int _w, int _h) : x(_x), y(_y), w(_w), h(_h) {}
        Rectangle(const Point& _p, int _wh) : x(_p.x), y(_p.y), w(_wh), h(_wh) {}
        Rectangle(const Point& _p, int _w, int _h) : x(_p.x), y(_p.y), w(_w), h(_h) {}
        Rectangle(int _xy, const Size& _s) : x(_xy), y(_xy), w(_s.w), h(_s.h) {}
        Rectangle(int _x, int _y, const Size& _s) : x(_x), y(_y), w(_s.w), h(_s.h) {}
        Rectangle(const Point& _p, const Size& _s) : x(_p.x), y(_p.y), w(_s.w), h(_s.h) {}

        Rectangle operator-() const { return Rectangle(-x, -y, -w, -h); }

        Rectangle& operator+=(const Rectangle& _r) { x += _r.x; y += _r.y; w += _r.w; h += _r.h; return *this; }
        Rectangle& operator-=(const Rectangle& _r) { x -= _r.x; y -= _r.y; w -= _r.w; h -= _r.h; return *this; }
        Rectangle& operator*=(const Rectangle& _r) { x *= _r.x; y *= _r.y; w *= _r.w; h *= _r.h; return *this; }
        Rectangle& operator+=(const Point& _p) { x += _p.x; y += _p.y; return *this; }
        Rectangle& operator-=(const Point& _p) { x -= _p.x; y -= _p.y; return *this; }
        Rectangle& operator*=(const Point& _p) { x *= _p.x; y *= _p.y; return *this; }
        Rectangle& operator+=(const Size& _s) { w += _s.w; h += _s.h; return *this; }
        Rectangle& operator-=(const Size& _s) { w -= _s.w; h -= _s.h; return *this; }
        Rectangle& operator*=(const Size& _s) { w *= _s.w; h *= _s.h; return *this; }
        Rectangle& operator+=(int _xywh) { x += _xywh; y += _xywh; w += _xywh; h += _xywh; return *this; }
        Rectangle& operator-=(int _xywh) { x -= _xywh; y -= _xywh; w -= _xywh; h -= _xywh; return *this; }
        Rectangle& operator*=(int _xywh) { x *= _xywh; y *= _xywh; w *= _xywh; h *= _xywh; return *this; }

        bool operator==(const Rectangle& _r) const { return x == _r.x && y == _r.y && w == _r.w && h == _r.h; }
        bool operator==(int _xywh) const { return x == _xywh && y == _xywh && w == _xywh && h == _xywh; }
        bool operator!=(const Rectangle& _r) const { return !(*this == _r); }
        bool operator!=(int _xywh) const { return !(*this == _xywh); }

        /// @brief Gets rectangle origin as Point.
        Point GetPoint() const { return Point(x, y); }
        /// @brief Gets rectangle size as Size.
        Size GetSize() const { return Size(w, h); }

        /// @brief Absolute width.
        int Width() const { return std::abs(w); }
        /// @brief Absolute height.
        int Height() const { return std::abs(h); }
        /// @brief Absolute area.
        int Area() const { return std::abs(w) * std::abs(h); }
        /// @brief Returns true if width or height is zero.
        bool IsEmptyArea() const { return w == 0 || h == 0; }
        /// @brief Gets absolute size.
        Size AbsoluteSize() const { return Size(std::abs(w), std::abs(h)); }

        /// @brief Left inclusive boundary.
        int LeftSide() const { return w >= 0 ? x : (x + w + 1); }
        /// @brief Right inclusive boundary.
        int RightSide() const { return w > 0 ? x + w - 1 : x; }
        /// @brief Top inclusive boundary.
        int TopSide() const { return h >= 0 ? y : (y + h + 1); }
        /// @brief Bottom inclusive boundary.
        int BottomSide() const { return h > 0 ? y + h - 1 : y; }

        Point TopLeft() const { return Point(LeftSide(), TopSide()); }
        Point TopRight() const { return Point(RightSide(), TopSide()); }
        Point BottomLeft() const { return Point(LeftSide(), BottomSide()); }
        Point BottomRight() const { return Point(RightSide(), BottomSide()); }

        Rectangle TopLeftRectangle() const { return Rectangle(LeftSide(), TopSide(), std::abs(w), std::abs(h)); }

        /// @brief Returns true if point is inside rectangle.
        bool IsContain(const Point& p) const {
            return (p.x >= LeftSide() && p.x <= RightSide()) && (p.y >= TopSide() && p.y <= BottomSide());
        }
        /// @brief Returns true if rectangle is fully contained in this rectangle.
        bool IsContain(const Rectangle& r) const {
            return (r.LeftSide() >= LeftSide() && r.RightSide() <= RightSide() && r.TopSide() >= TopSide() && r.BottomSide() <= BottomSide());
        }
        /// @brief Returns true if this rectangle is contained in @p r.
        bool IsContainedBy(const Rectangle& r) const { return r.IsContain(*this); }
        /// @brief Returns true if this rectangle represents exactly point @p p.
        bool IsIdenticalAs(const Point& p) const { return std::abs(w) == 1 && std::abs(h) == 1 && LeftSide() == p.x && TopSide() == p.y; }
        /// @brief Returns true if both rectangles cover same area.
        bool IsIdenticalAs(const Rectangle& r) const {
            return std::abs(w) == std::abs(r.w) && std::abs(h) == std::abs(r.h)
                && LeftSide() == r.LeftSide() && RightSide() == r.RightSide()
                && TopSide() == r.TopSide() && BottomSide() == r.BottomSide();
        }

        /// @brief Returns true if rectangles overlap.
        bool IsIntersectWith(const Rectangle& r) const {
            bool xInter = std::max(LeftSide(), r.LeftSide()) <= std::min(RightSide(), r.RightSide());
            bool yInter = std::max(TopSide(), r.TopSide()) <= std::min(BottomSide(), r.BottomSide());
            return xInter && yInter;
        }

        /// @brief Converts local point to global point using this rectangle origin.
        Point LocalToGlobal(const Point& local) const { return Point(local.x + LeftSide(), local.y + TopSide()); }
        /// @brief Converts local rectangle to global rectangle using this rectangle origin.
        Rectangle LocalToGlobal(const Rectangle& local) const { return Rectangle(local.x + LeftSide(), local.y + TopSide(), local.w, local.h); }
        /// @brief Converts global point to local point using this rectangle origin.
        Point GlobalToLocal(const Point& global) const { return Point(global.x - LeftSide(), global.y - TopSide()); }
        /// @brief Converts global rectangle to local rectangle using this rectangle origin.
        Rectangle GlobalToLocal(const Rectangle& global) const { return Rectangle(global.x - LeftSide(), global.y - TopSide(), global.w, global.h); }

        /// @brief Union with another rectangle.
        Rectangle UnionWith(const Rectangle& r) const { return Rectangle::Union(*this, r); }
        /// @brief Intersection with another rectangle.
        Rectangle IntersectWith(const Rectangle& r) const { return Rectangle::Intersect(*this, r); }

        /// @brief Returns union of two rectangles.
        static Rectangle Union(const Rectangle& r1, const Rectangle& r2) {
            int l = std::min(r1.LeftSide(), r2.LeftSide()), r = std::max(r1.RightSide(), r2.RightSide());
            int t = std::min(r1.TopSide(), r2.TopSide()), b = std::max(r1.BottomSide(), r2.BottomSide());
            return Rectangle(l, t, r - l + 1, b - t + 1);
        }
        /// @brief Returns intersection of two rectangles.
        static Rectangle Intersect(const Rectangle& r1, const Rectangle& r2) {
            int l = std::max(r1.LeftSide(), r2.LeftSide()), r = std::min(r1.RightSide(), r2.RightSide());
            int t = std::max(r1.TopSide(), r2.TopSide()), b = std::min(r1.BottomSide(), r2.BottomSide());
            if (l > r || t > b) return Rectangle(0); // No intersect
            return Rectangle(l, t, r - l + 1, b - t + 1);
        }
        /// @brief Creates rectangle covering both points.
        static Rectangle FromTwoPoints(const Point& p1, const Point& p2) {
            int l = std::min(p1.x, p2.x), r = std::max(p1.x, p2.x), t = std::min(p1.y, p2.y), b = std::max(p1.y, p2.y);
            return Rectangle(l, t, r - l + 1, b - t + 1);
        }
    };
}
