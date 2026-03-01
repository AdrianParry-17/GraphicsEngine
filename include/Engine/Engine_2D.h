#pragma once

#include "Engine_Renderer.h"
#include "Engine_Vector.h"
#include "Engine_Constant.h"
#include "Engine_Matrix.h"

#include <array>
#include <initializer_list>
#include <stack>
#include <vector>

namespace Engine {
    /// @brief 2D vertex with position and color payload.
    /// @tparam ColorT Vertex color/payload type.
    template <typename ColorT>
    struct Vertex2D {
        /// @brief X coordinate.
        double x = 0, y = 0;

        /// @brief Vertex color/payload.
        ColorT color;
        
        Vertex2D() = default;
        Vertex2D(double _xy) : x(_xy), y(_xy) {}
        Vertex2D(double _x, double _y) : x(_x), y(_y) {}
        Vertex2D(const Vector2& p) : x(p.x), y(p.y) {}
        Vertex2D(double _xy, const ColorT& _color) : x(_xy), y(_xy), color(_color) {}
        Vertex2D(double _x, double _y, const ColorT& _color) : x(_x), y(_y), color(_color) {}
        Vertex2D(const Vector2& p, const ColorT& _color) : x(p.x), y(p.y), color(_color) {}

        /// @brief Returns vertex position as Vector2.
        Vector2 GetPosition() const { return Vector2(x, y); }
    };

    /// @brief 2D triangle rendering interface.
    /// @tparam ColorT Vertex color/payload type.
    template <typename ColorT>
    struct IGraphics2D {
        /// @brief Virtual destructor.
        virtual ~IGraphics2D() = default;

        /// @brief Renders one triangle.
        virtual void RenderTriangle(const Vertex2D<ColorT>& a, const Vertex2D<ColorT>& b, const Vertex2D<ColorT>& c) = 0;

        /// @brief Renders indexed triangle geometry.
        virtual void RenderGeometry(const std::vector<Vertex2D<ColorT>>& vertices, const std::vector<int>& triangles_indices);
    };

    /// @brief Adapts RendererGraphics into IGraphics2D.
    /// @tparam ColorT Vertex color/payload type.
    template <typename ColorT>
    class RendererGraphics2D : public IGraphics2D<ColorT> {
    private:
        RendererGraphics<ColorT>& renderer;
    public:
        /// @brief Constructs from a RendererGraphics instance.
        RendererGraphics2D(RendererGraphics<ColorT>& _renderer) : renderer(_renderer) {}

        /// @brief Renders one triangle.
        void RenderTriangle(const Vertex2D<ColorT>& a, const Vertex2D<ColorT>& b, const Vertex2D<ColorT>& c) override;
    
        /// @brief Gets wrapped renderer (non-const).
        RendererGraphics<ColorT>& GetInternalRenderer() { return renderer; }

        /// @brief Gets wrapped renderer (const).
        const RendererGraphics<ColorT>& GetInternalRenderer() const { return renderer; }
    };

    /// @brief Forwarding wrapper for IGraphics2D.
    /// @tparam ColorT Vertex color/payload type.
    template <typename ColorT>
    class Graphics2DWrapper : public IGraphics2D<ColorT> {
    private:
        IGraphics2D<ColorT>& graphics;
    public:
        /// @brief Constructs wrapper from another 2D graphics target.
        Graphics2DWrapper(IGraphics2D<ColorT>& _graphics) : graphics(_graphics) {}

        /// @brief Forwards triangle rendering.
        void RenderTriangle(const Vertex2D<ColorT>& a, const Vertex2D<ColorT>& b, const Vertex2D<ColorT>& c) override {
            return graphics.RenderTriangle(a, b, c);
        }

        /// @brief Forwards indexed geometry rendering.
        void RenderGeometry(const std::vector<Vertex2D<ColorT>>& vertices, const std::vector<int>& triangles_indices) override {
            return graphics.RenderGeometry(vertices, triangles_indices);
        }

        /// @brief Gets wrapped graphics target (non-const).
        IGraphics2D<ColorT>& GetInternalGraphics() { return graphics; }

        /// @brief Gets wrapped graphics target (const).
        const IGraphics2D<ColorT>& GetInternalGraphics() const { return graphics; }
    };

    template <typename ColorTIn, typename ColorTOut>
    /// @brief Converts vertex type/payload before forwarding to another 2D renderer.
    /// @tparam ColorTIn Input payload type.
    /// @tparam ColorTOut Output payload type.
    class ConvertedGraphics2D : public IGraphics2D<ColorTIn> {
    private:
        IGraphics2D<ColorTOut>& graphics;
        std::function<void(const Vertex2D<ColorTIn>&, Vertex2D<ColorTOut>&)> converter;
    public:
        /// @brief Constructs a converting wrapper.
        ConvertedGraphics2D(IGraphics2D<ColorTOut>& _graphics, const std::function<void(const Vertex2D<ColorTIn>&, Vertex2D<ColorTOut>&)>& _converter) : graphics(_graphics), converter(_converter) {}

        /// @brief Converts and renders one triangle.
        void RenderTriangle(const Vertex2D<ColorTIn>& a, const Vertex2D<ColorTIn>& b, const Vertex2D<ColorTIn>& c) override {
            Vertex2D<ColorTOut> v_a, v_b, v_c;
            converter(a, v_a); converter(b, v_b); converter(c, v_c);

            graphics.RenderTriangle(v_a, v_b, v_c);
        }
        /// @brief Converts and renders indexed geometry.
        void RenderGeometry(const std::vector<Vertex2D<ColorTIn>>& vertices, const std::vector<int>& triangles_indices) override {
            std::vector<Vertex2D<ColorTOut>> converted_vertices(vertices.size());
            for (int i = 0; i < vertices.size(); ++i)
                converter(vertices[i], converted_vertices[i]);
            graphics.RenderGeometry(converted_vertices, triangles_indices);
        }
    };

    
    /// @brief Alias for position-only conversion (same payload type).
    template <typename ColorT>
    using PositionConvertedGraphics2D = ConvertedGraphics2D<ColorT, ColorT>;

    /// @brief Applies viewport-to-viewport coordinate mapping before rendering.
    /// @tparam ColorT Vertex color/payload type.
    template <typename ColorT>
    class ViewportTransformGraphics2D : public Graphics2DWrapper<ColorT> {
    private:
        double left_i, right_i, top_i, bottom_i;
        double left_o, right_o, top_o, bottom_o;
    public:
        /// @brief Constructs viewport transformation wrapper.
        ViewportTransformGraphics2D(IGraphics2D<ColorT>& _graphics, double _left_i, double _right_i, double _top_i, double _bottom_i, double _left_o, double _right_o, double _top_o, double _bottom_o)
            : Graphics2DWrapper<ColorT>(_graphics), left_i(_left_i), right_i(_right_i), top_i(_top_i), bottom_i(_bottom_i), left_o(_left_o), right_o(_right_o), top_o(_top_o), bottom_o(_bottom_o) {}
        
        /// @brief Sets input viewport bounds.
        void SetInputViewport(double _left_i, double _right_i, double _top_i, double _bottom_i) {
            left_i = _left_i; right_i = _right_i; top_i = _top_i; bottom_i = _bottom_i;
        }

        /// @brief Sets output viewport bounds.
        void SetOutputViewport(double _left_o, double _right_o, double _top_o, double _bottom_o) {
            left_o = _left_o; right_o = _right_o; top_o = _top_o; bottom_o = _bottom_o;
        }

        /// @brief Transforms one position from input to output viewport.
        void TransformPosition(double x_in, double y_in, double& x_out, double& y_out) const;

        /// @brief Transforms one Vector2 position from input to output viewport.
        void TransformPosition(const Vector2& in, Vector2& out) const { TransformPosition(in.x, in.y, out.x, out.y); }

        /// @brief Transforms one position from input to output viewport.
        Vector2 TransformPosition(double x_in, double y_in) const;

        /// @brief Transforms one Vector2 position from input to output viewport.
        Vector2 TransformPosition(const Vector2& in) const { return TransformPosition(in.x, in.y); }

        /// @brief Renders one transformed triangle.
        void RenderTriangle(const Vertex2D<ColorT>& a, const Vertex2D<ColorT>& b, const Vertex2D<ColorT>& c) override;

        /// @brief Renders transformed indexed geometry.
        void RenderGeometry(const std::vector<Vertex2D<ColorT>>& vertices, const std::vector<int>& triangles_indices) override;
    };

    /// @brief Applies a 2x2 matrix stack transform before rendering.
    /// @tparam ColorT Vertex color/payload type.
    template <typename ColorT>
    class MatrixGraphics2D : public Graphics2DWrapper<ColorT> {
    private:
        std::stack<Matrix2x2> matrix_stack;
    public:
        /// @brief Constructs matrix wrapper and initializes stack with identity.
		MatrixGraphics2D(IGraphics2D<ColorT>& _graphics) : Graphics2DWrapper<ColorT>(_graphics) {
			// Initialize matrix stack
			matrix_stack.push(Matrix2x2::GetIdentity(1));
		}

		// --- Matrix Stack Management ---

        /// @brief Gets the current matrix data as raw array.
        void GetMatrixData(std::array<double, 4>& res) const;
        /// @brief Gets the current matrix data.
        void GetMatrixData(Matrix2x2& res) const;
		
        /// @brief Replaces current top matrix data.
		void SetMatrixData(const std::array<double, 4>& data);
        /// @brief Replaces current top matrix data.
		void SetMatrixData(const Matrix2x2& data);
				
        /// @brief Pushes a copy of current matrix onto the stack.
		void PushMatrix();
        /// @brief Pops matrix stack (with defensive fallback behavior).
		void PopMatrix();

		// --- Ulility ---

		// Either replace the current matrix with the previous matrix data, or set it to 'identity matrix'
		// If 'replace' is true, replace the current matrix data with the identity matrix, otherwise will replace with previous matrix data
		// or replace with identity matrix if it's the only matrix.
        /// @brief Loads identity according to stack policy.
		void LoadIdentity(bool replace = false);

        /// @brief Right-multiplies current matrix by provided matrix data.
		void MultiplyMatrix(const std::array<double, 4>& mat);
        /// @brief Right-multiplies current matrix by provided matrix.
		void MultiplyMatrix(const Matrix2x2& mat);
        /// @brief Generates and multiplies a matrix.
        void MultiplyMatrix(const std::function<void(std::array<double, 4>&)>& mat_gen);

        // --- Linear Transformation ---

        /// @brief Applies scaling transform.
		void Scale(const Vector2& amount);
        /// @brief Applies scaling transform.
		void Scale(double x, double y);

        /// @brief Applies rotation transform.
		void Rotate(double angle);

        // -- Utility --

		/// @brief Transforms one position with current matrix.
        void TransformPosition(const Vector2& in, Vector2& out) const;
		/// @brief Transforms one position with current matrix.
        Vector2 TransformPosition(const Vector2& in) const;

        // -- Main function --

		/// @brief Renders one transformed triangle.
        void RenderTriangle(const Vertex2D<ColorT>& a, const Vertex2D<ColorT>& b, const Vertex2D<ColorT>& c) override;
		/// @brief Renders transformed indexed geometry.
        void RenderGeometry(const std::vector<Vertex2D<ColorT>>& vertices, const std::vector<int>& triangles_indices) override;
    };

    /// @brief 2D plane (line) represented by ax + by + c = 0.
    struct Plane2D {
        /// @brief Plane coefficient for x.
        double a = 0, b = 0, c = 0;
    
        /// @brief Default constructor.
        Plane2D() = default;
        /// @brief Constructs from plane coefficients.
        Plane2D(double _a, double _b, double _c) : a(_a), b(_b), c(_c) {}
        /// @brief Constructs from normal and a point on the plane.
        Plane2D(double _vx, double _vy, double _x0, double _y0) : a(_vx), b(_vy), c(-(_vx*_x0 + _vy*_y0)) {}
        /// @brief Constructs from vector normal and point.
        Plane2D(const Vector2& v, const Vector2& p) : Plane2D(v.x, v.y, p.x, p.y) {}
        /// @brief Copy constructor.
        Plane2D(const Plane2D& _p) : a(_p.a), b(_p.b), c(_p.c) {}

        /// @brief Compares two planes.
        bool operator==(const Plane2D& p) const { return a == p.a && b == p.b && c == p.c; }

        /// @brief Compares two planes.
        bool operator!=(const Plane2D& p) const { return !(*this == p); }

        /// @brief Evaluates plane equation at a point.
        double EvaluatePoint(const Vector2& p) const { return a * p.x + b * p.y + c; }
    };

    /// @brief Clips triangles against one or more 2D planes before rendering.
    /// @tparam ColorT Vertex color/payload type.
    template <typename ColorT>
    class PlaneClippedGraphics2D : public Graphics2DWrapper<ColorT> {
    private:
        std::vector<Plane2D> planes;
        IInterpolator<ColorT>& color_interpolator; // Use for clipping

        bool __isInside(const Vector2& p, const Plane2D& plane) const;
        double __computePlaneIntersection(const Plane2D& plane, const Vector2& a, const Vector2& b) const;
    public:
        /// @brief Constructs clipper with no initial planes.
        PlaneClippedGraphics2D(IGraphics2D<ColorT>& _graphics, IInterpolator<ColorT>& _color_interpolator)
            : Graphics2DWrapper<ColorT>(_graphics), color_interpolator(_color_interpolator) {}

        /// @brief Constructs clipper with initial plane set.
        PlaneClippedGraphics2D(IGraphics2D<ColorT>& _graphics, IInterpolator<ColorT>& _color_interpolator, std::initializer_list<Plane2D> _planes)
            : Graphics2DWrapper<ColorT>(_graphics), color_interpolator(_color_interpolator), planes(_planes) {}
        
        /// @brief Replaces clip planes.
        void SetClipPlanes(const std::vector<Plane2D>& _planes) { planes = _planes; }

        /// @brief Replaces clip planes.
        void SetClipPlanes(const std::initializer_list<Plane2D>& _planes) { planes = _planes; }

        /// @brief Computes clipped polygon/triangles for a source triangle.
        void ComputeClippedTriangleGeometry(const Vertex2D<ColorT>& a, const Vertex2D<ColorT>& b, const Vertex2D<ColorT>& c, std::vector<Vertex2D<ColorT>>& out_vertices, std::vector<int>& out_triangles_indices) const;

        /// @brief Clips then renders one triangle.
        void RenderTriangle(const Vertex2D<ColorT>& a, const Vertex2D<ColorT>& b, const Vertex2D<ColorT>& c) override;

        /// @brief Clips then renders indexed geometry.
        void RenderGeometry(const std::vector<Vertex2D<ColorT>>& vertices, const std::vector<int>& triangles_indices) override;
    };
}

// -- IGraphics2D --

template <typename ColorT>
inline void Engine::IGraphics2D<ColorT>::RenderGeometry(const std::vector<Vertex2D<ColorT>>& vertices, const std::vector<int>& triangles_indices) {
    // Not enough to render check
    if (vertices.empty() || triangles_indices.size() < 3) return;

    for (int i = 2; i < triangles_indices.size(); i += 3) {
        int i1 = triangles_indices[i-2], i2 = triangles_indices[i-1], i3 = triangles_indices[i];
        // Invalid triangle check
        if (i1 < 0 || i2 < 0 || i3 < 0 || i1 >= vertices.size() || i2 >= vertices.size() || i3 >= vertices.size())
            continue;
        
        this->RenderTriangle(vertices[i1], vertices[i2], vertices[i3]);
    }
}

// -- RendererGraphics2D --

template <typename ColorT>
inline void Engine::RendererGraphics2D<ColorT>::RenderTriangle(const Vertex2D<ColorT>& a, const Vertex2D<ColorT>& b, const Vertex2D<ColorT>& c) {
    return renderer.RenderTriangle((int)a.x, (int)a.y, (int)b.x, (int)b.y, (int)c.x, (int)c.y, a.color, b.color, c.color);
}

// -- ViewportTransformGraphics2D --

template <typename ColorT>
inline void Engine::ViewportTransformGraphics2D<ColorT>::TransformPosition(double x_in, double y_in, double& x_out, double& y_out) const {
    // Transform X
    // - Invalid case (since best effort, because right/left input are too near, so we don't transform).
    if (NumericConstants::IsNearZero(right_i-left_i)) x_out = x_in;
    else x_out = (x_in * (right_o - left_o) + (right_i * left_o - right_o * left_i)) / (right_i - left_i); // Normal case

    // Transform Y
    // - Invalid case (since best effort, because top/bottom input are too near, so we don't transform).
    if (NumericConstants::IsNearZero(top_i-bottom_i)) y_out = y_in;
    else y_out = (y_in * (top_o - bottom_o) + (top_i * bottom_o - top_o * bottom_i)) / (top_i - bottom_i); // Normal case
}
template <typename ColorT>
inline Engine::Vector2 Engine::ViewportTransformGraphics2D<ColorT>::TransformPosition(double x_in, double y_in) const {
    double x_res = x_in, y_res = y_in;
    TransformPosition(x_in, y_in, x_res, y_res);
    return Vector2(x_res, y_res);
}
template <typename ColorT>
inline void Engine::ViewportTransformGraphics2D<ColorT>::RenderTriangle(const Vertex2D<ColorT>& a, const Vertex2D<ColorT>& b, const Vertex2D<ColorT>& c) {
    // Compute new vertices (aka compute new position)
    Vertex2D<ColorT> v_a(a), v_b(b), v_c(c);
    TransformPosition(a.x, a.y, v_a.x, v_a.y);
    TransformPosition(b.x, b.y, v_b.x, v_b.y);
    TransformPosition(c.x, c.y, v_c.x, v_c.y);

    return Graphics2DWrapper<ColorT>::RenderTriangle(v_a, v_b, v_c);
}
template <typename ColorT>
inline void Engine::ViewportTransformGraphics2D<ColorT>::RenderGeometry(const std::vector<Vertex2D<ColorT>>& vertices, const std::vector<int>& triangles_indices) {
    std::vector<Vertex2D<ColorT>> tmp_v(vertices);
    for (Vertex2D<ColorT>& v : tmp_v)
        TransformPosition(v.x, v.y, v.x, v.y); // Transform position affect x and y independently, and only change output once input was done reading.
    
    return Graphics2DWrapper<ColorT>::RenderGeometry(tmp_v, triangles_indices);
}

// -- MatrixGraphics2D --

template <typename ColorT>
inline void Engine::MatrixGraphics2D<ColorT>::GetMatrixData(std::array<double, 4>& res) const {
    // Note that this is unexpected, the matrix_stack size are always bigger than 1. But handle just in case.
    if (matrix_stack.empty()) Matrix2x2::GetIdentity(res, 1);
    else res = matrix_stack.top().Data();
}
template <typename ColorT>
inline void Engine::MatrixGraphics2D<ColorT>::GetMatrixData(Matrix2x2& res) const { return GetMatrixData(res.Data()); }

// Only use this if you know what you're doing, will replaced the top matrix data with the given data
template <typename ColorT>
inline void Engine::MatrixGraphics2D<ColorT>::SetMatrixData(const std::array<double, 4>& data) { return SetMatrixData(Matrix2x2(data)); }
template <typename ColorT>
inline void Engine::MatrixGraphics2D<ColorT>::SetMatrixData(const Matrix2x2& data) {
    if (matrix_stack.empty()) matrix_stack.push(data);
    else matrix_stack.top() = data;
}
        
template <typename ColorT>
inline void Engine::MatrixGraphics2D<ColorT>::PushMatrix() { matrix_stack.push(matrix_stack.top()); }
template <typename ColorT>
inline void Engine::MatrixGraphics2D<ColorT>::PopMatrix() {
    // This case is unexpected and not supposed to happened, but handle just in case
    if (matrix_stack.empty()) {
        matrix_stack.push(Matrix2x2::GetIdentity(1));
        return;
    }
    // If there's only 1 matrix, set it to identity.
    if (matrix_stack.size() == 1) {
        Matrix2x2::GetIdentity(matrix_stack.top(), 1);
        return;
    }

    // Otherwise, pop it.
    matrix_stack.pop();
}

template <typename ColorT>
inline void Engine::MatrixGraphics2D<ColorT>::LoadIdentity(bool replace) {
    if (matrix_stack.empty()) {
        matrix_stack.push(Matrix2x2::GetIdentity(1));
        return;
    }

    if (replace || matrix_stack.size() == 1) {
        Matrix2x2::GetIdentity(matrix_stack.top(), 1);
        return;
    }

    // Pop + Re-push matrix
    matrix_stack.pop();
    matrix_stack.push(matrix_stack.top());
}

template <typename ColorT>
inline void Engine::MatrixGraphics2D<ColorT>::MultiplyMatrix(const std::array<double, 4>& mat) {
    if (matrix_stack.empty()) {
        matrix_stack.push(Matrix2x2(mat));
        return;
    }

    matrix_stack.top() *= mat;
}
template <typename ColorT>
inline void Engine::MatrixGraphics2D<ColorT>::MultiplyMatrix(const Matrix2x2& mat) { return MultiplyMatrix(mat.Data()); }
template <typename ColorT>
inline void Engine::MatrixGraphics2D<ColorT>::MultiplyMatrix(const std::function<void(std::array<double, 4>&)>& mat_gen) {
    std::array<double, 4> _tmp;
    mat_gen(_tmp);
    MultiplyMatrix(_tmp);
}

template <typename ColorT>
inline void Engine::MatrixGraphics2D<ColorT>::Scale(const Vector2& amount) {
    std::array<double, 4> _tmp;
    Matrix2x2::GetScale(_tmp, amount);
    MultiplyMatrix(_tmp);
}
template <typename ColorT>
inline void Engine::MatrixGraphics2D<ColorT>::Scale(double x, double y) { return Scale(Vector2(x, y)); }

template <typename ColorT>
inline void Engine::MatrixGraphics2D<ColorT>::Rotate(double angle) {
    std::array<double, 4> _tmp;
    Matrix2x2::GetRotation(_tmp, angle);
    MultiplyMatrix(_tmp);
}

template <typename ColorT>
inline void Engine::MatrixGraphics2D<ColorT>::TransformPosition(const Vector2& in, Vector2& out) const {
    out = matrix_stack.empty() ? in : (matrix_stack.top() * in);
}
template <typename ColorT>
inline Engine::Vector2 Engine::MatrixGraphics2D<ColorT>::TransformPosition(const Vector2& in) const {
    Vector3 out = in;
    TransformPosition(in, out);
    return out;
}
template <typename ColorT>
inline void Engine::MatrixGraphics2D<ColorT>::RenderTriangle(const Vertex2D<ColorT>& a, const Vertex2D<ColorT>& b, const Vertex2D<ColorT>& c) {
    // Compute new vertices (aka compute new position)
    Vertex2D<ColorT> v_a(a), v_b(b), v_c(c);
    Vector2 pa, pb, pc;
    TransformPosition(a.GetPosition(), pa);
    TransformPosition(b.GetPosition(), pb);
    TransformPosition(c.GetPosition(), pc);

    v_a.x = pa.x; v_a.y = pa.y;
    v_b.x = pb.x; v_b.y = pb.y;
    v_c.x = pc.x; v_c.y = pc.y;

    return Graphics2DWrapper<ColorT>::RenderTriangle(v_a, v_b, v_c);
}
template <typename ColorT>
inline void Engine::MatrixGraphics2D<ColorT>::RenderGeometry(const std::vector<Vertex2D<ColorT>>& vertices, const std::vector<int>& triangles_indices) {
    std::vector<Vertex2D<ColorT>> tmp_v(vertices);
    for (Vertex2D<ColorT>& v : tmp_v) {
        Vector2 p;
        TransformPosition(v.GetPosition(), p);
        v.x = p.x; v.y = p.y;
    }
    
    return Graphics2DWrapper<ColorT>::RenderGeometry(tmp_v, triangles_indices);
}

// -- PlaneClippedGraphics2D --

template <typename ColorT>
inline bool Engine::PlaneClippedGraphics2D<ColorT>::__isInside(const Vector2& p, const Plane2D& plane) const {
    return NumericConstants::IsNearZeroOrPositive(plane.EvaluatePoint(p));
}
template <typename ColorT>
inline double Engine::PlaneClippedGraphics2D<ColorT>::__computePlaneIntersection(const Plane2D& plane, const Vector2& a, const Vector2& b) const {
    double a_val = plane.EvaluatePoint(a), b_val = plane.EvaluatePoint(b);
    
    double factor = a_val - b_val;
    if (NumericConstants::IsNearZero(factor))
        return 0.5; // Best effort, the line is (or almost) parallel to the plane, chose 0.5 for numerical stability.

    return a_val / factor;
}

template <typename ColorT>
inline void Engine::PlaneClippedGraphics2D<ColorT>::ComputeClippedTriangleGeometry(const Vertex2D<ColorT>& a, const Vertex2D<ColorT>& b, const Vertex2D<ColorT>& c, std::vector<Vertex2D<ColorT>>& out_vertices, std::vector<int>& out_triangles_indices) const {
    // Initialize
    out_vertices.clear();
    out_triangles_indices.clear();

    // Algorithm init
    std::vector<Vertex2D<ColorT>> polygon = {a, b, c};
    std::vector<Vertex2D<ColorT>> tmp;

    for (const Plane2D& plane : planes) {
        // No triangle can be form
        if (polygon.size() < 3) break;
    
        tmp.clear();
        for (int i = 0; i < polygon.size(); ++i) {
            Vertex2D<ColorT>& prev = polygon[(polygon.size() - 1 + i) % polygon.size()];
            Vertex2D<ColorT>& curr = polygon[i];

            bool prev_inside = __isInside(prev.GetPosition(), plane);
            bool curr_inside = __isInside(curr.GetPosition(), plane);

            // Exactly one of the vertex not inside, append the intersection point
            if (curr_inside != prev_inside) {
                
                tmp.push_back(prev); // prev here is just tmp, avoiding 'no default constructor for ColorT'.
                Vertex2D<ColorT>& intersection = tmp.back();

                double t = __computePlaneIntersection(plane, prev.GetPosition(), curr.GetPosition());
                t = std::max(0.0, std::min(1.0, t)); // Clamp t to [0, 1]

                intersection.x = prev.x + t * (curr.x - prev.x);
                intersection.y = prev.y + t * (curr.y - prev.y);
                color_interpolator.Linear(prev.color, curr.color, t, intersection.color);
            }

            // Target vertex inside, add it.
            if (curr_inside)
                tmp.push_back(curr);
        }

        polygon.swap(tmp); // Update the polygon
    }

    // No triangle can be form afrer clipping
    if (polygon.size() < 3) return;

    // Otherwise, output result
    // - Vertices
    out_vertices.swap(polygon);
    // - Triangles
    for (int i = 2; i < out_vertices.size(); ++i) {
        out_triangles_indices.push_back(0);
        out_triangles_indices.push_back(i-1);
        out_triangles_indices.push_back(i);
    }
}

template <typename ColorT>
inline void Engine::PlaneClippedGraphics2D<ColorT>::RenderTriangle(const Vertex2D<ColorT>& a, const Vertex2D<ColorT>& b, const Vertex2D<ColorT>& c) {
    std::vector<Vertex2D<ColorT>> vertices;
    std::vector<int> triangles_indices;

    ComputeClippedTriangleGeometry(a, b, c, vertices, triangles_indices);

    Graphics2DWrapper<ColorT>::RenderGeometry(vertices, triangles_indices);
}

template <typename ColorT>
inline void Engine::PlaneClippedGraphics2D<ColorT>::RenderGeometry(const std::vector<Vertex2D<ColorT>>& vertices, const std::vector<int>& triangles_indices) {
    // Not enough to render check
    if (vertices.empty() || triangles_indices.size() < 3) return;

    for (int i = 2; i < triangles_indices.size(); i += 3) {
        int i1 = triangles_indices[i-2], i2 = triangles_indices[i-1], i3 = triangles_indices[i];
        // Invalid triangle check
        if (i1 < 0 || i2 < 0 || i3 < 0 || i1 >= vertices.size() || i2 >= vertices.size() || i3 >= vertices.size())
            continue;
        
        this->RenderTriangle(vertices[i1], vertices[i2], vertices[i3]);
    }
}
