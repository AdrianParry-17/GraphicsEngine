#pragma once

#include "Engine_Context.h"
#include "Engine_Geometry.h"
#include "Engine_Interpolation.h"
#include "Engine_Constant.h"

#include <cmath>
#include <functional>

namespace Engine {
    /**
     * @brief Interface for discrete integer-coordinate texture sampling.
     * @tparam ColorT Texture texel payload type.
     */
    template <typename ColorT>
    struct ITexture {
        /**
         * @brief Virtual destructor.
         */
        virtual ~ITexture() = default;

        /**
         * @brief Samples a texel at integer coordinates.
         * @param x Horizontal texel coordinate.
         * @param y Vertical texel coordinate.
         * @param result Output texel value.
         * @return @c true if sampling succeeded, otherwise @c false.
         */
        virtual bool GetColor(int x, int y, ColorT& result) = 0;
    };

    /**
     * @brief Texture adapter that reads texels from an `IDrawingContext`.
     * @tparam ColorT Texture texel payload type.
     */
    template <typename ColorT>
    class ContextTexture : public ITexture<ColorT> {
    private:
        IDrawingContext<ColorT>& context;
    public:
        /**
         * @brief Constructs a context-backed texture.
         * @param _context Source drawing context.
         */
        ContextTexture(IDrawingContext<ColorT>& _context) : context(_context) {}

        /**
         * @brief Samples a texel from the wrapped drawing context.
         * @param x Horizontal texel coordinate.
         * @param y Vertical texel coordinate.
         * @param result Output texel value.
         * @return @c true if the context contains the requested pixel.
         */
        bool GetColor(int x, int y, ColorT& result) override { return context.GetPixel(x, y, result); }

        /**
         * @brief Returns the wrapped drawing context.
         * @return Mutable reference to the source context.
         */
        IDrawingContext<ColorT>& GetContext() { return context; }
        /**
         * @brief Returns the wrapped drawing context.
         * @return Const reference to the source context.
         */
        const IDrawingContext<ColorT>& GetContext() const { return context; }
    };

    /**
     * @brief Texture implementation backed by a callback.
     * @tparam ColorT Texture texel payload type.
     */
    template <typename ColorT>
    class FunctionTexture : public ITexture<ColorT> {
    private:
        std::function<bool(int, int, ColorT&)> func;
    public:
        /**
         * @brief Constructs a function-backed texture.
         * @param _func Sampling callback.
         */
        FunctionTexture(const std::function<bool(int, int, ColorT&)>& _func) : func(_func) {}

        /**
         * @brief Replaces the sampling callback.
         * @param _func New sampling callback.
         */
        void SetFunction(const std::function<bool(int, int, ColorT&)>& _func) { func = _func; }

        /**
         * @brief Samples the texture through the callback.
         * @param x Horizontal texel coordinate.
         * @param y Vertical texel coordinate.
         * @param result Output texel value.
         * @return @c true if the callback exists and sampling succeeded.
         */
        bool GetColor(int x, int y, ColorT& result) override { return (bool)func ? func(x, y, result) : false; }
    };

    /**
     * @brief Base wrapper that forwards texture sampling to another texture.
     * @tparam ColorT Texture texel payload type.
     */
    template <typename ColorT>
    class TextureWrapper : public ITexture<ColorT> {
    private:
        ITexture<ColorT>& texture;
    public:
        /**
         * @brief Constructs a forwarding texture wrapper.
         * @param _texture Wrapped texture.
         */
        TextureWrapper(ITexture<ColorT>& _texture) : texture(_texture) {}
        
        /**
         * @brief Samples the wrapped texture.
         * @param x Horizontal texel coordinate.
         * @param y Vertical texel coordinate.
         * @param result Output texel value.
         * @return @c true if the wrapped texture returns a value.
         */
        bool GetColor(int x, int y, ColorT& result) override { return texture.GetColor(x, y, result); }

        /**
         * @brief Returns the wrapped texture.
         * @return Mutable reference to the wrapped texture.
         */
        ITexture<ColorT>& GetInternalTexture() { return texture; }
        /**
         * @brief Returns the wrapped texture.
         * @return Const reference to the wrapped texture.
         */
        const ITexture<ColorT>& GetInternalTexture() const { return texture; }
    };

    /**
     * @brief Texture view that remaps local coordinates into a rectangular source region.
     * @tparam ColorT Texture texel payload type.
     */
    template <typename ColorT>
    class ViewportTexture : public TextureWrapper<ColorT> {
    private:
        Rectangle viewport;
    public:
        /**
         * @brief Constructs a viewport texture.
         * @param _texture Wrapped texture.
         * @param _viewport Source viewport rectangle.
         */
        ViewportTexture(ITexture<ColorT>& _texture, const Rectangle& _viewport) : TextureWrapper<ColorT>(_texture), viewport(_viewport) {}

        /**
         * @brief Returns the source viewport rectangle.
         * @return Current viewport rectangle.
         */
        Rectangle GetViewport() const { return viewport; }
        /**
         * @brief Sets the source viewport rectangle.
         * @param _viewport New viewport rectangle.
         */
        void SetViewport(const Rectangle& _viewport) { viewport = _viewport; }

        /**
         * @brief Samples the wrapped texture inside the configured viewport.
         * @param x Horizontal texel coordinate relative to the viewport.
         * @param y Vertical texel coordinate relative to the viewport.
         * @param result Output texel value.
         * @return @c true if the local coordinates fall inside the viewport and the wrapped texture returns a value.
         */
        bool GetColor(int x, int y, ColorT& result) override  {
            if (x < 0 || x >= viewport.Width() || y < 0 || y >= viewport.Height())
                return false;
                return TextureWrapper<ColorT>::GetColor(x + viewport.LeftSide(), y + viewport.TopSide(), result);
        }
    };

    /**
     * @brief Texture wrapper that rejects samples outside a clipping rectangle.
     * @tparam ColorT Texture texel payload type.
     */
    template <typename ColorT>
    class ClippedTexture : public TextureWrapper<ColorT> {
    private:
        Rectangle clipped;
    public:
        /**
         * @brief Constructs a clipped texture.
         * @param _texture Wrapped texture.
         * @param _clipped Clip rectangle.
         */
        ClippedTexture(ITexture<ColorT>& _texture, const Rectangle& _clipped) : TextureWrapper<ColorT>(_texture), clipped(_clipped) {}

        /**
         * @brief Returns the clip rectangle.
         * @return Current clip rectangle.
         */
        Rectangle GetClipArea() const { return clipped; }
        /**
         * @brief Sets the clip rectangle.
         * @param _clipped New clip rectangle.
         */
        void SetClipArea(const Rectangle& _clipped) { clipped = _clipped; }

        /**
         * @brief Samples the wrapped texture only when the coordinates lie inside the clip rectangle.
         * @param x Horizontal texel coordinate.
         * @param y Vertical texel coordinate.
         * @param result Output texel value.
         * @return @c true if the point lies inside the clip rectangle and the wrapped texture returns a value.
         */
        bool GetColor(int x, int y, ColorT& result) override {
            return clipped.IsContain(x, y) ? TextureWrapper<ColorT>::GetColor(x, y, result) : false;
        }
    };

    /**
     * @brief Texture wrapper that tiles a rectangular source area repeatedly.
     * @tparam ColorT Texture texel payload type.
     */
    template <typename ColorT>
    class RepeatedTexture : public TextureWrapper<ColorT> {
    private:
        Rectangle src_area;
    public:
        /**
         * @brief Constructs a repeated texture view.
         * @param _texture Wrapped texture.
         * @param _src_area Source area to repeat.
         */
        RepeatedTexture(ITexture<ColorT>& _texture, const Rectangle& _src_area) : TextureWrapper<ColorT>(_texture), src_area(_src_area) {}

        /**
         * @brief Sets the source area to tile repeatedly.
         * @param _src_area New repeated source area.
         */
        void SetSourceArea(const Rectangle& _src_area) { src_area = _src_area; }
        /**
         * @brief Returns the repeated source area.
         * @return Current source area.
         */
        Rectangle GetSourceArea() const { return src_area; }

        /**
         * @brief Samples the wrapped texture using wrapped coordinates inside the source area.
         * @param x Horizontal texel coordinate.
         * @param y Vertical texel coordinate.
         * @param result Output texel value.
         * @return @c true if the source area is non-empty and the wrapped texture returns a value.
         */
        bool GetColor(int x, int y, ColorT& result) override {
            if (src_area.IsEmptyArea())
                return false;
            int src_x = src_area.LeftSide() + ((x - src_area.LeftSide()) % src_area.Width() + src_area.Width()) % src_area.Width();
            int src_y = src_area.TopSide() + ((y - src_area.TopSide()) % src_area.Height() + src_area.Height()) % src_area.Height();
            return TextureWrapper<ColorT>::GetColor(src_x, src_y, result);
        }
    };

    /**
     * @brief Texture adapter that converts sampled texels from one payload type to another.
     * @tparam ColorTIn Input texel payload type.
     * @tparam ColorTOut Output texel payload type.
     */
    template <typename ColorTIn, typename ColorTOut>
    class ConvertTexture : public ITexture<ColorTOut> {
    private:
        ITexture<ColorTIn>& texture;
        std::function<void(const ColorTIn&, ColorTOut&)> converter;
    public:
        /**
         * @brief Constructs a converting texture wrapper.
         * @param _texture Wrapped input texture.
         * @param _converter Conversion callback from `ColorTIn` to `ColorTOut`.
         */
        ConvertTexture(ITexture<ColorTIn>& _texture, const std::function<void(const ColorTIn&, ColorTOut&)>& _converter)
            : texture(_texture), converter(_converter) {}

        /**
         * @brief Replaces the conversion callback.
         * @param _converter New conversion callback.
         */
        void SetConverter(const std::function<void(const ColorTIn&, ColorTOut&)>& _converter) { converter = _converter; }

        /**
         * @brief Samples the wrapped texture and converts the texel payload.
         * @param x Horizontal texel coordinate.
         * @param y Vertical texel coordinate.
         * @param result Output converted texel value.
         * @return @c true if the wrapped texture returns a value.
         */
        bool GetColor(int x, int y, ColorTOut& result) override {
            ColorTIn res;
            if (!texture.GetColor(x, y, res)) return false;
            converter(res, result);
            return true;
        }
        
        /**
         * @brief Returns the wrapped input texture.
         * @return Mutable reference to the input texture.
         */
        ITexture<ColorTIn>& GetInternalInputTexture() { return texture; }
        /**
         * @brief Returns the wrapped input texture.
         * @return Const reference to the input texture.
         */
        const ITexture<ColorTIn>& GetInternalInputTexture() const { return texture; }
    };
}