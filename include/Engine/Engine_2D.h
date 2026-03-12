#pragma once

#include "Engine_WorldGraphics.h"
#include "Engine_Renderer.h"

namespace Engine {
    /**
     * @brief Alias for a vertex in 2D world space.
     * @tparam ColorT The color type.
     */
    template <typename ColorT> using WorldVertex2D = WorldVertex<ColorT, 2>;

    /**
     * @brief Alias for the 2D world graphics interface.
     * @tparam ColorT The color type.
     */
    template <typename ColorT> using IWorldGraphics2D = IWorldGraphics<ColorT, 2>;
    /**
     * @brief Alias for the 2D world graphics wrapper.
     * @tparam ColorT The color type.
     */
    template <typename ColorT> using WorldGraphics2DWrapper = WorldGraphicsWrapper<ColorT, 2>;
    /**
     * @brief Alias for the function-based 2D world graphics.
     * @tparam ColorT The color type.
     */
    template <typename ColorT> using FunctionWorldGraphics2D = FunctionWorldGraphics<ColorT, 2>;
    /**
     * @brief Alias for the matrix-transform 2D world graphics.
     * @tparam ColorT The color type.
     */
    template <typename ColorT> using MatrixWorldGraphics2D = MatrixWorldGraphics<ColorT, 2>;
    /**
     * @brief Alias for the plane-clipping 2D world graphics.
     * @tparam ColorT The color type.
     */
    template <typename ColorT> using PlaneClipWorldGraphics2D = PlaneClipWorldGraphics<ColorT, 2>;
    /**
     * @brief Alias for the viewport-transform 2D world graphics.
     * @tparam ColorT The color type.
     */
    template <typename ColorT> using ViewportTransformWorldGraphics2D = ViewportTransformWorldGraphics<ColorT, 2>;

    /**
     * @brief Alias for a plane (line) in 2D space.
     */
    typedef WorldPlane<2> WorldPlane2D;

    /**
     * @brief An IWorldGraphics2D implementation that forwards triangles to a pixel-level RendererGraphics.
     *        Converts floating-point 2D vertex positions to integer pixel coordinates.
     * @tparam ColorT The color type.
     */
    template <typename ColorT>
    class RendererGraphics2D : public IWorldGraphics2D<ColorT> {
    private:
        RendererGraphics<ColorT>& renderer;
    public:
        /**
         * @brief Constructor.
         * @param _renderer The pixel-level renderer to render triangles onto.
         */
        RendererGraphics2D(RendererGraphics<ColorT>& _renderer) : renderer(_renderer) {}

        void RenderTriangle(const WorldVertex2D<ColorT>& a, const WorldVertex2D<ColorT>& b, const WorldVertex2D<ColorT>& c) override {
            return renderer.RenderTriangle(
                (int)a.position.x(), (int)a.position.y(), (int)b.position.x(), (int)b.position.y(), (int)c.position.x(), (int)c.position.y(),
                a.color, b.color, c.color
            );
        }
    
        /**
         * @brief Returns a reference to the internal pixel renderer.
         * @return A reference to the RendererGraphics.
         */
        RendererGraphics<ColorT>& GetInternalRenderer() { return renderer; }
        /**
         * @brief Returns a const reference to the internal pixel renderer.
         * @return A const reference to the RendererGraphics.
         */
        const RendererGraphics<ColorT>& GetInternalRenderer() const { return renderer; }
    };
}