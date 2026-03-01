#pragma once

#include "Engine_Graphics.h"
#include "Engine_Interpolation.h"

namespace Engine {
    /// @brief Interpolation-aware renderer built on top of pixel graphics.
    /// @tparam ColorT Color/fragment payload type.
    template <typename ColorT>
    class RendererGraphics : public GraphicsWrapper<ColorT> {
    private:
        IInterpolator<ColorT> &interpolator;

    public:
        /// @brief Constructs a renderer from a graphics target and interpolator.
        /// @param _graphics Pixel graphics target.
        /// @param _interpolator Interpolator used for lines/triangles.
        RendererGraphics(IGraphics<ColorT> &_graphics, IInterpolator<ColorT> &_interpolator)
            : GraphicsWrapper<ColorT>(_graphics), interpolator(_interpolator) {}

        /// @brief Renders a single point.
        bool RenderPoint(int x, int y, const ColorT &color);

        /// @brief Renders a single point.
        bool RenderPoint(const Point &p, const ColorT &color);

        /// @brief Renders a color-interpolated line.
        void RenderLine(int xa, int ya, int xb, int yb, const ColorT &a_color, const ColorT &b_color);

        /// @brief Renders a color-interpolated line.
        void RenderLine(const Point &a, const Point &b, const ColorT &a_color, const ColorT &b_color);

        /// @brief Renders a color-interpolated line, excluding start point.
        void RenderLineExcludeStart(int xa, int ya, int xb, int yb, const ColorT &a_color, const ColorT &b_color);

        /// @brief Renders a color-interpolated line, excluding start point.
        void RenderLineExcludeStart(const Point &a, const Point &b, const ColorT &a_color, const ColorT &b_color);

        /// @brief Renders a color-interpolated line, excluding end point.
        void RenderLineExcludeEnd(int xa, int ya, int xb, int yb, const ColorT &a_color, const ColorT &b_color);

        /// @brief Renders a color-interpolated line, excluding end point.
        void RenderLineExcludeEnd(const Point &a, const Point &b, const ColorT &a_color, const ColorT &b_color);

        /// @brief Renders a color-interpolated filled triangle.
        void RenderTriangle(int xa, int ya, int xb, int yb, int xc, int yc, const ColorT &a_color, const ColorT &b_color, const ColorT &c_color);

        /// @brief Renders a color-interpolated filled triangle.
        void RenderTriangle(const Point &a, const Point &b, const Point &c, const ColorT &a_color, const ColorT &b_color, const ColorT &c_color);
    };
}

template <typename ColorT>
inline bool Engine::RendererGraphics<ColorT>::RenderPoint(int x, int y, const ColorT &color) {
  return this->DrawPoint(x, y, color);
}
template <typename ColorT>
inline bool Engine::RendererGraphics<ColorT>::RenderPoint(const Point &p, const ColorT &color) {
  return this->RenderPoint(p.x, p.y, color);
}

template <typename ColorT>
inline void Engine::RendererGraphics<ColorT>::RenderLine(int xa, int ya, int xb, int yb, const ColorT &a_color, const ColorT &b_color) {
    int n = std::max(std::abs(xb - xa),
                    std::abs(yb - ya)); // n = number of pixel draw - 1
    int pixel_i = 0;                     // Pixel index

    ColorT curr_c = a_color;
    auto draw_func = [this, &a_color, &b_color, &pixel_i, &n, &curr_c](int x, int y, const ColorT &color) -> bool {
        // Get color
        this->interpolator.Linear(a_color, b_color, (double)pixel_i / n, curr_c);
        ++pixel_i;

        return this->RenderPoint(x, y, curr_c);
    };

    FunctionGraphics<ColorT> func_g(draw_func);
    PrimitiveGraphics<ColorT> prim_g(func_g);

    return prim_g.DrawLine(xa, ya, xb, yb, a_color);
}
template <typename ColorT>
inline void Engine::RendererGraphics<ColorT>::RenderLine(const Point &a, const Point &b, const ColorT &color_a, const ColorT &color_b) {
    this->RenderLine(a.x, a.y, b.x, b.y, color_a, color_b);
}

template <typename ColorT>
inline void Engine::RendererGraphics<ColorT>::RenderLineExcludeStart(int xa, int ya, int xb, int yb, const ColorT &a_color, const ColorT &b_color) {
    int n = std::max(std::abs(xb - xa),
                    std::abs(yb - ya)); // n = number of pixel draw - 1
    int pixel_i = 1; // Pixel index (exclude start so start at 1)

    ColorT curr_c = a_color;
    auto draw_func = [this, &a_color, &b_color, &pixel_i, &n,
                        &curr_c](int x, int y, const ColorT &color) -> bool {
        // Get color
        this->interpolator.Linear(a_color, b_color, (double)pixel_i / n, curr_c);
        ++pixel_i;

        return this->RenderPoint(x, y, curr_c);
    };

    FunctionGraphics<ColorT> func_g(draw_func);
    PrimitiveGraphics<ColorT> prim_g(func_g);

    return prim_g.DrawLineExcludeStart(xa, ya, xb, yb, a_color);
}
template <typename ColorT>
inline void Engine::RendererGraphics<ColorT>::RenderLineExcludeStart(const Point &a, const Point &b, const ColorT &color_a, const ColorT &color_b) {
    this->RenderLineExcludeStart(a.x, a.y, b.x, b.y, color_a, color_b);
}

template <typename ColorT>
inline void Engine::RendererGraphics<ColorT>::RenderLineExcludeEnd(int xa, int ya, int xb, int yb, const ColorT &a_color, const ColorT &b_color) {
    int n = std::max(std::abs(xb - xa),
                    std::abs(yb - ya)); // n = number of pixel draw - 1
    int pixel_i = 0;                     // Pixel index

    ColorT curr_c = a_color;
    auto draw_func = [this, &a_color, &b_color, &pixel_i, &n,
                        &curr_c](int x, int y, const ColorT &color) -> bool {
        // Get color
        this->interpolator.Linear(a_color, b_color, (double)pixel_i / n, curr_c);
        ++pixel_i;

        return this->RenderPoint(x, y, curr_c);
    };

    FunctionGraphics<ColorT> func_g(draw_func);
    PrimitiveGraphics<ColorT> prim_g(func_g);

    return prim_g.DrawLineExcludeEnd(xa, ya, xb, yb, a_color);
}
template <typename ColorT>
inline void Engine::RendererGraphics<ColorT>::RenderLineExcludeEnd(const Point &a, const Point &b, const ColorT &color_a, const ColorT &color_b) {
    this->RenderLineExcludeEnd(a.x, a.y, b.x, b.y, color_a, color_b);
}

template <typename ColorT>
inline void Engine::RendererGraphics<ColorT>::RenderTriangle(int xa, int ya, int xb, int yb, int xc, int yc, const ColorT &color_a, const ColorT &color_b, const ColorT &color_c) {
    // Point case
    if (xa == xb && xa == xc && ya == yb && ya == yc) {
        this->RenderPoint(xa, ya, color_a);
        this->RenderPoint(xb, yb, color_b);
        this->RenderPoint(xc, yc, color_c);
        return;
    }

    // Line case
    // - AB overlap
    if (xa == xb && ya == yb) {
        this->RenderLine(xa, ya, xc, yc, color_a, color_c);
        this->RenderLineExcludeStart(xc, yc, xb, yb, color_c, color_b);
        return;
    }
    // - AC overlap
    if (xa == xc && ya == yc) {
        this->RenderLine(xa, ya, xb, yb, color_a, color_b);
        this->RenderLineExcludeStart(xb, yb, xc, yc, color_b, color_c);
        return;
    }
    // - BC overlap
    if (xb == xc && yb == yc) {
        this->RenderLine(xb, yb, xa, ya, color_b, color_a);
        this->RenderLineExcludeStart(xa, ya, xc, yc, color_a, color_c);
        return;
    }
    // - General line case
    long long cross_prod = (long long)(yb - yc) * (xa - xc) + (long long)(xc - xb) * (ya - yc);
    if (cross_prod == 0) {
        // Sort then render strategy (0 = a, 1 = b, 2 = c). Yes this sort the
        // mapping.
        int idx[3] = {0, 1, 2};

        auto get_x = [&xa, &xb, &xc](int i) { return i == 0 ? xa : (i == 1 ? xb : xc); };
        auto get_y = [&ya, &yb, &yc](int i) { return i == 0 ? ya : (i == 1 ? yb : yc); };
        auto get_color = [&color_a, &color_b, &color_c](int i) {
            return i == 0 ? color_a : (i == 1 ? color_b : color_c);
        };

        // - If vertical line -> compare horizontal
        bool vertical = (xa == xb && xa == xc);
        auto is_less = [&vertical, &get_x, &get_y](int i1, int i2) -> bool {
            return vertical ? get_y(i1) < get_y(i2) : (get_x(i1) < get_x(i2));
        };

        // - Sorting 3 element
        if (is_less(idx[1], idx[0]))
            std::swap(idx[0], idx[1]);
        if (is_less(idx[2], idx[1]))
            std::swap(idx[1], idx[2]);
        if (is_less(idx[1], idx[0]))
            std::swap(idx[0], idx[1]);

        // - Rendering
        this->RenderLine(get_x(idx[0]), get_y(idx[0]), get_x(idx[1]), get_y(idx[1]),
                        get_color(idx[0]), get_color(idx[1]));
        this->RenderLineExcludeStart(get_x(idx[1]), get_y(idx[1]), get_x(idx[2]),
                                    get_y(idx[2]), get_color(idx[1]),
                                    get_color(idx[2]));
        return;
    }

    // General triangle case
    ColorT curr_c = color_a;
    double factor = 1.0 / cross_prod;

    auto draw_func = [&](int x, int y, const ColorT &color) -> bool {
        // Compute weight
        double w_a =
            ((long long)(yb - yc) * (x - xc) + (long long)(xc - xb) * (y - yc)) *
            factor;
        double w_b =
            ((long long)(yc - ya) * (x - xc) + (long long)(xa - xc) * (y - yc)) *
            factor;
        double w_c = 1.0 - w_a - w_b;

        // Compute final color and render
        this->interpolator.Triangle(color_a, color_b, color_c, w_a, w_b, w_c,
                                    curr_c);

        this->RenderPoint(x, y, curr_c);
        return true;
    };

    // - Rendering triangle (use primitive graphics, pre-defined logic).
    FunctionGraphics<ColorT> func_g(draw_func);
    PrimitiveGraphics<ColorT> prim_g(func_g);

    return prim_g.FillTriangle(xa, ya, xb, yb, xc, yc, color_a);
}
template <typename ColorT>
inline void Engine::RendererGraphics<ColorT>::RenderTriangle(const Point &a, const Point &b, const Point &c, const ColorT &color_a, const ColorT &color_b, const ColorT &color_c) {
    return this->RenderTriangle(a.x, a.y, b.x, b.y, c.x, c.y, color_a, color_b, color_c);
}