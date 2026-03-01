#pragma once

#include "Engine_2D.h"
#include "Engine_3D.h"
#include "Engine_Geometry.h"
#include "Engine_Graphics.h"
#include "Engine_Interpolation.h"
#include "Engine_Renderer.h"
#include "Engine_Constant.h"

#include <stdexcept>
#include <vector>

namespace Engine {
    /// @brief High-level 2D rendering pipeline.
    /// @tparam ColorT External color/payload type.
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
        ViewportTransformGraphics2D<Fragment> viewport_g2d; // Viewport transform (2D).
        PlaneClippedGraphics2D<Fragment> clipped_g2d; // 2D plane clipper
        PositionProjected2DGraphics3D<Fragment> project2d_g3d; // 3D -> 2D projector
        MatrixGraphics3D<Fragment> matrix_g3d; // Transformation matrix (using 3D matrix for 2D space transformation).
        PositionEmbed3DGraphics2D<Fragment> embed3d_g2d; // 2D -> 3D embedder
        ConvertedGraphics2D<ColorT, Fragment> input_g2d; // Converter from input vertex

    private:
        // Private function field

        // -- Drawing --

        // For drawing a single fragment (pixel-wise processing level)
        bool drawFragment(int x, int y, const Fragment &frag) {
            // No depth buffer in 2D by default, just draw to graphics
            return graphics.DrawPoint(x, y, frag.color);
        }

        // -- Projecting/Embedding --

        void project3dTo2d(const Vertex3D<Fragment> &in, Vertex2D<Fragment> &out) const {
            out.x = in.x / in.z;
            out.y = in.y / in.z; // Homogeneous divide
            out.color = in.color;
        }

        void embed2dTo3d(const Vertex2D<Fragment> &in, Vertex3D<Fragment> &out) const {
            out.x = in.x;
            out.y = in.y;
            out.z = 1.0; // Homogeneous embed
            out.color = in.color;
        }

        void inputConvert2d(const Vertex2D<ColorT> &in, Vertex2D<Fragment> &out) const {
            out.x = in.x;
            out.y = in.y;
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
        /// @brief Constructs a 2D pipeline.
        /// @param _graphics Output pixel graphics target.
        /// @param _color_interpolator Color interpolator.
        /// @param _viewport Output viewport rectangle.
        Graphics2DPipeline(IGraphics<ColorT> &_graphics, IInterpolator<ColorT> &_color_interpolator, const Rectangle &_viewport)
            : graphics(_graphics), color_interpolator(_color_interpolator),
            function_g([this](int x, int y, const Fragment &c) -> bool { return this->drawFragment(x, y, c); }),
            clipped_g(function_g, _viewport),
            renderer_g(function_g, frag_interpolator), renderer_g2d(renderer_g),
            viewport_g2d(
                renderer_g2d, -1, 1, 1, -1,
                _viewport.LeftSide(), _viewport.RightSide(), _viewport.TopSide(), _viewport.BottomSide()
            ),
            clipped_g2d(viewport_g2d, frag_interpolator),
            project2d_g3d(
                clipped_g2d,
                [this](const Vertex3D<Fragment> &in, Vertex2D<Fragment> &out) { this->project3dTo2d(in, out); }
            ),
            matrix_g3d(project2d_g3d),
            embed3d_g2d(
                matrix_g3d,
                [this](const Vertex2D<Fragment> &in, Vertex3D<Fragment> &out) { this->embed2dTo3d(in, out); }
            ),
            input_g2d(
                embed3d_g2d,
                [this](const Vertex2D<ColorT> &in, Vertex2D<Fragment> &out) { this->inputConvert2d(in, out); }
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
                Plane2D(1, 0, 1),
                Plane2D(-1, 0, 1),
                Plane2D(0, -1, 1),
                Plane2D(0, 1, 1)
            });
        }

        // -- Properties --

        /// @brief Sets output viewport.
        /// @param _viewport New viewport rectangle.
        void SetViewport(const Rectangle &_viewport) {
            if (viewport.IsEmptyArea())
                throw std::invalid_argument("Graphics2DPipeline: The viewport area is 0!");

            viewport_g2d.SetOutputViewport(_viewport.LeftSide(), _viewport.RightSide(), _viewport.TopSide(), _viewport.BottomSide());
            clipped_g.SetClipped(_viewport);
            viewport = _viewport;
        }
        /// @brief Gets current output viewport.
        Rectangle GetViewport() const { return viewport; }

        /// @brief Configures world camera bounds from height and aspect ratio.
        /// @param camera_height Camera height in world units.
        /// @param camera_aspect Camera aspect ratio (width/height).
        void SetWorldCamera(double camera_height, double camera_aspect) {
            if (NumericConstants::IsNearZeroOrNegative(camera_height) || NumericConstants::IsNearZeroOrNegative(camera_aspect))
                throw std::invalid_argument("Graphics2DPipeline.SetWorldCamera: Camera height and aspect must be positive and not near zero!");

            double width = camera_aspect * camera_height, height = camera_height;
            viewport_g2d.SetInputViewport(
                -0.5 * width, 0.5 * width, 0.5 * height, -0.5 * height
            );
            clipped_g2d.SetClipPlanes({
                Plane2D(1, 0, 0.5 * width),
                Plane2D(-1, 0, 0.5 * width),
                Plane2D(0, -1, 0.5 * height),
                Plane2D(0, 1, 0.5 * height)
            });
        }

        // -- Matrix stack management --

        /// @brief Loads identity matrix using stack policy.
        void LoadIdentity(bool replace = false) { matrix_g3d.LoadIdentity(replace); }
        /// @brief Pushes matrix stack.
        void PushMatrix() { matrix_g3d.PushMatrix(); }
        /// @brief Pops matrix stack.
        void PopMatrix() { matrix_g3d.PopMatrix(); }

		/// @brief Gets current matrix data as raw array.
		void GetMatrixData(std::array<double, 9>& res) const { matrix_g3d.GetMatrixData(res); }
		/// @brief Gets current matrix data.
		void GetMatrixData(Matrix3x3& res) const { matrix_g3d.GetMatrixData(res); }
		
		/// @brief Replaces current matrix data.
		void SetMatrixData(const std::array<double, 9>& data) { matrix_g3d.SetMatrixData(data); }
		/// @brief Replaces current matrix data.
		void SetMatrixData(const Matrix3x3& data) { matrix_g3d.SetMatrixData(data); }

        /// @brief Right-multiplies current matrix.
        void MultiplyMatrix(const Matrix3x3& mat) { matrix_g3d.MultiplyMatrix(mat); }
        /// @brief Right-multiplies current matrix.
        void MultiplyMatrix(const std::array<double, 9>& mat) { matrix_g3d.MultiplyMatrix(mat); }

        /// @brief Applies 2D translation transform.
        void Translate2D(double x, double y) { matrix_g3d.Translate2D(x, y); }
        /// @brief Applies 2D translation transform.
        void Translate2D(const Vector2 &amount) { matrix_g3d.Translate2D(amount); }

        /// @brief Applies 2D scale transform.
        void Scale2D(double x, double y) { matrix_g3d.Scale2D(x, y); }
        /// @brief Applies 2D scale transform.
        void Scale2D(const Vector2 &amount) { matrix_g3d.Scale2D(amount); }

        /// @brief Applies 2D rotation transform.
        void Rotate2D(double angle) { matrix_g3d.Rotate2D(angle); }

        /// @brief Applies combined 2D transform.
        void Transform2D(const Vector2 &translation, double rotation, const Vector2 &scale) {
            matrix_g3d.Transform2D(translation, rotation, scale);
        }

        // Rendering

        /// @brief Renders one triangle through the pipeline.
        void RenderTriangle(const Vertex2D<ColorT> &a, const Vertex2D<ColorT> &b, const Vertex2D<ColorT> &c) {
            input_g2d.RenderTriangle(a, b, c);
        }
        /// @brief Renders indexed geometry through the pipeline.
        void RenderGeometry(const std::vector<Vertex2D<ColorT>> &vertices, const std::vector<int> &triangles_indices) {
            input_g2d.RenderGeometry(vertices, triangles_indices);
        }
    };
} // namespace Engine
