#pragma once

#include "Engine_2D.h"
#include "Engine_3D.h"
#include "Engine_Geometry.h"
#include "Engine_Graphics.h"
#include "Engine_Interpolation.h"
#include "Engine_Renderer.h"
#include "Engine_Constant.h"
#include "Engine_WorldGraphics.h"
#include "Engine_Transform.h"

#include <stdexcept>
#include <vector>

namespace Engine {
    /**
     * @brief High-level 2D rendering pipeline.
     *
     * Manages a multi-stage pipeline: color-convert → homogeneous embed → matrix
     * transform → clip → homogeneous divide → viewport → pixel render.
     * Supports a matrix stack, configurable world camera bounds, and custom
     * per-vertex color/payload interpolation.
     * @tparam ColorT External color/payload type attached to each input vertex.
     */
    template <typename ColorT>
    class Graphics2DPipeline {
    private:
        /// @brief Internal fragment payload type.
        struct Fragment {
            /// @brief Interpolated fragment color/payload.
            ColorT color;
        };

        // Injection stuff
        IGraphics<ColorT> &graphics;
        IInterpolator<ColorT> &color_interpolator;

        // Variables
        FunctionInterpolator<Fragment> frag_interpolator; // For interpolate Fragment
        Rectangle viewport;

        // Pipeline stuff
        FunctionGraphics<Fragment> function_g; // For custom pixel-level rendering.
        ClippedGraphics<Fragment> clipped_g;   // For clipping on pixel-level (avoid pixel going outside)
        RendererGraphics<Fragment> renderer_g; // The color renderer
        RendererGraphics2D<Fragment> renderer_g2d; // Adapt from 2D to Graphics
        ViewportTransformWorldGraphics2D<Fragment> viewport_g2d; // Viewport transform (2D).
        PlaneClipWorldGraphics2D<Fragment> clipped_g2d; // 2D plane clipper
        VertexDimensionConvertWorldGraphics<3, 2, Fragment> project2d_g3d; // 3D -> 2D projector
        MatrixWorldGraphics3D<Fragment> matrix_g3d; // Transformation matrix (using 3D matrix for 2D space transformation).
        VertexDimensionConvertWorldGraphics<2, 3, Fragment> embed3d_g2d; // 2D -> 3D embedder
        VertexColorConvertWorldGraphics<ColorT, Fragment, 2> input_g2d; // Converter from input vertex

    private:
        // Private function field

        // -- Drawing --

        // For drawing a single fragment (pixel-wise processing level)
        bool drawFragment(int x, int y, const Fragment &frag) {
            // No depth buffer in 2D by default, just draw to graphics
            return graphics.DrawPoint(x, y, frag.color);
        }

        // -- Projecting/Embedding --

        void project3dTo2d(const WorldVertex3D<Fragment> &in, WorldVertex2D<Fragment> &out) const {
            double factor = in.position.z();
            if (NumericConstants::IsNearZero(factor))
                factor = NumericConstants::NearZero;
            factor = 1.0 / factor;

            out.position = in.position; out.position *= factor;
            out.color = in.color;
        }

        void embed2dTo3d(const WorldVertex2D<Fragment> &in, WorldVertex3D<Fragment> &out) const {
            out.position = in.position;
            out.position.z() = 1.0; // Homogeneous embed

            out.color = in.color;
        }

        void inputConvert2d(const WorldVertex2D<ColorT> &in, WorldVertex2D<Fragment> &out) const {
            out.position = in.position;
            out.color = Fragment{in.color};
        }

        // -- Interpolation --

        void frag_linear_interpolate(const Fragment &a, const Fragment &b, double t, Fragment &result) {
            color_interpolator.Linear(a.color, b.color, t, result.color);
        }

        void frag_triangle_interpolate(const Fragment &a, const Fragment &b, const Fragment &c, double wa, double wb, double wc, Fragment &result) {
            color_interpolator.Triangle(a.color, b.color, c.color, wa, wb, wc, result.color);
        }

    public:
        /**
         * @brief Constructs a 2D pipeline.
         * @param _graphics            Output pixel graphics target.
         * @param _color_interpolator  Color interpolator for per-vertex payload blending.
         * @param _viewport            Output viewport rectangle.
         * @throws std::invalid_argument if the viewport has zero area.
         */
        Graphics2DPipeline(IGraphics<ColorT> &_graphics, IInterpolator<ColorT> &_color_interpolator, const Rectangle &_viewport)
            : graphics(_graphics), color_interpolator(_color_interpolator),
            function_g([this](int x, int y, const Fragment &c) -> bool { return this->drawFragment(x, y, c); }),
            clipped_g(function_g, _viewport),
            renderer_g(function_g, frag_interpolator), renderer_g2d(renderer_g),
            viewport_g2d(renderer_g2d, {
                Engine::RangeMapper(-1, 1, _viewport.LeftSide(), _viewport.RightSide()),
                Engine::RangeMapper(1, -1, _viewport.TopSide(), _viewport.BottomSide())
            }),
            clipped_g2d(viewport_g2d, frag_interpolator),
            project2d_g3d(
                clipped_g2d,
                [this](const WorldVertex3D<Fragment> &in, WorldVertex2D<Fragment> &out) { this->project3dTo2d(in, out); }
            ),
            matrix_g3d(project2d_g3d),
            embed3d_g2d(
                matrix_g3d,
                [this](const WorldVertex2D<Fragment> &in, WorldVertex3D<Fragment> &out) { this->embed2dTo3d(in, out); }
            ),
            input_g2d(
                embed3d_g2d,
                [this](const WorldVertex2D<ColorT> &in, WorldVertex2D<Fragment> &out) { this->inputConvert2d(in, out); }
            ),
            frag_interpolator(
                [this](const Fragment &a, const Fragment &b, double t, Fragment &result) {
                    return this->frag_linear_interpolate(a, b, t, result);
                },
                [this](const Fragment &a, const Fragment &b, const Fragment &c, double wa, double wb, double wc, Fragment &result) {
                    return this->frag_triangle_interpolate(a, b, c, wa, wb, wc, result);
                }
            ),
            viewport(_viewport)
            
        {
            if (viewport.IsEmptyArea())
                throw std::invalid_argument("Graphics2DPipeline: The viewport area is 0!");

            // Default 2D clip initialized against the -1 to 1 Math world boundary
            clipped_g2d.SetClipPlanes({
                WorldPlane2D( 1,  0, 1),
                WorldPlane2D(-1,  0, 1),
                WorldPlane2D( 0, -1, 1),
                WorldPlane2D( 0,  1, 1)
            });
        }

        // -- Properties --

        /**
         * @brief Sets the output viewport rectangle.
         *
         * Updates the viewport transform mapper ranges and the pixel-level clipper.
         * @param _viewport New viewport rectangle.
         * @throws std::invalid_argument if the viewport has zero area.
         */
        void SetViewport(const Rectangle &_viewport) {
            if (_viewport.IsEmptyArea())
                throw std::invalid_argument("Graphics2DPipeline: The viewport area is 0!");

            // Update viewport mapper output range
            viewport_g2d.GetMapper(0).output = Range(_viewport.LeftSide(), _viewport.RightSide());
            viewport_g2d.GetMapper(1).output = Range(_viewport.TopSide(), _viewport.BottomSide());

            clipped_g.SetClipped(_viewport);
            viewport = _viewport;
        }
        /**
         * @brief Returns the current output viewport rectangle.
         * @return Current viewport.
         */
        Rectangle GetViewport() const { return viewport; }

        /**
         * @brief Configures the world-space camera bounds from height and aspect ratio.
         *
         * Adjusts the viewport transform input range and the 2D clip planes so
         * that the world-space region seen by the camera maps to the full viewport.
         * @param camera_height Camera height in world units (must be positive and non-zero).
         * @param camera_aspect Camera aspect ratio (width / height, must be positive and non-zero).
         * @throws std::invalid_argument if either value is near zero or negative.
         */
        void SetWorldCamera(double camera_height, double camera_aspect) {
            if (NumericConstants::IsNearZeroOrNegative(camera_height) || NumericConstants::IsNearZeroOrNegative(camera_aspect))
                throw std::invalid_argument("Graphics2DPipeline.SetWorldCamera: Camera height and aspect must be positive and not near zero!");

            double width = camera_aspect * camera_height, height = camera_height;
            
            // Update viewport mapper input range
            viewport_g2d.GetMapper(0).input = Range(-0.5 * width, 0.5 * width);
            viewport_g2d.GetMapper(1).input = Range(-0.5 * height, 0.5 * height);

            clipped_g2d.SetClipPlanes({
                WorldPlane2D( 1,  0, 0.5 * width),
                WorldPlane2D(-1,  0, 0.5 * width),
                WorldPlane2D( 0, -1, 0.5 * height),
                WorldPlane2D( 0,  1, 0.5 * height)
            });
        }

        // -- Matrix stack management --

        /**
         * @brief Loads the identity matrix into the current stack slot.
         * @param replace If @c true the current top is overwritten; if @c false
         *                the identity is multiplied in (semantics depend on the
         *                underlying MatrixWorldGraphics implementation).
         */
        void LoadIdentity(bool replace = false) { matrix_g3d.LoadIdentity(replace); }
        /**
         * @brief Pushes the current transformation matrix onto the stack.
         */
        void PushMatrix() { matrix_g3d.PushMatrix(); }
        /**
         * @brief Pops the top matrix from the stack, restoring the previous one.
         */
        void PopMatrix() { matrix_g3d.PopMatrix(); }

        /**
         * @brief Retrieves the current transformation matrix as a raw 9-element array.
         * @param res Output array (row-major 3×3).
         */
        void GetMatrixData(std::array<double, 9>& res) const { matrix_g3d.GetMatrixData(res); }
        /**
         * @brief Retrieves the current transformation matrix.
         * @param res Output Matrix3x3.
         */
        void GetMatrixData(Matrix3x3& res) const { matrix_g3d.GetMatrixData(res); }

        /**
         * @brief Replaces the current transformation matrix with a raw array.
         * @param data New matrix data (row-major 3×3).
         */
        void SetMatrixData(const std::array<double, 9>& data) { matrix_g3d.SetMatrixData(data); }
        /**
         * @brief Replaces the current transformation matrix.
         * @param data New Matrix3x3.
         */
        void SetMatrixData(const Matrix3x3& data) { matrix_g3d.SetMatrixData(data); }

        /**
         * @brief Right-multiplies the current matrix by @p mat.
         * @param mat Matrix to multiply.
         */
        void MultiplyMatrix(const Matrix3x3& mat) { matrix_g3d.MultiplyMatrix(mat); }
        /**
         * @brief Right-multiplies the current matrix by a raw array.
         * @param mat Matrix data (row-major 3×3).
         */
        void MultiplyMatrix(const std::array<double, 9>& mat) { matrix_g3d.MultiplyMatrix(mat); }

        /**
         * @brief Right-multiplies a translation matrix.
         * @param x Translation along X. @param y Along Y.
         */
        void Translate(double x, double y) {
            std::array<double, 9> _tmp; Transform2D::GetTranslation(_tmp, x, y); MultiplyMatrix(_tmp);
        }
        /**
         * @brief Right-multiplies a translation matrix from a vector.
         * @param amount Translation vector.
         */
        void Translate(const Vector2 &amount) { Translate(amount.x(), amount.y()); }

        /**
         * @brief Right-multiplies a scale matrix.
         * @param x Scale along X. @param y Along Y.
         */
        void Scale(double x, double y) {
            std::array<double, 9> _tmp; Transform2D::GetScale(_tmp, x, y); MultiplyMatrix(_tmp);
        }
        /**
         * @brief Right-multiplies a scale matrix from a vector.
         * @param amount Scale vector.
         */
        void Scale(const Vector2 &amount) { Scale(amount.x(), amount.y()); }

        /**
         * @brief Right-multiplies a 2D rotation matrix.
         * @param angle Rotation angle in radians.
         */
        void Rotate2D(double angle) {
            std::array<double, 9> _tmp; Transform2D::GetRotation(_tmp, angle); MultiplyMatrix(_tmp);
        }

        /**
         * @brief Right-multiplies a combined 2D TRS (translate × rotate × scale) matrix.
         * @param translation Translation vector.
         * @param rotation    Rotation angle in radians.
         * @param scale       Scale vector.
         */
        void Transform2D(const Vector2 &translation, double rotation, const Vector2 &scale) {
            std::array<double, 9> _tmp; ::Engine::Transform2D::GetTransformation(_tmp, translation, rotation, scale); MultiplyMatrix(_tmp);
        }

        // Rendering

        /**
         * @brief Renders a single triangle through the full 2D pipeline.
         * @param a First vertex.
         * @param b Second vertex.
         * @param c Third vertex.
         */
        void RenderTriangle(const WorldVertex2D<ColorT> &a, const WorldVertex2D<ColorT> &b, const WorldVertex2D<ColorT> &c) {
            input_g2d.RenderTriangle(a, b, c);
        }
        /**
         * @brief Renders indexed triangle geometry through the full 2D pipeline.
         * @param vertices          Vertex array.
         * @param triangles_indices Index list; every three consecutive indices form one triangle.
         */
        void RenderGeometry(const std::vector<WorldVertex2D<ColorT>> &vertices, const std::vector<int> &triangles_indices) {
            input_g2d.RenderGeometry(vertices, triangles_indices);
        }
    };
} // namespace Engine
