#pragma once

#include "Engine_Texture.h"
#include "Engine_Range.h"

namespace Engine {
    /**
     * @brief Interface for continuous 2D texture sampling.
     * @tparam ColorT Texture sample payload type.
     */
    template <typename ColorT>
    struct ITexture2D {
        /**
         * @brief Virtual destructor.
         */
        virtual ~ITexture2D() = default;

        /**
         * @brief Samples the texture at floating-point coordinates.
         * @param x Horizontal texture coordinate.
         * @param y Vertical texture coordinate.
         * @param result Output sample value.
         * @return @c true if sampling succeeded, otherwise @c false.
         */
        virtual bool GetColor(double x, double y, ColorT& result) = 0;
    };

    /**
     * @brief Continuous texture implementation backed by a callback.
     * @tparam ColorT Texture sample payload type.
     */
    template <typename ColorT>
    class FunctionTexture2D : public ITexture2D<ColorT> {
    private:
        std::function<bool(double, double, ColorT& result)> func;
    public:
        /**
         * @brief Constructs a function-backed continuous texture.
         * @param _func Sampling callback.
         */
        FunctionTexture2D(const std::function<bool(double, double, ColorT&)>& _func) : func(_func) {}
        
        /**
         * @brief Replaces the sampling callback.
         * @param _func New sampling callback.
         */
        void SetFunction(const std::function<bool(double, double, ColorT&)>& _func) { func = _func; }

        /**
         * @brief Samples the texture through the callback.
         * @param x Horizontal texture coordinate.
         * @param y Vertical texture coordinate.
         * @param result Output sample value.
         * @return @c true if the callback exists and sampling succeeded.
         */
        bool GetColor(double x, double y, ColorT& result) override { return (bool)func ? func(x, y, result) : false; }
    };

    /**
     * @brief Base wrapper that forwards continuous texture sampling to another texture.
     * @tparam ColorT Texture sample payload type.
     */
    template <typename ColorT>
    class Texture2DWrapper : public ITexture2D<ColorT> {
    private:
        ITexture2D<ColorT>& texture;
    public:
        /**
         * @brief Constructs a forwarding continuous texture wrapper.
         * @param _texture Wrapped texture.
         */
        Texture2DWrapper(ITexture2D<ColorT>& _texture) : texture(_texture) {}
        
        /**
         * @brief Samples the wrapped texture.
         * @param x Horizontal texture coordinate.
         * @param y Vertical texture coordinate.
         * @param result Output sample value.
         * @return @c true if the wrapped texture returns a value.
         */
        bool GetColor(double x, double y, ColorT& result) override { return texture.GetColor(x, y, result); }

        /**
         * @brief Returns the wrapped texture.
         * @return Mutable reference to the wrapped texture.
         */
        ITexture2D<ColorT>& GetInternalTexture() { return texture; }
        /**
         * @brief Returns the wrapped texture.
         * @return Const reference to the wrapped texture.
         */
        const ITexture2D<ColorT>& GetInternalTexture() const { return texture; }
    };

    /**
     * @brief Sampling method used by `NativeTexture2D`.
     */
    enum class NativeTextureAdaptMethod {
        /**
         * @brief Samples one integer texel without interpolation.
         *
         * Coordinates are truncated when no source area is configured. With
         * source-area edge handling enabled, they are floored before applying
         * the selected edge policy so negative coordinates behave consistently.
         */
        NearestNeighbor = 0,
        /** @brief Sample by bilinearly blending the four surrounding texels. */
        Bilinear = 1
    };

    /**
     * @brief Adapts an integer-coordinate `ITexture` into a floating-point `ITexture2D`.
     * @tparam ColorT Texture sample payload type.
     */
    template <typename ColorT>
    class NativeTexture2D : public ITexture2D<ColorT> {
    private:
        ITexture<ColorT>& texture;
        IInterpolator<ColorT>& interpolator;
        NativeTextureAdaptMethod adapt_method;
        Rectangle source_area;
        bool source_area_enabled = false;
        EdgeHandlingMode edge_mode = EdgeHandlingMode::Clamp;

        bool getTexel(int x, int y, ColorT& result);
        bool nearestNeighbor(double x, double y, ColorT& result);
        bool bilinear(double x, double y, ColorT& result);
    public:
        /**
         * @brief Constructs a native texture adapter.
         * @param _texture Underlying integer-coordinate texture.
         * @param _interpolator Interpolator used for bilinear sampling.
         * @param _adapt_method Initial sampling method.
         */
        NativeTexture2D(ITexture<ColorT>& _texture, IInterpolator<ColorT>& _interpolator,
            NativeTextureAdaptMethod _adapt_method = NativeTextureAdaptMethod::NearestNeighbor)
            : texture(_texture), interpolator(_interpolator), adapt_method(_adapt_method) {}

        /**
         * @brief Constructs a native texture adapter with explicit source edge handling.
         * @param _texture Underlying integer-coordinate texture.
         * @param _interpolator Interpolator used for bilinear sampling.
         * @param _source_area Source texel area used for edge handling.
         * @param _adapt_method Initial sampling method.
         * @param _edge_mode Source edge handling mode.
         */
        NativeTexture2D(ITexture<ColorT>& _texture, IInterpolator<ColorT>& _interpolator,
            const Rectangle& _source_area,
            NativeTextureAdaptMethod _adapt_method = NativeTextureAdaptMethod::NearestNeighbor,
            EdgeHandlingMode _edge_mode = EdgeHandlingMode::Clamp)
            : texture(_texture), interpolator(_interpolator), adapt_method(_adapt_method),
            source_area(_source_area), source_area_enabled(true), edge_mode(_edge_mode) {}
        /**
         * @brief Constructs a native texture adapter with explicit source edge handling
         *        and the default nearest-neighbor adaptation method.
         * @param _texture Underlying integer-coordinate texture.
         * @param _interpolator Interpolator used for bilinear sampling.
         * @param _source_area Source texel area used for edge handling.
         * @param _edge_mode Source edge handling mode.
         */
        NativeTexture2D(ITexture<ColorT>& _texture, IInterpolator<ColorT>& _interpolator,
            const Rectangle& _source_area,
            EdgeHandlingMode _edge_mode)
            : NativeTexture2D(_texture, _interpolator, _source_area, NativeTextureAdaptMethod::NearestNeighbor, _edge_mode) {}

        /**
         * @brief Samples the native texture using the active adaptation method.
         * @param x Horizontal texture coordinate.
         * @param y Vertical texture coordinate.
         * @param result Output sample value.
         * @return @c true if sampling succeeded.
         */
        bool GetColor(double x, double y, ColorT& result) override {
            switch (adapt_method)
            {
            case NativeTextureAdaptMethod::NearestNeighbor:
                return nearestNeighbor(x, y, result);
            case NativeTextureAdaptMethod::Bilinear:
                return bilinear(x, y, result);
            default:
                return false;
            }
        }

        /**
         * @brief Returns the active sampling method.
         * @return Current sampling method.
         */
        NativeTextureAdaptMethod GetCurrentAdaptMethod() const { return adapt_method; }
        /**
         * @brief Sets the active sampling method.
         * @param method New sampling method.
         */
        void SetCurrentAdaptMethod(NativeTextureAdaptMethod method) { adapt_method = method; }

        /**
         * @brief Enables source-area edge handling.
         * @param _source_area Source texel area used for clamp/repeat/reject behavior.
         */
        void SetSourceArea(const Rectangle& _source_area) {
            source_area = _source_area;
            source_area_enabled = true;
        }
        /**
         * @brief Disables source-area edge handling and forwards texel reads directly.
         */
        void ClearSourceArea() { source_area_enabled = false; }
        /**
         * @brief Returns whether source-area edge handling is enabled.
         * @return @c true when source edge handling is active.
         */
        bool IsSourceAreaEnabled() const { return source_area_enabled; }
        /**
         * @brief Returns the configured source area.
         * @return Current source area.
         */
        Rectangle GetSourceArea() const { return source_area; }

        /**
         * @brief Returns the source edge handling mode.
         * @return Current edge handling mode.
         */
        EdgeHandlingMode GetEdgeHandlingMode() const { return edge_mode; }
        /**
         * @brief Sets the source edge handling mode.
         * @param mode New edge handling mode.
         */
        void SetEdgeHandlingMode(EdgeHandlingMode mode) { edge_mode = mode; }

        /**
         * @brief Returns the wrapped integer-coordinate texture.
         * @return Mutable reference to the native texture.
         */
        ITexture<ColorT>& GetNativeTexture() { return texture; }
        /**
         * @brief Returns the wrapped integer-coordinate texture.
         * @return Const reference to the native texture.
         */
        const ITexture<ColorT>& GetNativeTexture() const { return texture; }
    };

    /**
     * @brief Continuous texture wrapper that remaps input coordinates through two range mappers.
     * @tparam ColorT Texture sample payload type.
     */
    template <typename ColorT>
    class ViewportTransformTexture2D : public Texture2DWrapper<ColorT> {
    public:
        /**
         * @brief Horizontal coordinate mapper.
         */
        RangeMapper x_mapper;
        /**
         * @brief Vertical coordinate mapper.
         */
        RangeMapper y_mapper;
        /**
         * @brief Constructs a coordinate-remapping texture wrapper.
         * @param _texture Wrapped continuous texture.
         * @param _x_mapper Horizontal range mapper.
         * @param _y_mapper Vertical range mapper.
         */
        ViewportTransformTexture2D(ITexture2D<ColorT>& _texture, const RangeMapper& _x_mapper, const RangeMapper& _y_mapper)
            : Texture2DWrapper<ColorT>(_texture), x_mapper(_x_mapper), y_mapper(_y_mapper) {}
        /**
         * @brief Samples the wrapped texture after remapping the input coordinates.
         * @param x Horizontal coordinate in source space.
         * @param y Vertical coordinate in source space.
         * @param result Output sample value.
         * @return @c true if the wrapped texture returns a value.
         */
        bool GetColor(double x, double y, ColorT& result) override {
            return Texture2DWrapper<ColorT>::GetColor(x_mapper.Map(x), y_mapper.Map(y), result);
        }
    };
}

// -- NativeTexture2D --

template <typename ColorT>
bool Engine::NativeTexture2D<ColorT>::getTexel(int x, int y, ColorT& result) {
    if (!source_area_enabled)
        return texture.GetColor(x, y, result);

    if (source_area.IsEmptyArea())
        return false;

    int src_x = 0, src_y = 0;
    if (!TextureEdgeHandling::Apply(x, source_area.LeftSide(), source_area.Width(), edge_mode, src_x))
        return false;
    if (!TextureEdgeHandling::Apply(y, source_area.TopSide(), source_area.Height(), edge_mode, src_y))
        return false;

    return texture.GetColor(src_x, src_y, result);
}

template <typename ColorT>
bool Engine::NativeTexture2D<ColorT>::nearestNeighbor(double x, double y, ColorT& result) {
    const int ix = source_area_enabled ? (int)std::floor(x) : (int)x;
    const int iy = source_area_enabled ? (int)std::floor(y) : (int)y;
    return getTexel(ix, iy, result);
}
template <typename ColorT>
bool Engine::NativeTexture2D<ColorT>::bilinear(double x, double y, ColorT& result) {
    const int ix = (int)std::round(x);
    const int iy = (int)std::round(y);
    const bool xNearInteger = NumericConstants::IsNearZero(x - ix);
    const bool yNearInteger = NumericConstants::IsNearZero(y - iy);

    if (xNearInteger && yNearInteger)
        return getTexel(ix, iy, result);

    if (xNearInteger) {
        const int yt = (int)std::floor(y);
        const int yb = yt + 1;

        ColorT c0, c1;
        if (!getTexel(ix, yt, c0) || !getTexel(ix, yb, c1))
            return false;

        interpolator.Linear(c0, c1, y - yt, result);
        return true;
    }

    if (yNearInteger) {
        const int xl = (int)std::floor(x);
        const int xr = xl + 1;

        ColorT c0, c1;
        if (!getTexel(xl, iy, c0) || !getTexel(xr, iy, c1))
            return false;

        interpolator.Linear(c0, c1, x - xl, result);
        return true;
    }

    const int l = (int)std::floor(x);
    const int r = l + 1;
    const int t = (int)std::floor(y);
    const int b = t + 1;

    ColorT c00, c01, c10, c11;
    if (!getTexel(l, t, c00) || !getTexel(r, t, c01) || !getTexel(l, b, c10) || !getTexel(r, b, c11))
        return false;

    const double tx = x - l;
    const double ty = y - t;

    ColorT top, bottom;
    interpolator.Linear(c00, c01, tx, top);
    interpolator.Linear(c10, c11, tx, bottom);
    interpolator.Linear(top, bottom, ty, result);
    return true;
}
