#pragma once

#include "Engine_Context.h"
#include "Engine_Geometry.h"

#include <climits>
#include <math.h>
#include <functional>
#include <utility>
#include <vector>

namespace Engine {
    /// @brief Pixel plotting interface.
    /// @tparam ColorT Pixel color type.
    template <typename ColorT>
    struct IGraphics {
        /// @brief Virtual destructor.
        virtual ~IGraphics() = default;

        /// @brief Draws one pixel.
        /// @param x X coordinate.
        /// @param y Y coordinate.
        /// @param color Pixel color.
        /// @return True if draw succeeds, otherwise false.
        virtual bool DrawPoint(int x, int y, const ColorT& color) = 0;
    };

    /// @brief Forwarding wrapper around an IGraphics target.
    /// @tparam ColorT Pixel color type.
    template <typename ColorT>
    struct GraphicsWrapper : public IGraphics<ColorT> {
    private:
        IGraphics<ColorT>& graphics;
    public:
        /// @brief Constructs a wrapper for an existing graphics target.
        /// @param _graphics Wrapped graphics implementation.
        GraphicsWrapper(IGraphics<ColorT>& _graphics) : graphics(_graphics) {}

        /// @brief Forwards point drawing to wrapped target.
        bool DrawPoint(int x, int y, const ColorT& color) override { return graphics.DrawPoint(x, y, color); }

        /// @brief Gets wrapped graphics target (const).
        const IGraphics<ColorT>& GetInternalGraphics() const { return graphics; }

        /// @brief Gets wrapped graphics target.
        IGraphics<ColorT>& GetInternalGraphics() { return graphics; }
    };

    /// @brief Graphics adapter backed by IDrawingContext.
    /// @tparam ColorT Pixel color type.
    template <typename ColorT>
    class ContextGraphics : public IGraphics<ColorT> {
    private:
        IDrawingContext<ColorT>& context;
    public:
        /// @brief Constructs adapter from drawing context.
        /// @param _context Pixel context backend.
        ContextGraphics(IDrawingContext<ColorT>& _context) : context(_context) {}

        /// @brief Writes one pixel into context.
        bool DrawPoint(int x, int y, const ColorT& color) override { return context.SetPixel(x, y, color); }

        /// @brief Gets wrapped context (const).
        const IDrawingContext<ColorT>& GetContext() const { return context; }

        /// @brief Gets wrapped context.
        IDrawingContext<ColorT>& GetContext() { return context; }
    };

    /// @brief Graphics adapter backed by callback function.
    /// @tparam ColorT Pixel color type.
    template <typename ColorT>
    class FunctionGraphics : public IGraphics<ColorT> {
    private:
        std::function<bool(int, int, const ColorT&)> func;
    public:
        /// @brief Constructs adapter from draw callback.
        /// @param _func Callback invoked per pixel.
        FunctionGraphics(const std::function<bool(int, int, const ColorT&)>& _func) : func(_func) {}

        /// @brief Draws one pixel through callback.
        bool DrawPoint(int x, int y, const ColorT& color) override {
            return !func ? false : func(x, y, color);
        }
    };

    /// @brief Applies local viewport mapping before forwarding points.
    /// @tparam ColorT Pixel color type.
    template <typename ColorT>
    class ViewportGraphics : public GraphicsWrapper<ColorT> {
    private:
        Rectangle viewport;
    public:
        /// @brief Constructs viewport graphics wrapper.
        /// @param _graphics Wrapped graphics target.
        /// @param _viewport Viewport rectangle.
        /// @param _enable Reserved compatibility argument.
        ViewportGraphics(IGraphics<ColorT>& _graphics, const Rectangle& _viewport, bool _enable = true)
            : GraphicsWrapper<ColorT>(_graphics), viewport(_viewport) {}

        /// @brief Sets viewport rectangle.
        void SetViewport(const Rectangle& _viewport) { viewport = _viewport; }

        /// @brief Gets viewport rectangle.
        Rectangle GetViewport() const { return viewport; }


        /// @brief Draws point in local viewport coordinates.
        bool DrawPoint(int x, int y, const ColorT& color) override {
            if (viewport.IsEmptyArea() || x < 0 || x >= std::abs(viewport.w) || y < 0 || y >= std::abs(viewport.h))
                return false;
            return GraphicsWrapper<ColorT>::DrawPoint(x + viewport.LeftSide(), y + viewport.TopSide(), color);
        }
    };

    /// @brief Clips points to a rectangle before forwarding.
    /// @tparam ColorT Pixel color type.
    template <typename ColorT>
    class ClippedGraphics : public GraphicsWrapper<ColorT> {
    private:
        Rectangle clipped;
    public:
        /// @brief Constructs clipping graphics wrapper.
        /// @param _graphics Wrapped graphics target.
        /// @param _clipped Clipping rectangle.
        /// @param _enable Reserved compatibility argument.
        ClippedGraphics(IGraphics<ColorT>& _graphics, const Rectangle& _clipped, bool _enable = true)
            : GraphicsWrapper<ColorT>(_graphics), clipped(_clipped) {}

        /// @brief Sets clipping rectangle.
        void SetClipped(const Rectangle& _clipped) { clipped = _clipped; }

        /// @brief Gets clipping rectangle.
        Rectangle GetClipped() const { return clipped; }

        /// @brief Draws point only when inside clipping rectangle.
        bool DrawPoint(int x, int y, const ColorT& color) override {
            return clipped.IsContain(Point(x, y)) ? GraphicsWrapper<ColorT>::DrawPoint(x, y, color) : false;
        }
    };

    /// @brief Converts input color type then forwards drawing.
    /// @tparam ColorTIn Input color type.
    /// @tparam ColorTOut Output color type.
    template <typename ColorTIn, typename ColorTOut>
    class ConvertedGraphics : public IGraphics<ColorTIn> {
    private:
        IGraphics<ColorTOut>& graphics;
        std::function<ColorTOut(const ColorTIn&)> convert;
    public:
        /// @brief Constructs color-converting wrapper.
        /// @param _graphics Wrapped graphics target.
        /// @param _convert Color conversion callback.
        ConvertedGraphics(IGraphics<ColorTOut>& _graphics, const std::function<ColorTOut(const ColorTIn&)>& _convert) : graphics(_graphics), convert(_convert) {}

        /// @brief Converts and draws a point.
        bool DrawPoint(int x, int y, const ColorTIn& color) override {
            return graphics.DrawPoint(x, y, convert(color));
        }
    };

    /// @brief Primitive rasterization helper on top of IGraphics.
    /// @tparam ColorT Pixel color type.
    template <typename ColorT>
    class PrimitiveGraphics : public GraphicsWrapper<ColorT> {
    public:
        /// @brief Constructs primitive rasterizer wrapper.
        /// @param _graphics Wrapped graphics target.
        PrimitiveGraphics(IGraphics<ColorT>& _graphics) : GraphicsWrapper<ColorT>(_graphics) {}

        /// @brief Draws one point.
        bool DrawPoint(int x, int y, const ColorT& color) override;
        /// @brief Draws one point.
        bool DrawPoint(const Point& p, const ColorT& color);

        /// @brief Draws horizontal line segment.
        void DrawHorizontalLine(int x1, int y, int x2, const ColorT& color);
        /// @brief Draws horizontal line segment.
        void DrawHorizontalLine(const Point& p, int x2, const ColorT& color);

        /// @brief Draws vertical line segment.
        void DrawVerticalLine(int x, int y1, int y2, const ColorT& color);
        /// @brief Draws vertical line segment.
        void DrawVerticalLine(const Point& p, int y2, const ColorT& color);

        /// @brief Draws line segment.
        void DrawLine(int x1, int y1, int x2, int y2, const ColorT& color);
        /// @brief Draws line segment.
        void DrawLine(const Point& p1, const Point& p2, const ColorT& color);

        /// @brief Draws line excluding start point.
        void DrawLineExcludeStart(int x1, int y1, int x2, int y2, const ColorT& color);
        /// @brief Draws line excluding start point.
        void DrawLineExcludeStart(const Point& p1, const Point& p2, const ColorT& color);

        /// @brief Draws line excluding end point.
        void DrawLineExcludeEnd(int x1, int y1, int x2, int y2, const ColorT& color);
        /// @brief Draws line excluding end point.
        void DrawLineExcludeEnd(const Point& p1, const Point& p2, const ColorT& color);

        /// @brief Draws rectangle outline.
        void DrawRectangle(int x, int y, int w, int h, const ColorT& color);
        /// @brief Draws rectangle outline.
        void DrawRectangle(const Point& pos, const Size& size, const ColorT& color);
        /// @brief Draws rectangle outline.
        void DrawRectangle(const Rectangle& r, const ColorT& color);

        /// @brief Fills rectangle area.
        void FillRectangle(int x, int y, int w, int h, const ColorT& color);
        /// @brief Fills rectangle area.
        void FillRectangle(const Point& pos, const Size& size, const ColorT& color);
        /// @brief Fills rectangle area.
        void FillRectangle(const Rectangle& r, const ColorT& color);

        /// @brief Draws triangle outline.
        void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, const ColorT& color);
        /// @brief Draws triangle outline.
        void DrawTriangle(const Point& a, const Point& b, const Point& c, const ColorT& color);

        /// @brief Fills triangle area.
        void FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, const ColorT& color);
        /// @brief Fills triangle area.
        void FillTriangle(const Point& a, const Point& b, const Point& c, const ColorT& color);
    };
}

template <typename ColorT>
inline bool Engine::PrimitiveGraphics<ColorT>::DrawPoint(int x, int y, const ColorT& color) {
    return GraphicsWrapper<ColorT>::DrawPoint(x, y, color);
}
template <typename ColorT>
inline bool Engine::PrimitiveGraphics<ColorT>::DrawPoint(const Point& p, const ColorT& color) {
    return GraphicsWrapper<ColorT>::DrawPoint(p.x, p.y, color);
}

template <typename ColorT>
inline void Engine::PrimitiveGraphics<ColorT>::DrawHorizontalLine(int x1, int y, int x2, const ColorT& color) {
    int l = std::min(x1, x2), r = std::max(x1, x2);
    for (int x = l; x <= r; ++x)
        this->DrawPoint(x, y, color);
}
template <typename ColorT>
inline void Engine::PrimitiveGraphics<ColorT>::DrawHorizontalLine(const Point& p, int x2, const ColorT& color) {
    return this->DrawHorizontalLine(p.x, p.y, x2, color);
}

template <typename ColorT>
inline void Engine::PrimitiveGraphics<ColorT>::DrawVerticalLine(int x, int y1, int y2, const ColorT& color) {
    int t = std::min(y1, y2), b = std::max(y1, y2);
    for (int y = t; y <= b; ++y)
        this->DrawPoint(x, y, color);
}
template <typename ColorT>
inline void Engine::PrimitiveGraphics<ColorT>::DrawVerticalLine(const Point& p, int y2, const ColorT& color) {
    return this->DrawVerticalLine(p.x, p.y, y2, color);
}

template <typename ColorT>
inline void Engine::PrimitiveGraphics<ColorT>::DrawLine(int x1, int y1, int x2, int y2, const ColorT& color) {
    int dx = x2 - x1, dy = y2 - y1;
    int max_diff = std::max(std::abs(dx), std::abs(dy));
    // Point case
    if (max_diff == 0) {
        this->DrawPoint(x1, y1, color);
        return;
    }

    // For the question of why not use dx/dy and dy/dx
    // - Simply because if dx, dy both < 0, then dx/dy, dy/dx > 0 => drawing in wrong direction.
    double x_step = (dx < 0 ? -1 : 1) * ((double)std::abs(dx) / max_diff);
    double y_step = (dy < 0 ? -1 : 1) * ((double)std::abs(dy) / max_diff);
    double x = x1, y = y1;

    for (int i = 0; i <= max_diff; ++i, x += x_step, y += y_step)
        this->DrawPoint((int)round(x), (int)round(y), color);

}
template <typename ColorT>
inline void Engine::PrimitiveGraphics<ColorT>::DrawLine(const Point& p1, const Point& p2, const ColorT& color) {
    return this->DrawLine(p1.x, p1.y, p2.x, p2.y, color);
}

template <typename ColorT>
inline void Engine::PrimitiveGraphics<ColorT>::DrawLineExcludeStart(int x1, int y1, int x2, int y2, const ColorT& color) {
    int dx = x2 - x1, dy = y2 - y1;
    int max_diff = std::max(std::abs(dx), std::abs(dy));
    // Point case
    if (max_diff == 0) {
        this->DrawPoint(x1, y1, color);
        return;
    }

    // For the question of why not use dx/dy and dy/dx
    // - Simply because if dx, dy both < 0, then dx/dy, dy/dx > 0 => drawing in wrong direction.
    double x_step = (dx < 0 ? -1 : 1) * ((double)std::abs(dx) / max_diff);
    double y_step = (dy < 0 ? -1 : 1) * ((double)std::abs(dy) / max_diff);
    double x = x1 + x_step, y = y1 + y_step;

    for (int i = 1; i <= max_diff; ++i, x += x_step, y += y_step)
        this->DrawPoint((int)round(x), (int)round(y), color);

}
template <typename ColorT>
inline void Engine::PrimitiveGraphics<ColorT>::DrawLineExcludeStart(const Point& p1, const Point& p2, const ColorT& color) {
    return this->DrawLineExcludeStart(p1.x, p1.y, p2.x, p2.y, color);
}

template <typename ColorT>
inline void Engine::PrimitiveGraphics<ColorT>::DrawLineExcludeEnd(int x1, int y1, int x2, int y2, const ColorT& color) {
    int dx = x2 - x1, dy = y2 - y1;
    int max_diff = std::max(std::abs(dx), std::abs(dy));
    // Point case
    if (max_diff == 0) {
        this->DrawPoint(x1, y1, color);
        return;
    }

    // For the question of why not use dx/dy and dy/dx
    // - Simply because if dx, dy both < 0, then dx/dy, dy/dx > 0 => drawing in wrong direction.
    double x_step = (dx < 0 ? -1 : 1) * ((double)std::abs(dx) / max_diff);
    double y_step = (dy < 0 ? -1 : 1) * ((double)std::abs(dy) / max_diff);
    double x = x1, y = y1;

    for (int i = 0; i < max_diff; ++i, x += x_step, y += y_step)
        this->DrawPoint((int)round(x), (int)round(y), color);

}
template <typename ColorT>
inline void Engine::PrimitiveGraphics<ColorT>::DrawLineExcludeEnd(const Point& p1, const Point& p2, const ColorT& color) {
    return this->DrawLineExcludeEnd(p1.x, p1.y, p2.x, p2.y, color);
}

template <typename ColorT>
inline void Engine::PrimitiveGraphics<ColorT>::DrawRectangle(const Rectangle& r, const ColorT& color) {
    int _l = r.LeftSide(), _r = r.RightSide(), _t = r.TopSide(), _b = r.BottomSide();
    // Top & Bottom Line
    for (int x = _l; x <= _r; ++x) {
        this->DrawPoint(x, _t, color); // Top
        this->DrawPoint(x, _b, color); // Bottom
    }
    // Left & Right Line (_t + 1 -> _b - 1 because excluding top-bottom line)
    for (int y = _t + 1; y < _b; ++y) {
        this->DrawPoint(_l, y, color); // Left
        this->DrawPoint(_r, y, color); // Right
    }
}
template <typename ColorT>
inline void Engine::PrimitiveGraphics<ColorT>::DrawRectangle(const Point& pos, const Size& size, const ColorT& color) {
    return this->DrawRectangle(Rectangle(pos, size), color);
}
template <typename ColorT>
inline void Engine::PrimitiveGraphics<ColorT>::DrawRectangle(int x, int y, int w, int h, const ColorT& color) {
    return this->DrawRectangle(Rectangle(x, y, w, h), color);
}

template <typename ColorT>
inline void Engine::PrimitiveGraphics<ColorT>::FillRectangle(const Rectangle& r, const ColorT& color) {
    int _l = r.LeftSide(), _r = r.RightSide(), _t = r.TopSide(), _b = r.BottomSide();
    for (int y = _t; y <= _b; ++y) {
        for (int x = _l; x <= _r; ++x)
            this->DrawPoint(x, y, color);
    }
}
template <typename ColorT>
inline void Engine::PrimitiveGraphics<ColorT>::FillRectangle(const Point& pos, const Size& size, const ColorT& color) {
    return this->FillRectangle(Rectangle(pos, size), color);
}
template <typename ColorT>
inline void Engine::PrimitiveGraphics<ColorT>::FillRectangle(int x, int y, int w, int h, const ColorT& color) {
    return this->FillRectangle(Rectangle(x, y, w, h), color);
}

template <typename ColorT>
inline void Engine::PrimitiveGraphics<ColorT>::DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, const ColorT& color) {
    // Point case
    if (x1 == x2 && x2 == x3 && y1 == y2 && y2 == y3) {
        this->DrawPoint(x1, y1, color);
        return;
    }

    DrawLineExcludeStart(x1, y1, x2, y2, color);
    DrawLineExcludeStart(x2, y2, x3, y3, color);
    DrawLineExcludeStart(x3, y3, x1, y1, color);
}
template <typename ColorT>
inline void Engine::PrimitiveGraphics<ColorT>::DrawTriangle(const Point& a, const Point& b, const Point& c, const ColorT& color) {
    return this->DrawTriangle(a.x, a.y, b.x, b.y, c.x, c.y, color);
}

template <typename ColorT>
inline void Engine::PrimitiveGraphics<ColorT>::FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, const ColorT& color) {
    // Initialize
    int start_y = std::min(y1, std::min(y2, y3)), end_y = std::max(y1, std::max(y2, y3));
    std::vector<std::pair<int, int>> hori_range(end_y - start_y + 1, std::pair<int, int>(INT_MAX, INT_MIN));
    std::vector<bool> updated_y(end_y - start_y + 1, false);

    // Use to update the x-range
    auto update_range_func = [this, &hori_range, &updated_y, &start_y](int _x, int _y, const ColorT& _color) -> bool {
        if (_y < start_y) return false; // This is unexpected, handle just in case
        std::pair<int, int>& r = hori_range[_y - start_y];
        updated_y[_y - start_y] = true;

        r.first = std::min(r.first, _x);
        r.second = std::max(r.second, _x);

        return true;
    };

    // Simulating drawing line to update the x-range for each y (using FunctionGraphics).
    FunctionGraphics<ColorT> func_g(update_range_func);
    PrimitiveGraphics<ColorT> prim_func_g(func_g);

    prim_func_g.DrawLine(x1, y1, x2, y2, color);
    prim_func_g.DrawLine(x1, y1, x3, y3, color);
    prim_func_g.DrawLine(x2, y2, x3, y3, color);

    // Filling the triangle
    for (int i = 0; i < hori_range.size(); ++i) {
        if (!updated_y[i]) continue;
        std::pair<int, int>& r = hori_range[i];
        DrawHorizontalLine(r.first, i + start_y, r.second, color);
    }
}
template <typename ColorT>
inline void Engine::PrimitiveGraphics<ColorT>::FillTriangle(const Point& a, const Point& b, const Point& c, const ColorT& color) {
    return this->FillTriangle(a.x, a.y, b.x, b.y, c.x, c.y, color);
}
