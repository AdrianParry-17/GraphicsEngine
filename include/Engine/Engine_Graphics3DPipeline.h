#pragma once

#include "Engine_2D.h"
#include "Engine_3D.h"
#include "Engine_4D.h"
#include "Engine_Geometry.h"
#include "Engine_Graphics.h"
#include "Engine_Interpolation.h"
#include "Engine_Renderer.h"

#include <array>
#include <cmath>
#include <stdexcept>
#include <vector>

namespace Engine {
    /// @brief High-level 3D rendering pipeline with optional depth buffering.
    /// @tparam ColorT External color/payload type.
    template <typename ColorT>
    class Graphics3DPipeline {
    private:
        /// @brief Internal fragment payload type.
        struct Fragment {
            /// @brief Interpolated fragment color/payload.
            ColorT color;
            /// @brief Interpolated depth value used for depth testing.
            double z_depth; // For z-depth test
        };

        // Buffer stuff
        std::vector<double> depth_buffer;

        // Injection stuff
        IGraphics<ColorT> &graphics;
        IInterpolator<ColorT> &color_interpolator;

        // Variables
        FunctionInterpolator<Fragment> frag_interpolator; // For interpolate Fragment3D
        Rectangle viewport;

        // Pipeline stuff

        FunctionGraphics<Fragment> function_g; // For custom pixel-level rendering.
        ClippedGraphics<Fragment> clipped_g;   // For clipping on pixel-level (avoid accidential pixel going outside).
        RendererGraphics<Fragment> renderer_g; // The color renderer
        RendererGraphics2D<Fragment> renderer_g2d; // Adapt from 2D to Graphics (convert x, y to int)
        ViewportTransformGraphics2D<Fragment> viewport_g2d; // Viewport transform (2D).
        PositionProjected2DGraphics3D<Fragment> project2d_g3d; // 3D -> 2D projector
        PositionProjected3DGraphics4D<Fragment> project3d_g4d; // 4D -> 3D projector (W divide should be here)
        PlaneClippedGraphics4D<Fragment> clipped_g4d; // 4D plane clipper (usually for W-clipping)
        MatrixGraphics4D<Fragment> matrix_g4d; // Transformation matrix.
        PositionEmbed4DGraphics3D<Fragment> embed_g3d; // 3D -> 4D embedder (homogenerous here)
        ConvertedGraphics3D<ColorT, Fragment> input_g3d; // Converter from input vertex into to fragment

    private:
        // Private function field

        // -- Drawing --

        // For drawing a single fragment (pixel-wise processing level)
        bool drawFragment(int x, int y, const Fragment &frag) {
            // Depth buffer test (must be enabled and the point is inside the viewport
            // to test, otherwise skip this step)
            if (IsDepthBufferEnabled() && viewport.IsContain(Point(x, y))) {
                int idx = (y - viewport.TopSide()) * viewport.Width() + (x - viewport.LeftSide());
                if (frag.z_depth > depth_buffer[idx])
                    return false; // Further away, skip.

                // Otherwise update depth
                depth_buffer[idx] = frag.z_depth;
            }

            // For now, just sent directly to graphics
            return graphics.DrawPoint(x, y, frag.color);
        }

        // -- Projecting/Embedding --

        void project3dTo2d(const Vertex3D<Fragment> &in, Vertex2D<Fragment> &out) const {
            // Just ignore z
            out.x = in.x;
            out.y = in.y;
            out.color = Fragment{in.color.color, in.z};
        }
        void project4dTo3d(const Vertex4D<Fragment> &in, Vertex3D<Fragment> &out) const {
            // Homogenerous divide

            double w_factor = in.w;
            if (Engine::NumericConstants::IsNearZero(w_factor))
                w_factor = Engine::NumericConstants::NearZero;
            w_factor = 1.0 / w_factor;

            out.x = in.x * w_factor;
            out.y = in.y * w_factor;
            out.z = in.z * w_factor;
            out.color = in.color;
        }
        void embed3dTo4d(const Vertex3D<Fragment> &in, Vertex4D<Fragment> &out) const {
            out.x = in.x;
            out.y = in.y;
            out.z = in.z;
            out.w = 1; // Homogenerous embed
            out.color = in.color;
        }
        void inputConvert3d(const Vertex3D<ColorT> &in, Vertex3D<Fragment> &out) const {
            out.x = in.x;
            out.y = in.y;
            out.z = in.z;
            out.color = Fragment{in.color, in.z};
        }

        // -- Interpolation --

        void frag_linear_interpolate(const Fragment &a, const Fragment &b, double t, Fragment &result) {
            // Color
            color_interpolator.Linear(a.color, b.color, t, result.color);
            // Attributes
            result.z_depth = a.z_depth + (b.z_depth - a.z_depth) * t; // Z-depth
        }
        void frag_triangle_interpolate(const Fragment &a, const Fragment &b, const Fragment &c, double wa, double wb, double wc, Fragment &result) {
            // Color
            color_interpolator.Triangle(a.color, b.color, c.color, wa, wb, wc, result.color);
            // Attributes;
            result.z_depth = a.z_depth * wa + b.z_depth * wb + c.z_depth * wc; // Z-depth
        }

    public:
        /// @brief Constructs a 3D pipeline.
        /// @param _graphics Output pixel graphics target.
        /// @param _color_interpolator Color interpolator.
        /// @param _viewport Output viewport rectangle.
        Graphics3DPipeline(IGraphics<ColorT> &_graphics, IInterpolator<ColorT> &_color_interpolator, const Rectangle &_viewport)
            : graphics(_graphics), color_interpolator(_color_interpolator),
            function_g([this](int x, int y, const Fragment &c) -> bool { return this->drawFragment(x, y, c); }),
            clipped_g(function_g, _viewport), // Disable on default
            renderer_g(function_g, frag_interpolator), renderer_g2d(renderer_g),
            viewport_g2d(
                renderer_g2d, -1, 1, 1, -1,
                _viewport.LeftSide(), _viewport.RightSide(), _viewport.TopSide(), _viewport.BottomSide()
            ),
            project2d_g3d(
                viewport_g2d,
                [this](const Vertex3D<Fragment> &in, Vertex2D<Fragment> &out) { this->project3dTo2d(in, out); }
            ),
            project3d_g4d(
                project2d_g3d,
                [this](const Vertex4D<Fragment> &in, Vertex3D<Fragment> &out) { this->project4dTo3d(in, out); }
            ),
            clipped_g4d(project3d_g4d, frag_interpolator),
            matrix_g4d(clipped_g4d),
            embed_g3d(
                matrix_g4d,
                [this](const Vertex3D<Fragment> &in, Vertex4D<Fragment> &out) { this->embed3dTo4d(in, out); }
            ),
            input_g3d(
                embed_g3d,
                [this](const Vertex3D<ColorT> &in, Vertex3D<Fragment> &out) { this->inputConvert3d(in, out); }
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
            // For this stage, as this is user interface, throw exception is allow
            if (viewport.IsEmptyArea())
                throw std::invalid_argument("Graphics3DPipeline: The viewport area is 0!");

            // 4D Clip initialize
            clipped_g4d.SetClipPlanes({
                Plane4D(-1, 0, 0, 1, 0), // -x + w >= 0
                Plane4D(1, 0, 0, 1, 0),  // x + w >= 0
                Plane4D(0, -1, 0, 1, 0), // -y + w >= 0
                Plane4D(0, 1, 0, 1, 0),  // y + w >= 0
                Plane4D(0, 0, -1, 1, 0), // -z + w >= 0
                Plane4D(0, 0, 1, 1, 0),  // z + w >= 0
            });
        }

        // -- Properties --

        /// @brief Sets output viewport.
        /// @param _viewport New viewport rectangle.
        void SetViewport(const Rectangle &_viewport) {
            if (_viewport.IsEmptyArea())
                throw std::invalid_argument("Graphics3DPipeline: The viewport area is 0!");

            viewport_g2d.SetOutputViewport(_viewport.LeftSide(), _viewport.RightSide(), _viewport.TopSide(), _viewport.BottomSide());
            clipped_g.SetClipped(_viewport);
            viewport = _viewport;

            // Depth buffer
            if (!depth_buffer.empty()) // Depth buffer is enabled
                depth_buffer.assign(viewport.Area(), INFINITY);
        }
        /// @brief Gets current output viewport.
        Rectangle GetViewport() const { return viewport; }

        /// @brief Enables depth buffer allocation and usage.
        void EnableDepthBuffer() {
            if (depth_buffer.empty())
                depth_buffer.assign(viewport.Area(), INFINITY);
        }
        /// @brief Disables depth buffering.
        void DisableDepthBuffer() { depth_buffer.clear(); }

        /// @brief Checks whether depth buffering is enabled.
        bool IsDepthBufferEnabled() const { return !depth_buffer.empty(); }

        /// @brief Clears depth buffer to infinity when enabled.
        void ClearDepthBuffer() {
            if (!depth_buffer.empty())
                depth_buffer.assign(viewport.Area(), INFINITY);
        }

        // -- Matrix stack management --

        /// @brief Loads identity matrix using stack policy.
        void LoadIdentity(bool replace = false) { matrix_g4d.LoadIdentity(replace); }
        /// @brief Pushes matrix stack.
        void PushMatrix() { matrix_g4d.PushMatrix(); }
        /// @brief Pops matrix stack.
        void PopMatrix() { matrix_g4d.PopMatrix(); }

		/// @brief Gets current matrix data as raw array.
		void GetMatrixData(std::array<double, 16>& res) const { matrix_g4d.GetMatrixData(res); }
		/// @brief Gets current matrix data.
		void GetMatrixData(Matrix4x4& res) const { matrix_g4d.GetMatrixData(res); }
		
		/// @brief Replaces current matrix data.
		void SetMatrixData(const std::array<double, 16>& data) { matrix_g4d.SetMatrixData(data); }
		/// @brief Replaces current matrix data.
		void SetMatrixData(const Matrix4x4& data) { matrix_g4d.SetMatrixData(data); }

        /// @brief Right-multiplies current matrix.
        void MultiplyMatrix(const Matrix4x4& mat) { matrix_g4d.MultiplyMatrix(mat); }
        /// @brief Right-multiplies current matrix.
        void MultiplyMatrix(const std::array<double, 16>& mat) { matrix_g4d.MultiplyMatrix(mat); }

        /// @brief Applies perspective projection transform.
        void Perspective(double fovY, double aspect, double near, double far) {
            matrix_g4d.Perspective(fovY, aspect, near, far);
        }
        /// @brief Applies orthographic projection transform.
        void Orthographic(double left, double right, double top, double bottom, double near, double far) {
            matrix_g4d.Orthographic(left, right, top, bottom, near, far);
        }
        /// @brief Applies frustum projection transform.
        void Frustum(double left, double right, double top, double bottom, double near, double far) {
            matrix_g4d.Frustum(left, right, top, bottom, near, far);
        }

        /// @brief Applies 3D translation transform.
        void Translate3D(double x, double y, double z) { matrix_g4d.Translate3D(x, y, z); }
        /// @brief Applies 3D translation transform.
        void Translate3D(const Vector3 &amount) { matrix_g4d.Translate3D(amount); }

        /// @brief Applies 3D scale transform.
        void Scale3D(double x, double y, double z) { matrix_g4d.Scale3D(x, y, z); }
        /// @brief Applies 3D scale transform.
        void Scale3D(const Vector3 &amount) { matrix_g4d.Scale3D(amount); }

        /// @brief Applies 3D rotation transform.
        void Rotate3D(double x, double y, double z) { matrix_g4d.Rotate3D(x, y, z); }
        /// @brief Applies 3D rotation transform.
        void Rotate3D(const Vector3 &amount) { matrix_g4d.Rotate3D(amount); }

        /// @brief Applies combined 3D transformation.
        void Transform3D(const Vector3 &translation, const Vector3 &rotation, const Vector3 &scale) {
            matrix_g4d.Transform3D(translation, rotation, scale);
        }

        // Rendering

        /// @brief Renders one triangle through the pipeline.
        void RenderTriangle(const Vertex3D<ColorT> &a, const Vertex3D<ColorT> &b, const Vertex3D<ColorT> &c) {
            input_g3d.RenderTriangle(a, b, c);
        }
        /// @brief Renders indexed geometry through the pipeline.
        void RenderGeometry(const std::vector<Vertex3D<ColorT>> &vertices, const std::vector<int> &triangles_indices) {
            input_g3d.RenderGeometry(vertices, triangles_indices);
        }
    };
}
