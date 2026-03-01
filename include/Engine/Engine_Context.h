#pragma once

namespace Engine {
    /// @brief Abstract pixel read/write interface for drawing backends.
    /// @tparam ColorT Pixel color type.
    template <typename ColorT>
    struct IDrawingContext {
        /// @brief Virtual destructor.
        virtual ~IDrawingContext() = default;

        /// @brief Writes a pixel at the given coordinate.
        /// @param x X coordinate.
        /// @param y Y coordinate.
        /// @param color Pixel color to write.
        /// @return True if write succeeds, otherwise false.
        virtual bool SetPixel(int x, int y, const ColorT& color) = 0;

        /// @brief Reads a pixel at the given coordinate.
        /// @param x X coordinate.
        /// @param y Y coordinate.
        /// @param res Output pixel color.
        /// @return True if read succeeds, otherwise false.
        virtual bool GetPixel(int x, int y, ColorT& res) = 0;
    };
}
