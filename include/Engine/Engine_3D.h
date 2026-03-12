#pragma once

#include "Engine_WorldGraphics.h"

namespace Engine {
    /**
     * @brief Alias for a vertex in 3D world space.
     * @tparam ColorT The color type.
     */
    template <typename ColorT> using WorldVertex3D = WorldVertex<ColorT, 3>;

    /**
     * @brief Alias for the 3D world graphics interface.
     * @tparam ColorT The color type.
     */
    template <typename ColorT> using IWorldGraphics3D = IWorldGraphics<ColorT, 3>;
    /**
     * @brief Alias for the 3D world graphics wrapper.
     * @tparam ColorT The color type.
     */
    template <typename ColorT> using WorldGraphics3DWrapper = WorldGraphicsWrapper<ColorT, 3>;
    /**
     * @brief Alias for the function-based 3D world graphics.
     * @tparam ColorT The color type.
     */
    template <typename ColorT> using FunctionWorldGraphics3D = FunctionWorldGraphics<ColorT, 3>;
    /**
     * @brief Alias for the matrix-transform 3D world graphics.
     * @tparam ColorT The color type.
     */
    template <typename ColorT> using MatrixWorldGraphics3D = MatrixWorldGraphics<ColorT, 3>;
    /**
     * @brief Alias for the plane-clipping 3D world graphics.
     * @tparam ColorT The color type.
     */
    template <typename ColorT> using PlaneClipWorldGraphics3D = PlaneClipWorldGraphics<ColorT, 3>;
    /**
     * @brief Alias for the viewport-transform 3D world graphics.
     * @tparam ColorT The color type.
     */
    template <typename ColorT> using ViewportTransformWorldGraphics3D = ViewportTransformWorldGraphics<ColorT, 3>;

    /**
     * @brief Alias for a plane (or line) in 3D space.
     */
    typedef WorldPlane<3> WorldPlane3D;
}