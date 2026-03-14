#pragma once

#include "Engine_Texture2D.h"
#include "Engine_Vector.h"
#include "Engine_Graphics.h"
#include "Engine_Interpolation.h"

namespace Engine {
    /**
     * @brief Payload that carries both a base color and UV coordinates.
     * @tparam ColorT Base color payload type.
     */
    template <typename ColorT>
    struct TexturedColor {
        /**
         * @brief Base color payload used when no texture is bound or texture sampling fails.
         */
        ColorT color;
        /**
         * @brief Horizontal texture coordinate.
         */
        double uv_x = 0;
        /**
         * @brief Vertical texture coordinate.
         */
        double uv_y = 0;
        
        /**
         * @brief Default constructor.
         */
        TexturedColor() = default;
        /**
         * @brief Constructs a payload with only a base color.
         * @param _color Base color payload.
         */
        TexturedColor(const ColorT& _color) : color(_color) {}
        /**
         * @brief Constructs a payload with only UV coordinates.
         * @param _uv_x Horizontal texture coordinate.
         * @param _uv_y Vertical texture coordinate.
         */
        TexturedColor(double _uv_x, double _uv_y) : uv_x(_uv_x), uv_y(_uv_y) {}
        /**
         * @brief Constructs a payload with both base color and UV coordinates.
         * @param _color Base color payload.
         * @param _uv_x Horizontal texture coordinate.
         * @param _uv_y Vertical texture coordinate.
         */
        TexturedColor(const ColorT& _color, double _uv_x, double _uv_y) : color(_color), uv_x(_uv_x), uv_y(_uv_y) {}
    
        /**
         * @brief Returns the UV coordinates as a `Vector2`.
         * @return UV vector.
         */
        Vector2 GetUV() const { return Vector2(uv_x, uv_y); }
        /**
         * @brief Writes the UV coordinates into an output vector.
         * @param res Output UV vector.
         */
        void GetUV(Vector2& res) const { res.x() = uv_x; res.y() = uv_y; }
    };

    /**
     * @brief Interpolator for `TexturedColor` using a wrapped base-color interpolator.
     * @tparam ColorT Base color payload type.
     */
    template <typename ColorT>
    struct TexturedColorInterpolator : public IInterpolator<TexturedColor<ColorT>> {
    private:
        IInterpolator<ColorT>& color_interpolator;
    public:
        /**
         * @brief Constructs a textured-color interpolator.
         * @param _color_interpolator Interpolator used for the base color payload.
         */
        TexturedColorInterpolator(IInterpolator<ColorT>& _color_interpolator) : color_interpolator(_color_interpolator) {}

        /**
         * @brief Linearly interpolates UV coordinates and base color.
         * @param a First sample.
         * @param b Second sample.
         * @param t Interpolation factor in $[0, 1]$.
         * @param res Output interpolated sample.
         */
        void Linear(const TexturedColor<ColorT>& a, const TexturedColor<ColorT>& b, double t, TexturedColor<ColorT>& res) override {
            res.uv_x = a.uv_x + (b.uv_x - a.uv_x) * t;
            res.uv_y = a.uv_y + (b.uv_y - a.uv_y) * t;
            color_interpolator.Linear(a.color, b.color, t, res.color);
        }
        /**
         * @brief Interpolates UV coordinates and base color using barycentric weights.
         * @param a First sample.
         * @param b Second sample.
         * @param c Third sample.
         * @param wa Weight of @p a.
         * @param wb Weight of @p b.
         * @param wc Weight of @p c.
         * @param res Output interpolated sample.
         */
        void Triangle(const TexturedColor<ColorT>& a, const TexturedColor<ColorT>& b, const TexturedColor<ColorT>& c, double wa, double wb, double wc, TexturedColor<ColorT>& res) override {
            res.uv_x = a.uv_x * wa + b.uv_x * wb + c.uv_x * wc;
            res.uv_y = a.uv_y * wa + b.uv_y * wb + c.uv_y * wc;
            color_interpolator.Triangle(a.color, b.color, c.color, wa, wb, wc, res.color);
        }
    };
    
    /**
     * @brief Scalable interpolator for `TexturedColor` using a wrapped scalable base-color interpolator.
     * @tparam ColorT Base color payload type.
     */
    template <typename ColorT>
    struct TexturedColorScalableInterpolator : public IScalableInterpolator<TexturedColor<ColorT>> {
    private:
        IScalableInterpolator<ColorT>& color_interpolator;
    public:
        /**
         * @brief Constructs a scalable textured-color interpolator.
         * @param _color_interpolator Scalable interpolator used for the base color payload.
         */
        TexturedColorScalableInterpolator(IScalableInterpolator<ColorT>& _color_interpolator) : color_interpolator(_color_interpolator) {}

        /**
         * @brief Scales UV coordinates and base color by a scalar.
         * @param in Input sample.
         * @param scalar Scaling factor.
         * @param res Output scaled sample.
         */
        void Scale(const TexturedColor<ColorT>& in, double scalar, TexturedColor<ColorT>& res) override {
            res.uv_x = in.uv_x * scalar;
            res.uv_y = in.uv_y * scalar;
            color_interpolator.Scale(in.color, scalar, res.color);
        }
        /**
         * @brief Linearly interpolates UV coordinates and base color.
         * @param a First sample.
         * @param b Second sample.
         * @param t Interpolation factor in $[0, 1]$.
         * @param res Output interpolated sample.
         */
        void Linear(const TexturedColor<ColorT>& a, const TexturedColor<ColorT>& b, double t, TexturedColor<ColorT>& res) override {
            res.uv_x = a.uv_x + (b.uv_x - a.uv_x) * t;
            res.uv_y = a.uv_y + (b.uv_y - a.uv_y) * t;
            color_interpolator.Linear(a.color, b.color, t, res.color);
        }
        /**
         * @brief Interpolates UV coordinates and base color using barycentric weights.
         * @param a First sample.
         * @param b Second sample.
         * @param c Third sample.
         * @param wa Weight of @p a.
         * @param wb Weight of @p b.
         * @param wc Weight of @p c.
         * @param res Output interpolated sample.
         */
        void Triangle(const TexturedColor<ColorT>& a, const TexturedColor<ColorT>& b, const TexturedColor<ColorT>& c, double wa, double wb, double wc, TexturedColor<ColorT>& res) override {
            res.uv_x = a.uv_x * wa + b.uv_x * wb + c.uv_x * wc;
            res.uv_y = a.uv_y * wa + b.uv_y * wb + c.uv_y * wc;
            color_interpolator.Triangle(a.color, b.color, c.color, wa, wb, wc, res.color);
        }
    };

    /**
     * @brief Graphics adapter that samples an optional texture using UV coordinates carried by `TexturedColor`.
     * @tparam ColorT Base color payload type.
     */
    template <typename ColorT>
    struct TextureSamplingGraphics : public IGraphics<TexturedColor<ColorT>> {
    private:
        IGraphics<ColorT>& graphics;
        ITexture2D<ColorT>* texture = nullptr;
    public:
        /**
         * @brief Constructs a texture-sampling graphics adapter.
         * @param _graphics Wrapped output graphics target.
         */
        TextureSamplingGraphics(IGraphics<ColorT>& _graphics) : graphics(_graphics) {}

        /**
         * @brief Sets the texture used for sampling.
         * @param _texture Texture to sample, or `nullptr` to disable texturing.
         */
        void SetTargetTexture(ITexture2D<ColorT>* _texture) { texture = _texture; }
        /**
         * @brief Returns the currently bound texture.
         * @return Pointer to the active texture, or `nullptr` if texturing is disabled.
         */
        ITexture2D<ColorT>* GetTargetTexture() const { return texture; }

        /**
         * @brief Draws a point by sampling the active texture using the payload UV coordinates.
         *
         * If no texture is bound, or if texture sampling fails, the embedded base
         * color is drawn instead.
         * @param x Horizontal pixel coordinate.
         * @param y Vertical pixel coordinate.
         * @param color Textured payload containing fallback color and UV coordinates.
         * @return @c true if the wrapped graphics target draws the point successfully.
         */
        bool DrawPoint(int x, int y, const TexturedColor<ColorT>& color) override {
            if (texture) {
                ColorT tex_c;
                if (texture->GetColor(color.uv_x, color.uv_y, tex_c))
                    return graphics.DrawPoint(x, y, tex_c);
            }
            // Not having texture or failed to draw, swap to draw normal color.
            return graphics.DrawPoint(x, y, color.color);
        }
    };

}