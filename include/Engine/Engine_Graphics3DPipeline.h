#pragma once

#include "Engine_Constant.h"
#include "Engine_2D.h"
#include "Engine_3D.h"
#include "Engine_4D.h"
#include "Engine_WorldGraphics.h"
#include "Engine_Geometry.h"
#include "Engine_Graphics.h"
#include "Engine_Interpolation.h"
#include "Engine_Renderer.h"
#include "Engine_Transform.h"

#include <array>
#include <cmath>
#include <stdexcept>
#include <vector>
#include <limits>

namespace Engine {
    /**
     * @brief High-level 3D rendering pipeline with optional depth buffering.
     *
     * Manages a multi-stage pipeline: color-convert → homogeneous embed → matrix
     * transform → W-clip → W-divide → 2D project → viewport → pixel render.
     * Supports a matrix stack, optional depth buffering, and perspective-correct
     * interpolation of per-vertex color/payload data.
     * @tparam ColorT External color/payload type attached to each input vertex.
     */
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
        IScalableInterpolator<ColorT> &color_interpolator;

        // Variables
        FunctionScalableInterpolator<Fragment> frag_interpolator; // For interpolate Fragment
        Rectangle viewport;
        bool perspective_correct = true; // If true will enable perspective correction

        // Pipeline stuff

        FunctionGraphics<Fragment> function_g; // For custom pixel-level rendering.
        ClippedGraphics<Fragment> clipped_g;   // For clipping on pixel-level (avoid accidential pixel going outside).
        RendererGraphics<Fragment> renderer_g; // The color renderer
        RendererGraphics2D<Fragment> renderer_g2d; // Adapt from 2D to Graphics (convert x, y to int)
        ViewportTransformWorldGraphics2D<Fragment> viewport_g2d; // Viewport transform (2D).
        VertexDimensionConvertWorldGraphics<3, 2, Fragment> project2d_g3d; // 3D -> 2D projector
        VertexDimensionConvertWorldGraphics<4, 3, Fragment> project3d_g4d; // 4D -> 3D projector (W divide should be here)
        PlaneClipWorldGraphics4D<Fragment> clipped_g4d; // 4D plane clipper (usually for W-clipping)
        MatrixWorldGraphics4D<Fragment> matrix_g4d; // Transformation matrix.
        VertexDimensionConvertWorldGraphics<3, 4, Fragment> embed4d_g3d; // 3D -> 4D embedder (homogenerous here)
        VertexColorConvertWorldGraphics<ColorT, Fragment, 3> input_g3d; // Converter from input vertex into to fragment

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

        void project3dTo2d(const WorldVertex3D<Fragment> &in, WorldVertex2D<Fragment> &out) {
            // Just ignore z
            out.position = in.position;

            out.color = in.color;
            out.color.z_depth = in.position.z();
        }
        void project4dTo3d(const WorldVertex4D<Fragment> &in, WorldVertex3D<Fragment> &out) {
            // Homogenerous divide

            double w_factor = in.position.w();
            if (Engine::NumericConstants::IsNearZero(w_factor))
                w_factor = Engine::NumericConstants::NearZero;
            w_factor = 1.0 / w_factor;

            out.position = in.position; out.position *= w_factor;

            // On perspective correction, scale the color by 1/w (before interpolating)
            if (IsPerspectiveCorrectionEnabled()) {
                frag_interpolator.Scale(in.color, w_factor, out.color);
                out.color.inv_w = w_factor;
            }
            else {
                out.color = in.color; // No correction => no need for scaling
                out.color.inv_w = 1;  // For later, if somehow correction enable after this, it'll still get the non-corrected result.
                
                // Notice that this trick still broken, when somehow perspective correct was changing while
                // this part happen (e.g. the color_interpolator.Scale/Linear/Triangle, graphics.DrawPoint change perspective correct status).
            }
        }
        void embed3dTo4d(const WorldVertex3D<Fragment> &in, WorldVertex4D<Fragment> &out) const {
            out.position = in.position;
            out.position.w() = 1; // Homogenerous embed

            out.color = in.color;
        }
        void inputConvert3d(const WorldVertex3D<ColorT> &in, WorldVertex3D<Fragment> &out) const {
            out.position = in.position;

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
        /**
         * @brief Constructs a 3D pipeline.
         * @param _graphics            Output pixel graphics target.
         * @param _color_interpolator  Scalable color interpolator for per-vertex payload blending
         *                             (also used for perspective-correct scaling).
         * @param _viewport            Output viewport rectangle.
         * @throws std::invalid_argument if the viewport has zero area.
         */
        Graphics3DPipeline(IGraphics<ColorT> &_graphics, IScalableInterpolator<ColorT> &_color_interpolator, const Rectangle &_viewport)
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
            viewport_g2d(renderer_g2d, {
                Engine::RangeMapper(-1, 1, _viewport.LeftSide(), _viewport.RightSide()),
                Engine::RangeMapper(1, -1, _viewport.TopSide(), _viewport.BottomSide())
            }),
            project2d_g3d(
                viewport_g2d,
                [this](const WorldVertex3D<Fragment> &in, WorldVertex2D<Fragment> &out) { this->project3dTo2d(in, out); }
            ),
            project3d_g4d(
                project2d_g3d,
                [this](const WorldVertex4D<Fragment> &in, WorldVertex3D<Fragment> &out) { this->project4dTo3d(in, out); }
            ),
            clipped_g4d(project3d_g4d, frag_interpolator),
            matrix_g4d(clipped_g4d),
            embed4d_g3d(
                matrix_g4d,
                [this](const WorldVertex3D<Fragment> &in, WorldVertex4D<Fragment> &out) { this->embed3dTo4d(in, out); }
            ),
            input_g3d(
                embed4d_g3d,
                [this](const WorldVertex3D<ColorT> &in, WorldVertex3D<Fragment> &out) { this->inputConvert3d(in, out); }
            ),
            viewport(_viewport) 
        {
            // For this stage, as this is user interface, throw exception is allow
            if (viewport.IsEmptyArea())
                throw std::invalid_argument("Graphics3DPipeline: The viewport area is 0!");

            // 4D Clip initialize
            clipped_g4d.SetClipPlanes({
                WorldPlane4D(-1,  0,  0, 1, 0), // -x + w >= 0
                WorldPlane4D( 1,  0,  0, 1, 0),  // x + w >= 0
                WorldPlane4D( 0, -1,  0, 1, 0), // -y + w >= 0
                WorldPlane4D( 0,  1,  0, 1, 0),  // y + w >= 0
                WorldPlane4D( 0,  0, -1, 1, 0), // -z + w >= 0
                WorldPlane4D( 0,  0,  1, 1, 0),  // z + w >= 0
            });
        }

        // -- Properties --

        /**
         * @brief Sets the output viewport rectangle.
         *
         * Updates the viewport transform mapper ranges, the pixel-level clipper,
         * and (if enabled) reallocates the depth buffer to match the new area.
         * @param _viewport New viewport rectangle.
         * @throws std::invalid_argument if the viewport has zero area.
         */
        void SetViewport(const Rectangle &_viewport) {
            if (_viewport.IsEmptyArea())
                throw std::invalid_argument("Graphics3DPipeline: The viewport area is 0!");

            // Update viewport mapper output range
            viewport_g2d.GetMapper(0).output = Range(_viewport.LeftSide(), _viewport.RightSide());
            viewport_g2d.GetMapper(1).output = Range(_viewport.TopSide(), _viewport.BottomSide());

            clipped_g.SetClipped(_viewport);
            viewport = _viewport;

            // Depth buffer
            if (!depth_buffer.empty()) // Depth buffer is enabled
                depth_buffer.assign(viewport.Area(), std::numeric_limits<double>::infinity());
        }
        /**
         * @brief Returns the current output viewport rectangle.
         * @return Current viewport.
         */
        Rectangle GetViewport() const { return viewport; }

        /**
         * @brief Enables the depth buffer, allocating storage for the current viewport.
         *
         * Has no effect if the depth buffer is already enabled.
         */
        void EnableDepthBuffer() {
            if (depth_buffer.empty())
                depth_buffer.assign(viewport.Area(), std::numeric_limits<double>::infinity());
        }
        /**
         * @brief Disables depth buffering and releases the buffer memory.
         */
        void DisableDepthBuffer() { depth_buffer.clear(); }

        /**
         * @brief Returns whether depth buffering is currently active.
         * @return @c true if the depth buffer is allocated and in use.
         */
        bool IsDepthBufferEnabled() const { return !depth_buffer.empty(); }

        // Note: the below 3 functions should not be called while rasterizing since
        // that could cause incorrect results. They are only safe to call between
        // RenderTriangle / RenderGeometry calls.

        /**
         * @brief Enables perspective-correct interpolation of fragment attributes.
         *
         * When enabled, per-vertex data is scaled by 1/w before interpolation and
         * unscaled during fragment shading to produce perspective-correct results.
         */
        void EnablePerspectiveCorrection() { perspective_correct = true; }
        /**
         * @brief Disables perspective-correct interpolation.
         *
         * Fragment attributes are interpolated linearly in screen space.
         */
        void DisablePerspectiveCorrection() { perspective_correct = false; }
        /**
         * @brief Returns whether perspective-correct interpolation is active.
         * @return @c true if perspective correction is enabled.
         */
        bool IsPerspectiveCorrectionEnabled() const { return perspective_correct; }

        /**
         * @brief Resets all depth buffer values to positive infinity.
         *
         * Call this before rendering a new frame to discard depth data from
         * the previous frame. Has no effect when the depth buffer is disabled.
         */
        void ClearDepthBuffer() {
            if (!depth_buffer.empty())
                depth_buffer.assign(viewport.Area(), INFINITY);
        }

        // -- Matrix stack management --

        /**
         * @brief Loads the identity matrix into the current stack slot.
         * @param replace If @c true the current top is overwritten; if @c false
         *                the identity is multiplied in (stack-push semantics depend
         *                on the underlying MatrixWorldGraphics implementation).
         */
        void LoadIdentity(bool replace = false) { matrix_g4d.LoadIdentity(replace); }
        /**
         * @brief Pushes the current transformation matrix onto the stack.
         */
        void PushMatrix() { matrix_g4d.PushMatrix(); }
        /**
         * @brief Pops the top matrix from the stack, restoring the previous one.
         */
        void PopMatrix() { matrix_g4d.PopMatrix(); }

        /**
         * @brief Retrieves the current transformation matrix as a raw 16-element array.
         * @param res Output array (row-major 4×4).
         */
        void GetMatrixData(std::array<double, 16>& res) const { matrix_g4d.GetMatrixData(res); }
        /**
         * @brief Retrieves the current transformation matrix.
         * @param res Output Matrix4x4.
         */
        void GetMatrixData(Matrix4x4& res) const { matrix_g4d.GetMatrixData(res); }

        /**
         * @brief Replaces the current transformation matrix with a raw array.
         * @param data New matrix data (row-major 4×4).
         */
        void SetMatrixData(const std::array<double, 16>& data) { matrix_g4d.SetMatrixData(data); }
        /**
         * @brief Replaces the current transformation matrix.
         * @param data New Matrix4x4.
         */
        void SetMatrixData(const Matrix4x4& data) { matrix_g4d.SetMatrixData(data); }

        /**
         * @brief Right-multiplies the current matrix by @p mat.
         * @param mat Matrix to multiply.
         */
        void MultiplyMatrix(const Matrix4x4& mat) { matrix_g4d.MultiplyMatrix(mat); }
        /**
         * @brief Right-multiplies the current matrix by a raw array.
         * @param mat Matrix data (row-major 4×4).
         */
        void MultiplyMatrix(const std::array<double, 16>& mat) { matrix_g4d.MultiplyMatrix(mat); }

        /**
         * @brief Right-multiplies a symmetric perspective projection matrix.
         * @param fovY   Vertical field of view in radians.
         * @param aspect Viewport aspect ratio (width / height).
         * @param near   Near clipping distance.
         * @param far    Far clipping distance.
         */
        void Perspective(double fovY, double aspect, double near, double far) {
            std::array<double, 16> _tmp; Transform3D::GetPerspectiveProjection(_tmp, fovY, aspect, near, far); MultiplyMatrix(_tmp);
        }
        /**
         * @brief Right-multiplies an orthographic projection matrix.
         * @param left   Left clipping plane.
         * @param right  Right clipping plane.
         * @param top    Top clipping plane.
         * @param bottom Bottom clipping plane.
         * @param near   Near clipping distance.
         * @param far    Far clipping distance.
         */
        void Orthographic(double left, double right, double top, double bottom, double near, double far) {
            std::array<double, 16> _tmp; Transform3D::GetOrthographicProjection(_tmp, left, right, top, bottom, near, far); MultiplyMatrix(_tmp);
        }
        /**
         * @brief Right-multiplies an off-axis frustum projection matrix.
         * @param left   Left clipping plane.
         * @param right  Right clipping plane.
         * @param top    Top clipping plane.
         * @param bottom Bottom clipping plane.
         * @param near   Near clipping distance.
         * @param far    Far clipping distance.
         */
        void Frustum(double left, double right, double top, double bottom, double near, double far) {
            std::array<double, 16> _tmp; Transform3D::GetFrustumProjection(_tmp, left, right, top, bottom, near, far); MultiplyMatrix(_tmp);
        }

        /**
         * @brief Right-multiplies a translation matrix.
         * @param x Translation along X. @param y Along Y. @param z Along Z.
         */
        void Translate(double x, double y, double z) {
            std::array<double, 16> _tmp; Transform3D::GetTranslation(_tmp, x, y, z); MultiplyMatrix(_tmp);
        }
        /**
         * @brief Right-multiplies a translation matrix from a vector.
         * @param amount Translation vector.
         */
        void Translate(const Vector3 &amount) {
            std::array<double, 16> _tmp; Transform3D::GetTranslation(_tmp, amount); MultiplyMatrix(_tmp);
        }

        /**
         * @brief Right-multiplies a scale matrix.
         * @param x Scale along X. @param y Along Y. @param z Along Z.
         */
        void Scale(double x, double y, double z) {
            std::array<double, 16> _tmp; Transform3D::GetScale(_tmp, x, y, z); MultiplyMatrix(_tmp);
        }
        /**
         * @brief Right-multiplies a scale matrix from a vector.
         * @param amount Scale vector.
         */
        void Scale(const Vector3 &amount) {
            std::array<double, 16> _tmp; Transform3D::GetScale(_tmp, amount); MultiplyMatrix(_tmp);
        }

        /**
         * @brief Right-multiplies a rotation matrix from a Quaternion.
         * @param rotation Source quaternion.
         */
        void Rotate(const Quaternion& rotation) {
            std::array<double, 16> _tmp; Transform3D::GetRotation(_tmp, rotation); MultiplyMatrix(_tmp);
        }
        /**
         * @brief Right-multiplies a rotation matrix from Euler angles.
         * @param x Pitch (radians). @param y Yaw (radians). @param z Roll (radians).
         */
        void Rotate(double x, double y, double z) {
            std::array<double, 16> _tmp; Transform3D::GetRotation(_tmp, x, y, z); MultiplyMatrix(_tmp);
        }
        /**
         * @brief Right-multiplies a rotation matrix from a Vector3 of Euler angles.
         * @param amount Euler angles vector (radians).
         */
        void Rotate(const Vector3 &amount) {
            std::array<double, 16> _tmp; Transform3D::GetRotation(_tmp, amount); MultiplyMatrix(_tmp);
        }

        /**
         * @brief Right-multiplies a combined TRS (translate × rotate × scale) matrix.
         * @param translation Translation vector.
         * @param rotation    Euler rotation angles (radians).
         * @param scale       Scale vector.
         */
        void Transform(const Vector3 &translation, const Vector3 &rotation, const Vector3 &scale) {
            std::array<double, 16> _tmp; Transform3D::GetTransformation(_tmp, translation, rotation, scale); MultiplyMatrix(_tmp);
        }

        // Rendering

        /**
         * @brief Renders a single triangle through the full 3D pipeline.
         * @param a First vertex.
         * @param b Second vertex.
         * @param c Third vertex.
         */
        void RenderTriangle(const WorldVertex3D<ColorT> &a, const WorldVertex3D<ColorT> &b, const WorldVertex3D<ColorT> &c) {
            input_g3d.RenderTriangle(a, b, c);
        }
        /**
         * @brief Renders indexed triangle geometry through the full 3D pipeline.
         * @param vertices         Vertex array.
         * @param triangles_indices Index list; every three consecutive indices form one triangle.
         */
        void RenderGeometry(const std::vector<WorldVertex3D<ColorT>> &vertices, const std::vector<int> &triangles_indices) {
            input_g3d.RenderGeometry(vertices, triangles_indices);
        }
    };
}
