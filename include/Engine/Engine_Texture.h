#pragma once

#include "Engine_Context.h"
#include "Engine_Geometry.h"
#include "Engine_Interpolation.h"
#include "Engine_Constant.h"

#include <cmath>
#include <functional>

namespace Engine {
    template <typename ColorT>
    struct ITexture {
        virtual ~ITexture() = default;

        virtual bool GetColor(int x, int y, ColorT& result) = 0;
    };

    template <typename ColorT>
    class ContextTexture : public ITexture<ColorT> {
    private:
        IDrawingContext<ColorT>& context;
    public:
        ContextTexture(IDrawingContext<ColorT>& _context) : context(_context) {}

        bool GetColor(int x, int y, ColorT& result) override { return context.GetPixel(x, y, result); }

        IDrawingContext<ColorT>& GetContext() { return context; }
        const IDrawingContext<ColorT>& GetContext() const { return context; }
    };

    template <typename ColorT>
    class FunctionTexture : public ITexture<ColorT> {
    private:
        std::function<bool(int, int, ColorT&)> func;
    public:
        FunctionTexture(const std::function<bool(int, int, ColorT&)>& _func) : func(_func) {}

        void SetFunction(const std::function<bool(int, int, ColorT&)>& _func) { func = _func; }

        bool GetColor(int x, int y, ColorT& result) override { return (bool)func ? func(x, y, result) : false; }
    };

    template <typename ColorT>
    class TextureWrapper : public ITexture<ColorT> {
    private:
        ITexture<ColorT>& texture;
    public:
        TextureWrapper(ITexture<ColorT>& _texture) : texture(_texture) {}
        
        bool GetColor(int x, int y, ColorT& result) override { return texture.GetColor(x, y, result); }

        ITexture<ColorT>& GetInternalTexture() { return texture; }
        const ITexture<ColorT>& GetInternalTexture() const { return texture; }
    };

    template <typename ColorT>
    class ViewportTexture : public TextureWrapper<ColorT> {
    private:
        Rectangle viewport;
    public:
        ViewportTexture(ITexture<ColorT>& _texture, const Rectangle& _viewport) : TextureWrapper<ColorT>(_texture), viewport(_viewport) {}

        Rectangle GetViewport() const { return viewport; }
        void SetViewport(const Rectangle& _viewport) { viewport = _viewport; }

        bool GetColor(int x, int y, ColorT& result) override  {
            if (x < 0 || x >= viewport.Width() || y < 0 || y >= viewport.Height())
                return false;
            return TextureWrapper<ColorT>::GetColor(x + viewport.LeftSide(), y + viewport.RightSide(), result);
        }
    };

    template <typename ColorT>
    class ClippedTexture : public TextureWrapper<ColorT> {
    private:
        Rectangle clipped;
    public:
        ClippedTexture(ITexture<ColorT>& _texture, const Rectangle& _clipped) : TextureWrapper<ColorT>(_texture), clipped(_clipped) {}

        Rectangle GetClipArea() const { return clipped; }
        void SetClipArea(const Rectangle& _clipped) { clipped = _clipped; }

        bool GetColor(int x, int y, ColorT& result) override {
            return clipped.IsContain(x, y) ? TextureWrapper<ColorT>::GetColor(x, y, result) : false;
        }
    };

    template <typename ColorTIn, typename ColorTOut>
    class ConvertTexture : public ITexture<ColorTIn> {
    private:
        ITexture<ColorTOut>& texture;
        std::function<void(const ColorTIn&, ColorTOut&)> converter;
    public:
        ConvertTexture(ITexture<ColorTOut>& _texture, const std::function<void(const ColorTIn&, ColorTOut&)>& _converter)
            : texture(_texture), converter(_converter) {}

        void SetConverter(const std::function<void(const ColorTIn&, ColorTOut&)>& _converter) { converter = _converter; }

        bool GetColor(double x, double y, ColorTOut& result) override {
            ColorTIn res;
            if (!texture.GetColor(x, y, res)) return false;
            converter(res, result);
            return true;
        }
        
        ITexture<ColorTOut>& GetInternalOutputTexture() { return texture; }
        const ITexture<ColorTOut>& GetInternalOutputTexture() const { return texture; }
    };
}