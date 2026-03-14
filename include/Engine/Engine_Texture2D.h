#pragma once

#include "Engine_Texture.h"
#include "Engine_Range.h"

namespace Engine {
    template <typename ColorT>
    struct ITexture2D {
        virtual ~ITexture2D() = default;

        virtual bool GetColor(double x, double y, ColorT& result) = 0;
    };

    template <typename ColorT>
    class FunctionTexture2D : public ITexture2D<ColorT> {
    private:
        std::function<bool(double, double, ColorT& result)> func;
    public:
        FunctionTexture2D(const std::function<bool(double, double, ColorT&)>& _func) : func(_func) {}
        
        void SetFunction(const std::function<bool(double, double, ColorT&)>& _func) { func = _func; }

        bool GetColor(double x, double y, ColorT& result) override { return (bool)func ? func(x, y, result) : false; }
    };

    template <typename ColorT>
    class Texture2DWrapper : public ITexture2D<ColorT> {
    private:
        ITexture2D<ColorT>& texture;
    public:
        Texture2DWrapper(ITexture2D<ColorT>& _texture) : texture(_texture) {}
        
        bool GetColor(double x, double y, ColorT& result) override { return texture.GetColor(x, y, result); }

        ITexture2D<ColorT>& GetInternalTexture() { return texture; }
        const ITexture2D<ColorT>& GetInternalTexture() const { return texture; }
    };

    enum class NativeTextureAdaptMethod {
        NearestNeighbor = 0,
        Bilinear = 1
    };

    template <typename ColorT>
    class NativeTexture2D : public ITexture2D<ColorT> {
    private:
        ITexture<ColorT>& texture;
        IInterpolator<ColorT>& interpolator;
        NativeTextureAdaptMethod adapt_method;

        bool __nearest_neighbor(double x, double y, ColorT& result);
        bool __bilinear(double x, double y, ColorT& result);
    public:
        NativeTexture2D(ITexture<ColorT>& _texture, IInterpolator<ColorT>& _interpolator,
            NativeTextureAdaptMethod _adapt_method = NativeTextureAdaptMethod::NearestNeighbor)
            : texture(_texture), interpolator(_interpolator), adapt_method(_adapt_method) {}

        bool GetColor(double x, double y, ColorT& result) override {
            switch (adapt_method)
            {
            case NativeTextureAdaptMethod::NearestNeighbor:
                return __nearest_neighbor(x, y, result);
            case NativeTextureAdaptMethod::Bilinear:
                return __bilinear(x, y, result);
            default:
                return false;
            }
        }

        NativeTextureAdaptMethod GetCurrentAdaptMethod() const { return adapt_method; }
        void SetCurrentAdaptMethod(NativeTextureAdaptMethod method) { adapt_method = method; }

        ITexture<ColorT>& GetNativeTexture() { return texture; }
        const ITexture<ColorT>& GetNativeTexture() const { return texture; }
    };

    template <typename ColorT>
    class ViewportTransformTexture2D : public Texture2DWrapper<ColorT> {
    public:
        RangeMapper x_mapper, y_mapper;
        
        ViewportTransformTexture2D(ITexture2D<ColorT>& _texture, const RangeMapper& _x_mapper, const RangeMapper& _y_mapper)
            : Texture2DWrapper<ColorT>(_texture), x_mapper(_x_mapper), y_mapper(_y_mapper) {}
        
        bool GetColor(double x, double y, ColorT& result) override {
            return Texture2DWrapper<ColorT>::GetColor(x_mapper.Map(x), y_mapper.Map(y), result);
        }
    };
}

// -- NativeTexture2D --

template <typename ColorT>
bool Engine::NativeTexture2D<ColorT>::__nearest_neighbor(double x, double y, ColorT& result) {
    return texture.GetColor((int)std::round(x), (int)std::round(y), result);
}
template <typename ColorT>
bool Engine::NativeTexture2D<ColorT>::__bilinear(double x, double y, ColorT& result) {
    const int ix = (int)std::round(x);
    const int iy = (int)std::round(y);
    const bool xNearInteger = NumericConstants::IsNearZero(x - ix);
    const bool yNearInteger = NumericConstants::IsNearZero(y - iy);

    if (xNearInteger && yNearInteger)
        return texture.GetColor(ix, iy, result);

    if (xNearInteger) {
        const int yt = (int)std::floor(y);
        const int yb = yt + 1;

        ColorT c0, c1;
        if (!texture.GetColor(ix, yt, c0) || !texture.GetColor(ix, yb, c1))
            return false;

        interpolator.Linear(c0, c1, y - yt, result);
        return true;
    }

    if (yNearInteger) {
        const int xl = (int)std::floor(x);
        const int xr = xl + 1;

        ColorT c0, c1;
        if (!texture.GetColor(xl, iy, c0) || !texture.GetColor(xr, iy, c1))
            return false;

        interpolator.Linear(c0, c1, x - xl, result);
        return true;
    }

    const int l = (int)std::floor(x);
    const int r = l + 1;
    const int t = (int)std::floor(y);
    const int b = t + 1;

    ColorT c00, c01, c10, c11;
    if (!texture.GetColor(l, t, c00) || !texture.GetColor(r, t, c01) || !texture.GetColor(l, b, c10) || !texture.GetColor(r, b, c11))
        return false;

    const double tx = x - l;
    const double ty = y - t;

    ColorT top, bottom;
    interpolator.Linear(c00, c01, tx, top);
    interpolator.Linear(c10, c11, tx, bottom);
    interpolator.Linear(top, bottom, ty, result);
    return true;
}