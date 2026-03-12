#pragma once

#include "Engine_WorldGraphics.h"

namespace Engine {
    /**
     * @brief Alias for a vertex in 4D world space.
     * @tparam ColorT The color type.
     */
    template <typename ColorT> using WorldVertex4D = WorldVertex<ColorT, 4>;

    /**
     * @brief Alias for the 4D world graphics interface.
     * @tparam ColorT The color type.
     */
    template <typename ColorT> using IWorldGraphics4D = IWorldGraphics<ColorT, 4>;
    /**
     * @brief Alias for the 4D world graphics wrapper.
     * @tparam ColorT The color type.
     */
    template <typename ColorT> using WorldGraphics4DWrapper = WorldGraphicsWrapper<ColorT, 4>;
    /**
     * @brief Alias for the function-based 4D world graphics.
     * @tparam ColorT The color type.
     */
    template <typename ColorT> using FunctionWorldGraphics4D = FunctionWorldGraphics<ColorT, 4>;
    /**
     * @brief Alias for the matrix-transform 4D world graphics.
     * @tparam ColorT The color type.
     */
    template <typename ColorT> using MatrixWorldGraphics4D = MatrixWorldGraphics<ColorT, 4>;
    /**
     * @brief Alias for the plane-clipping 4D world graphics.
     * @tparam ColorT The color type.
     */
    template <typename ColorT> using PlaneClipWorldGraphics4D = PlaneClipWorldGraphics<ColorT, 4>;
    /**
     * @brief Alias for the viewport-transform 4D world graphics.
     * @tparam ColorT The color type.
     */
    template <typename ColorT> using ViewportTransformWorldGraphics4D = ViewportTransformWorldGraphics<ColorT, 4>;

    /**
     * @brief Alias for a hyperplane in 4D space.
     */
    typedef WorldPlane<4> WorldPlane4D;
}