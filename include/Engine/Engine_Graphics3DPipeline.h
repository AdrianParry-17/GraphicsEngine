#pragma once

#include "Engine/Engine_Constant.h"
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
#include <limits>

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
            /// @brief Interpolated inverse w (1/w)
            double inv_w; // For perspective correction

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
        bool perspective_correct = true; // If true will enable perspective correction

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

            // Perspective correct case
            if (IsPerspectiveCorrectionEnabled()) {
                Fragment tmp; // Required ColorT to have default constructor.
                double inv_w_factor = frag.inv_w;
                if (NumericConstants::IsNearZero(inv_w_factor))
                    inv_w_factor = NumericConstants::NearZero;
                inv_w_factor = 1.0 / inv_w_factor;

                frag_interpolator.Scale(frag, inv_w_factor, tmp);

                // Sent the corrected color
                return graphics.DrawPoint(x, y, tmp.color);
            }
            else {
                // Sent directly to graphics, no correction.
                return graphics.DrawPoint(x, y, frag.color);
            }
        }

        // -- Projecting/Embedding --

        void project3dTo2d(const Vertex3D<Fragment> &in, Vertex2D<Fragment> &out) {
            // Just ignore z
            out.x = in.x;
            out.y = in.y;

            out.color = in.color;
            out.color.z_depth = in.z;
        }
        void project4dTo3d(const Vertex4D<Fragment> &in, Vertex3D<Fragment> &out) {
            // Homogenerous divide

            double w_factor = in.w;
            if (Engine::NumericConstants::IsNearZero(w_factor))
                w_factor = Engine::NumericConstants::NearZero;
            w_factor = 1.0 / w_factor;

            out.x = in.x * w_factor;
            out.y = in.y * w_factor;
            out.z = in.z * w_factor;

            // On perspective correction, scale the color by 1/w (before interpolating)
            if (IsPerspectiveCorrectionEnabled()) {
                frag_interpolator.Scale(in.color, w_factor, out.color);
                out.color.inv_w = w_factor;
            }
            else {
                out.color = in.color; // No correction => no need for scaling
                out.color.inv_w = 1; // For later, if somehow correction enable after this, it'll still get the non-corrected result.
                
                // Notice that this trick still broken, when somehow perspective correct was changing while
                // this part happen (e.g. the color_interpolator.Scale change perspective correct status).
            }
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

            // Set inv_w = 1, z_depth = 0 as default
            out.color = Fragment{in.color, 1, 0};
        }

        // -- Interpolation --

        void frag_scale_interpolate(const Fragment& in, double scalar, Fragment& result) {
            // Color
            color_interpolator.Scale(in.color, scalar, result.color);
            
            // Non-scale attribute
            result.z_depth = in.z_depth; // This use for depth test, not an actual attribute
            result.inv_w = in.inv_w; // Inverse W (1/w), this use for later scaling
        }
        void frag_linear_interpolate(const Fragment &a, const Fragment &b, double t, Fragment &result) {
            // Color
            color_interpolator.Linear(a.color, b.color, t, result.color);
            // Attributes
            result.z_depth = a.z_depth + (b.z_depth - a.z_depth) * t; // Z-depth
            result.inv_w = a.inv_w + (b.inv_w - a.inv_w) * t; // Inverse W (1/w)
        }
        void frag_triangle_interpolate(const Fragment &a, const Fragment &b, const Fragment &c, double wa, double wb, double wc, Fragment &result) {
            // Color
            color_interpolator.Triangle(a.color, b.color, c.color, wa, wb, wc, result.color);
            // Attributes;
            result.z_depth = a.z_depth * wa + b.z_depth * wb + c.z_depth * wc; // Z-depth
            result.inv_w = a.inv_w * wa + b.inv_w * wb + c.inv_w * wc; // Inverse W (1/w)
        }

    public:
        /// @brief Constructs a 3D pipeline.
        /// @param _graphics Output pixel graphics target.
        /// @param _color_interpolator Color interpolator.
        /// @param _viewport Output viewport rectangle.
        Graphics3DPipeline(IGraphics<ColorT> &_graphics, IInterpolator<ColorT> &_color_interpolator, const Rectangle &_viewport)
            : graphics(_graphics), color_interpolator(_color_interpolator),
            frag_interpolator(
                [this](const Fragment& in, double scalar, Fragment& result) {
                    return this->frag_scale_interpolate(in, scalar, result);
                },
                [this](const Fragment &a, const Fragment &b, double t, Fragment &result) {
                    return this->frag_linear_interpolate(a, b, t, result);
                },
                [this](const Fragment &a, const Fragment &b, const Fragment &c, double wa, double wb, double wc, Fragment &result) {
                    return this->frag_triangle_interpolate(a, b, c, wa, wb, wc, result);
                }
            ),
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
                depth_buffer.assign(viewport.Area(), std::numeric_limits<double>::infinity());
        }
        /// @brief Gets current output viewport.
        Rectangle GetViewport() const { return viewport; }

        /// @brief Enables depth buffer allocation and usage.
        void EnableDepthBuffer() {
            if (depth_buffer.empty())
                depth_buffer.assign(viewport.Area(), std::numeric_limits<double>::infinity());
        }
        /// @brief Disables depth buffering.
        void DisableDepthBuffer() { depth_buffer.clear(); }

        /// @brief Checks whether depth buffering is enabled.
        bool IsDepthBufferEnabled() const { return !depth_buffer.empty(); }

        // Note, the below 2 function shouldn't be called while "rasterizing" since it could caused problem.
        // They only safe when called between RenderTriangle/RenderGeometry call.
        void EnablePerspectiveCorrection() { perspective_correct = true; }
        void DisablePerspectiveCorrection() { perspective_correct = false; }
        
        bool IsPerspectiveCorrectionEnabled() const { return perspective_correct; }

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
