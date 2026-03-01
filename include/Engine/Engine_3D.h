#pragma once

#include "Engine_2D.h"
#include "Engine_Vector.h"
#include "Engine_Constant.h"
#include "Engine_Matrix.h"
#include "Engine_Interpolation.h"

#include <array>
#include <functional>
#include <stack>
#include <vector>

namespace Engine {
    /// @brief 3D vertex with position and color payload.
    /// @tparam ColorT Vertex color/payload type.
    template <typename ColorT>
    struct Vertex3D {
        /// @brief X, Y, Z coordinates.
        double x = 0, y = 0, z = 0;

        /// @brief Vertex color/payload.
        ColorT color;

        Vertex3D() = default;
        Vertex3D(double _xyz) : x(_xyz), y(_xyz), z(_xyz) {}
        Vertex3D(double _x, double _y, double _z) : x(_x), y(_y), z(_z) {}
        Vertex3D(const Vector3& p) : x(p.x), y(p.y), z(p.z) {}
        Vertex3D(double _xyz, const ColorT& _color) : x(_xyz), y(_xyz), z(_xyz), color(_color) {}
        Vertex3D(double _x, double _y, double _z, const ColorT& _color) : x(_x), y(_y), z(_z), color(_color) {}
        Vertex3D(const Vector3& p, const ColorT& _color) : x(p.x), y(p.y), z(p.z), color(_color) {}

        /// @brief Returns vertex position as Vector3.
        Vector3 GetPosition() const { return Vector3(x, y, z); }
    };

    /// @brief 3D triangle rendering interface.
    /// @tparam ColorT Vertex color/payload type.
    template <typename ColorT>
    struct IGraphics3D {
        /// @brief Virtual destructor.
        virtual ~IGraphics3D() = default;

        /// @brief Renders one 3D triangle.
        virtual void RenderTriangle(const Vertex3D<ColorT>& a, const Vertex3D<ColorT>& b, const Vertex3D<ColorT>& c) = 0;

        /// @brief Renders indexed triangle geometry.
        virtual void RenderGeometry(const std::vector<Vertex3D<ColorT>>& vertices, const std::vector<int>& triangles_indices);
    };

    template <typename ColorTIn, typename ColorTOut>
    /// @brief Projects 3D vertices to 2D and forwards rendering.
    /// @tparam ColorTIn Input payload type.
    /// @tparam ColorTOut Output payload type.
    class Projected2DGraphics3D : public IGraphics3D<ColorTIn> {
    private:
        IGraphics2D<ColorTOut>& graphics;
        std::function<void(const Vertex3D<ColorTIn>&, Vertex2D<ColorTOut>&)> projector;
    public:
        /// @brief Constructs a 3D-to-2D projection adapter.
        Projected2DGraphics3D(IGraphics2D<ColorTOut>& _graphics, const std::function<void(const Vertex3D<ColorTIn>&, Vertex2D<ColorTOut>&)>& _projector)
            : graphics(_graphics), projector(_projector) {}

        /// @brief Projects one 3D vertex to 2D.
        void Project(const Vertex3D<ColorTIn>& in, Vertex2D<ColorTOut>& out) const;

        /// @brief Replaces projection callback.
        void SetProjector(const std::function<void(const Vertex3D<ColorTIn>&, Vertex2D<ColorTOut>&)>& _projector) { projector = _projector; }

        /// @brief Projects and renders one triangle.
        void RenderTriangle(const Vertex3D<ColorTIn>& a, const Vertex3D<ColorTIn>& b, const Vertex3D<ColorTIn>& c) override;

        /// @brief Projects and renders indexed geometry.
        void RenderGeometry(const std::vector<Vertex3D<ColorTIn>>& vertices, const std::vector<int>& triangles_indices) override;
    
        /// @brief Gets wrapped 2D graphics target (non-const).
        IGraphics2D<ColorTOut>& GetInternal2DGraphics() { return graphics; }

        /// @brief Gets wrapped 2D graphics target (const).
        const IGraphics2D<ColorTOut>& GetInternal2DGraphics() const { return graphics; }
    };

    /// @brief Alias for payload-preserving 3D-to-2D projection.
    template <typename ColorT>
    using PositionProjected2DGraphics3D = Projected2DGraphics3D<ColorT, ColorT>;

    template <typename ColorTIn, typename ColorTOut>
    /// @brief Embeds 2D vertices into 3D and forwards rendering.
    /// @tparam ColorTIn Input payload type.
    /// @tparam ColorTOut Output payload type.
    class Embed3DGraphics2D : public IGraphics2D<ColorTIn> {
    private:
        IGraphics3D<ColorTOut>& graphics;
        std::function<void(const Vertex2D<ColorTIn>&, Vertex3D<ColorTOut>&)> embedder;
    public:
        /// @brief Constructs a 2D-to-3D embedding adapter.
        Embed3DGraphics2D(IGraphics3D<ColorTOut>& _graphics, const std::function<void(const Vertex2D<ColorTIn>&, Vertex3D<ColorTOut>&)>& _embedder)
            : graphics(_graphics), embedder(_embedder) {}

        /// @brief Embeds one 2D vertex to 3D.
        void Embed(const Vertex2D<ColorTIn>& in, Vertex3D<ColorTOut>& out) const;

        /// @brief Replaces embedding callback.
        void SetEmbedder(const std::function<void(const Vertex2D<ColorTIn>&, Vertex3D<ColorTOut>&)>& _embedder) { embedder = _embedder; }

        /// @brief Embeds and renders one triangle.
        void RenderTriangle(const Vertex2D<ColorTIn>& a, const Vertex2D<ColorTIn>& b, const Vertex2D<ColorTIn>& c) override;

        /// @brief Embeds and renders indexed geometry.
        void RenderGeometry(const std::vector<Vertex2D<ColorTIn>>& vertices, const std::vector<int>& triangles_indices) override;
    
        /// @brief Gets wrapped 3D graphics target (non-const).
        IGraphics3D<ColorTOut>& GetInternal3DGraphics() { return graphics; }

        /// @brief Gets wrapped 3D graphics target (const).
        const IGraphics3D<ColorTOut>& GetInternal3DGraphics() const { return graphics; }
    };

    /// @brief Alias for payload-preserving 2D-to-3D embedding.
    template <typename ColorT>
    using PositionEmbed3DGraphics2D = Embed3DGraphics2D<ColorT, ColorT>;

    template <typename ColorTIn, typename ColorTOut>
    /// @brief Converts 3D vertex payload before forwarding rendering.
    /// @tparam ColorTIn Input payload type.
    /// @tparam ColorTOut Output payload type.
    class ConvertedGraphics3D : public IGraphics3D<ColorTIn> {
    private:
        IGraphics3D<ColorTOut>& graphics;
        std::function<void(const Vertex3D<ColorTIn>&, Vertex3D<ColorTOut>&)> converter;
    public:
        /// @brief Constructs a converting 3D graphics wrapper.
        ConvertedGraphics3D(IGraphics3D<ColorTOut>& _graphics, const std::function<void(const Vertex3D<ColorTIn>&, Vertex3D<ColorTOut>&)>& _converter) : graphics(_graphics), converter(_converter) {}

        /// @brief Converts and renders one triangle.
        void RenderTriangle(const Vertex3D<ColorTIn>& a, const Vertex3D<ColorTIn>& b, const Vertex3D<ColorTIn>& c) override {
            Vertex3D<ColorTOut> v_a, v_b, v_c;
            converter(a, v_a); converter(b, v_b); converter(c, v_c);

            graphics.RenderTriangle(v_a, v_b, v_c);
        }
        /// @brief Converts and renders indexed geometry.
        void RenderGeometry(const std::vector<Vertex3D<ColorTIn>>& vertices, const std::vector<int>& triangles_indices) override {
            std::vector<Vertex3D<ColorTOut>> converted_vertices(vertices.size());
            for (int i = 0; i < vertices.size(); ++i)
                converter(vertices[i], converted_vertices[i]);
            graphics.RenderGeometry(converted_vertices, triangles_indices);
        }
    };

    /// @brief Alias for payload-preserving 3D conversion wrapper.
    template <typename ColorT>
    using PositionConvertedGraphics3D = ConvertedGraphics3D<ColorT, ColorT>;

    /// @brief Forwarding wrapper for IGraphics3D.
    /// @tparam ColorT Vertex color/payload type.
    template <typename ColorT>
    class Graphics3DWrapper : public IGraphics3D<ColorT> {
    private:
        IGraphics3D<ColorT>& graphics;
    public:
        /// @brief Constructs wrapper from another 3D graphics target.
        Graphics3DWrapper(IGraphics3D<ColorT>& _graphics) : graphics(_graphics) {}

        /// @brief Forwards triangle rendering.
        void RenderTriangle(const Vertex3D<ColorT>& a, const Vertex3D<ColorT>& b, const Vertex3D<ColorT>& c) override {
            return graphics.RenderTriangle(a, b, c);
        }

        /// @brief Forwards indexed geometry rendering.
        void RenderGeometry(const std::vector<Vertex3D<ColorT>>& vertices, const std::vector<int>& triangles_indices) override {
            return graphics.RenderGeometry(vertices, triangles_indices);
        }

        /// @brief Gets wrapped graphics target (non-const).
        IGraphics3D<ColorT>& GetInternalGraphics() { return graphics; }

        /// @brief Gets wrapped graphics target (const).
        const IGraphics3D<ColorT>& GetInternalGraphics() const { return graphics; }
    };

    /// @brief Applies a 3x3 matrix stack transform before rendering.
    /// @tparam ColorT Vertex color/payload type.
    template <typename ColorT>
    class MatrixGraphics3D : public Graphics3DWrapper<ColorT> {
    private:
        std::stack<Matrix3x3> matrix_stack;
    public:
        /// @brief Constructs matrix wrapper and initializes stack with identity.
		MatrixGraphics3D(IGraphics3D<ColorT>& _graphics) : Graphics3DWrapper<ColorT>(_graphics) {
			// Initialize matrix stack
			matrix_stack.push(Matrix3x3::GetIdentity(1));
		}

		// --- Matrix Stack Management ---

        /// @brief Gets current matrix as raw array.
        void GetMatrixData(std::array<double, 9>& res) const;
        /// @brief Gets current matrix.
        void GetMatrixData(Matrix3x3& res) const;
		
        /// @brief Replaces current top matrix data.
		void SetMatrixData(const std::array<double, 9>& data);
        /// @brief Replaces current top matrix data.
		void SetMatrixData(const Matrix3x3& data);
				
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

        /// @brief Right-multiplies current matrix.
		void MultiplyMatrix(const std::array<double, 9>& mat);
        /// @brief Right-multiplies current matrix.
		void MultiplyMatrix(const Matrix3x3& mat);
        /// @brief Generates then multiplies a matrix.
        void MultiplyMatrix(const std::function<void(std::array<double, 9>&)>& mat_gen);

		// --- Transformation 2D ---
		
        /// @brief Applies 2D translation (homogeneous 3x3 form).
		void Translate2D(const Vector2& amount);
        /// @brief Applies 2D translation (homogeneous 3x3 form).
		void Translate2D(double x, double y);

        /// @brief Applies 2D scaling.
		void Scale2D(const Vector2& amount);
        /// @brief Applies 2D scaling.
		void Scale2D(double x, double y);

        /// @brief Applies 2D rotation.
		void Rotate2D(double angle);

        /// @brief Applies combined 2D transform.
		void Transform2D(const Vector2& translation, double rotation_angle, const Vector2& scale);
        /// @brief Applies viewport transform matrix in 2D homogeneous space.
        void ViewportTransform2D(double left_i, double right_i, double top_i, double bottom_i, double left_o, double right_o, double top_o, double bottom_o);

        // --- Transformation 3D ---

        /// @brief Applies 3D scaling.
		void Scale3D(const Vector3& amount);
        /// @brief Applies 3D scaling.
		void Scale3D(double x, double y, double z);

        /// @brief Applies 3D rotation.
		void Rotate3D(const Vector3& amount);
        /// @brief Applies 3D rotation.
		void Rotate3D(double x, double y, double z);

        // -- Utility --

		/// @brief Transforms one position by current matrix.
        void TransformPosition(const Vector3& in, Vector3& out) const;
		/// @brief Transforms one position by current matrix.
        Vector3 TransformPosition(const Vector3& in) const;

        // -- Main function --

		/// @brief Renders one transformed triangle.
        void RenderTriangle(const Vertex3D<ColorT>& a, const Vertex3D<ColorT>& b, const Vertex3D<ColorT>& c) override;
		/// @brief Renders transformed indexed geometry.
        void RenderGeometry(const std::vector<Vertex3D<ColorT>>& vertices, const std::vector<int>& triangles_indices) override;
    };

    /// @brief 3D plane represented by ax + by + cz + d = 0.
    struct Plane3D {
        /// @brief Plane coefficients.
        double a = 0, b = 0, c = 0, d = 0;
    
        /// @brief Default constructor.
        Plane3D() = default;
        /// @brief Constructs from plane coefficients.
        Plane3D(double _a, double _b, double _c, double _d) : a(_a), b(_b), c(_c), d(_d) {}
        /// @brief Constructs from normal and point on plane.
        Plane3D(double _vx, double _vy, double _vz, double _x0, double _y0, double _z0) : a(_vx), b(_vy), c(_vz), d(-(_vx*_x0 + _vy*_y0 + _vz*_z0)) {}
        /// @brief Constructs from normal vector and point.
        Plane3D(const Vector3& v, const Vector3& p) : Plane3D(v.x, v.y, v.z, p.x, p.y, p.z) {}
        /// @brief Copy constructor.
        Plane3D(const Plane3D& _p) : a(_p.a), b(_p.b), c(_p.c), d(_p.d) {}

        /// @brief Compares two planes.
        bool operator==(const Plane3D& p) const { return a == p.a && b == p.b && c == p.c && d == p.d; }

        /// @brief Compares two planes.
        bool operator!=(const Plane3D& p) const { return !(*this == p); }

        /// @brief Evaluates plane equation at a point.
        double EvaluatePoint(const Vector3& p) const { return a * p.x + b * p.y + c * p.z + d; }
    };

    /// @brief Clips 3D triangles against one or more planes before rendering.
    /// @tparam ColorT Vertex color/payload type.
    template <typename ColorT>
    class PlaneClippedGraphics3D : public Graphics3DWrapper<ColorT> {
    private:
        std::vector<Plane3D> planes;
        IInterpolator<ColorT>& color_interpolator; // Use for clipping

        bool __isInside(const Vector3& p, const Plane3D& plane) const;
        double __computePlaneIntersection(const Plane3D& plane, const Vector3& a, const Vector3& b) const;
    public:
        /// @brief Constructs clipper with no initial planes.
        PlaneClippedGraphics3D(IGraphics3D<ColorT>& _graphics, IInterpolator<ColorT>& _color_interpolator)
            : Graphics3DWrapper<ColorT>(_graphics), color_interpolator(_color_interpolator) {}

        /// @brief Constructs clipper with initial planes.
        PlaneClippedGraphics3D(IGraphics3D<ColorT>& _graphics, IInterpolator<ColorT>& _color_interpolator, std::initializer_list<Plane3D> _planes)
            : Graphics3DWrapper<ColorT>(_graphics), color_interpolator(_color_interpolator), planes(_planes) {}
        
        /// @brief Replaces clip planes.
        void SetClipPlanes(const std::vector<Plane3D>& _planes) { planes = _planes; }

        /// @brief Replaces clip planes.
        void SetClipPlanes(const std::initializer_list<Plane3D>& _planes) { planes = _planes; }

        /// @brief Computes clipped geometry for a triangle.
        void ComputeClippedTriangleGeometry(const Vertex3D<ColorT>& a, const Vertex3D<ColorT>& b, const Vertex3D<ColorT>& c, std::vector<Vertex3D<ColorT>>& out_vertices, std::vector<int>& out_triangles_indices) const;

        /// @brief Clips and renders one triangle.
        void RenderTriangle(const Vertex3D<ColorT>& a, const Vertex3D<ColorT>& b, const Vertex3D<ColorT>& c) override;

        /// @brief Clips and renders indexed geometry.
        void RenderGeometry(const std::vector<Vertex3D<ColorT>>& vertices, const std::vector<int>& triangles_indices) override;
    };
}

// -- IGraphics3D --

template <typename ColorT>
inline void Engine::IGraphics3D<ColorT>::RenderGeometry(const std::vector<Vertex3D<ColorT>>& vertices, const std::vector<int>& triangles_indices) {
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

// -- Project2DGraphics3D --

template <typename ColorTIn, typename ColorTOut>
inline void Engine::Projected2DGraphics3D<ColorTIn, ColorTOut>::Project(const Vertex3D<ColorTIn>& in, Vertex2D<ColorTOut>& out) const {
    if (projector) projector(in, out);
    else { out.x = in.x; out.y = in.y; out.color = in.color; }
}
template <typename ColorTIn, typename ColorTOut>
inline void Engine::Projected2DGraphics3D<ColorTIn, ColorTOut>::RenderTriangle(const Vertex3D<ColorTIn>& a, const Vertex3D<ColorTIn>& b, const Vertex3D<ColorTIn>& c) {
    Vertex2D<ColorTOut> va, vb, vc;
    Project(a, va); Project(b, vb); Project(c, vc);

    return graphics.RenderTriangle(va, vb, vc);
}
template <typename ColorTIn, typename ColorTOut>
inline void Engine::Projected2DGraphics3D<ColorTIn, ColorTOut>::RenderGeometry(const std::vector<Vertex3D<ColorTIn>>& vertices, const std::vector<int>& triangles_indices) {
    std::vector<Vertex2D<ColorTOut>> vert_2d(vertices.size());
    for (int i = 0; i < vertices.size(); ++i)
        Project(vertices[i], vert_2d[i]);

    return graphics.RenderGeometry(vert_2d, triangles_indices);
}

// -- Embed3DGraphics2D --

template <typename ColorTIn, typename ColorTOut>
inline void Engine::Embed3DGraphics2D<ColorTIn, ColorTOut>::Embed(const Vertex2D<ColorTIn>& in, Vertex3D<ColorTOut>& out) const {
    if (embedder) embedder(in, out);
    else { out.x = in.x; out.y = in.y; out.z = 0; out.color = in.color; }
}
template <typename ColorTIn, typename ColorTOut>
inline void Engine::Embed3DGraphics2D<ColorTIn, ColorTOut>::RenderTriangle(const Vertex2D<ColorTIn>& a, const Vertex2D<ColorTIn>& b, const Vertex2D<ColorTIn>& c) {
    Vertex3D<ColorTOut> va, vb, vc;
    Embed(a, va); Embed(b, vb); Embed(c, vc);

    return graphics.RenderTriangle(va, vb, vc);
}
template <typename ColorTIn, typename ColorTOut>
inline void Engine::Embed3DGraphics2D<ColorTIn, ColorTOut>::RenderGeometry(const std::vector<Vertex2D<ColorTIn>>& vertices, const std::vector<int>& triangles_indices) {
    std::vector<Vertex3D<ColorTOut>> vert_3d(vertices.size());
    for (int i = 0; i < vertices.size(); ++i)
        Embed(vertices[i], vert_3d[i]);

    return graphics.RenderGeometry(vert_3d, triangles_indices);
}

// -- MatrixGraphics3D --

template <typename ColorT>
inline void Engine::MatrixGraphics3D<ColorT>::GetMatrixData(std::array<double, 9>& res) const {
    // Note that this is unexpected, the matrix_stack size are always bigger than 1. But handle just in case.
    if (matrix_stack.empty()) Matrix3x3::GetIdentity(res, 1);
    else res = matrix_stack.top().Data();
}
template <typename ColorT>
inline void Engine::MatrixGraphics3D<ColorT>::GetMatrixData(Matrix3x3& res) const { return GetMatrixData(res.Data()); }

// Only use this if you know what you're doing, will replaced the top matrix data with the given data
template <typename ColorT>
inline void Engine::MatrixGraphics3D<ColorT>::SetMatrixData(const std::array<double, 9>& data) { return SetMatrixData(Matrix3x3(data)); }
template <typename ColorT>
inline void Engine::MatrixGraphics3D<ColorT>::SetMatrixData(const Matrix3x3& data) {
    if (matrix_stack.empty()) matrix_stack.push(data);
    else matrix_stack.top() = data;
}
        
template <typename ColorT>
inline void Engine::MatrixGraphics3D<ColorT>::PushMatrix() { matrix_stack.push(matrix_stack.top()); }
template <typename ColorT>
inline void Engine::MatrixGraphics3D<ColorT>::PopMatrix() {
    // This case is unexpected and not supposed to happened, but handle just in case
    if (matrix_stack.empty()) {
        matrix_stack.push(Matrix3x3::GetIdentity(1));
        return;
    }
    // If there's only 1 matrix, set it to identity.
    if (matrix_stack.size() == 1) {
        Matrix3x3::GetIdentity(matrix_stack.top(), 1);
        return;
    }

    // Otherwise, pop it.
    matrix_stack.pop();
}

template <typename ColorT>
inline void Engine::MatrixGraphics3D<ColorT>::LoadIdentity(bool replace) {
    if (matrix_stack.empty()) {
        matrix_stack.push(Matrix3x3::GetIdentity(1));
        return;
    }

    if (replace || matrix_stack.size() == 1) {
        Matrix3x3::GetIdentity(matrix_stack.top(), 1);
        return;
    }

    // Pop + Re-push matrix
    matrix_stack.pop();
    matrix_stack.push(matrix_stack.top());
}

template <typename ColorT>
inline void Engine::MatrixGraphics3D<ColorT>::MultiplyMatrix(const std::array<double, 9>& mat) {
    if (matrix_stack.empty()) {
        matrix_stack.push(Matrix3x3(mat));
        return;
    }

    matrix_stack.top() *= mat;
}
template <typename ColorT>
inline void Engine::MatrixGraphics3D<ColorT>::MultiplyMatrix(const Matrix3x3& mat) { return MultiplyMatrix(mat.Data()); }
template <typename ColorT>
inline void Engine::MatrixGraphics3D<ColorT>::MultiplyMatrix(const std::function<void(std::array<double, 9>&)>& mat_gen) {
    std::array<double, 9> _tmp;
    mat_gen(_tmp);
    MultiplyMatrix(_tmp);
}

template <typename ColorT>
inline void Engine::MatrixGraphics3D<ColorT>::Translate2D(const Vector2& amount) {
    std::array<double, 9> _tmp;
    Matrix3x3::GetTranslation2D(_tmp, amount);
    MultiplyMatrix(_tmp);
}
template <typename ColorT>
inline void Engine::MatrixGraphics3D<ColorT>::Translate2D(double x, double y) { return Translate2D(Vector2(x, y)); }

template <typename ColorT>
inline void Engine::MatrixGraphics3D<ColorT>::Scale2D(const Vector2& amount) {
    std::array<double, 9> _tmp;
    Matrix3x3::GetScale2D(_tmp, amount);
    MultiplyMatrix(_tmp);
}
template <typename ColorT>
inline void Engine::MatrixGraphics3D<ColorT>::Scale2D(double x, double y) { return Scale2D(Vector2(x, y)); }

template <typename ColorT>
inline void Engine::MatrixGraphics3D<ColorT>::Rotate2D(double angle) {
    std::array<double, 9> _tmp;
    Matrix3x3::GetRotation2D(_tmp, angle);
    MultiplyMatrix(_tmp);
}

template <typename ColorT>
inline void Engine::MatrixGraphics3D<ColorT>::Transform2D(const Vector2& translation, double rotation_angle, const Vector2& scale) {
    std::array<double, 9> _tmp;
    Matrix3x3::GetTransformation2D(_tmp, translation, rotation_angle, scale);
    MultiplyMatrix(_tmp);
}

template <typename ColorT>
inline void Engine::MatrixGraphics3D<ColorT>::ViewportTransform2D(double left_i, double right_i, double top_i, double bottom_i, double left_o, double right_o, double top_o, double bottom_o) {
    std::array<double, 9> _tmp;
    Matrix3x3::GetViewportTransform2D(_tmp, left_i, right_i, top_i, bottom_i, left_o, right_o, top_o, bottom_o);
    MultiplyMatrix(_tmp);
}

template <typename ColorT>
inline void Engine::MatrixGraphics3D<ColorT>::Scale3D(const Vector3& amount) {
    std::array<double, 9> _tmp;
    Matrix3x3::GetScale3D(_tmp, amount);
    MultiplyMatrix(_tmp);
}
template <typename ColorT>
inline void Engine::MatrixGraphics3D<ColorT>::Scale3D(double x, double y, double z) { return Scale3D(Vector3(x, y, z)); }

template <typename ColorT>
inline void Engine::MatrixGraphics3D<ColorT>::Rotate3D(const Vector3& amount) {
    std::array<double, 9> _tmp;
    Matrix3x3::GetRotation3D(_tmp, amount);
    MultiplyMatrix(_tmp);
}
template <typename ColorT>
inline void Engine::MatrixGraphics3D<ColorT>::Rotate3D(double x, double y, double z) { return Rotate3D(Vector3(x, y, z)); }


template <typename ColorT>
inline void Engine::MatrixGraphics3D<ColorT>::TransformPosition(const Vector3& in, Vector3& out) const {
    out = matrix_stack.empty() ? in : (matrix_stack.top() * in);
}
template <typename ColorT>
inline Engine::Vector3 Engine::MatrixGraphics3D<ColorT>::TransformPosition(const Vector3& in) const {
    Vector3 out = in;
    TransformPosition(in, out);
    return out;
}

template <typename ColorT>
inline void Engine::MatrixGraphics3D<ColorT>::RenderTriangle(const Vertex3D<ColorT>& a, const Vertex3D<ColorT>& b, const Vertex3D<ColorT>& c) {
    // Compute new vertices (aka compute new position)
    Vertex3D<ColorT> v_a(a), v_b(b), v_c(c);
    Vector3 pa, pb, pc;
    TransformPosition(a.GetPosition(), pa);
    TransformPosition(b.GetPosition(), pb);
    TransformPosition(c.GetPosition(), pc);

    v_a.x = pa.x; v_a.y = pa.y; v_a.z = pa.z;
    v_b.x = pb.x; v_b.y = pb.y; v_b.z = pb.z;
    v_c.x = pc.x; v_c.y = pc.y; v_c.z = pc.z;

    return Graphics3DWrapper<ColorT>::RenderTriangle(v_a, v_b, v_c);
}
template <typename ColorT>
inline void Engine::MatrixGraphics3D<ColorT>::RenderGeometry(const std::vector<Vertex3D<ColorT>>& vertices, const std::vector<int>& triangles_indices) {
    std::vector<Vertex3D<ColorT>> tmp_v(vertices);
    for (Vertex3D<ColorT>& v : tmp_v) {
        Vector3 p;
        TransformPosition(v.GetPosition(), p);
        v.x = p.x; v.y = p.y; v.z = p.z;
    }
    
    return Graphics3DWrapper<ColorT>::RenderGeometry(tmp_v, triangles_indices);
}

// -- PlaneClippedGraphics3D --

template <typename ColorT>
inline bool Engine::PlaneClippedGraphics3D<ColorT>::__isInside(const Vector3& p, const Plane3D& plane) const {
    return NumericConstants::IsNearZeroOrPositive(plane.EvaluatePoint(p));
}
template <typename ColorT>
inline double Engine::PlaneClippedGraphics3D<ColorT>::__computePlaneIntersection(const Plane3D& plane, const Vector3& a, const Vector3& b) const {
    double a_val = plane.EvaluatePoint(a), b_val = plane.EvaluatePoint(b);
    
    double factor = a_val - b_val;
    if (NumericConstants::IsNearZero(factor))
        return 0.5; // Best effort, the line is (or almost) parallel to the plane, chose 0.5 for numerical stability.

    return a_val / factor;
}

template <typename ColorT>
inline void Engine::PlaneClippedGraphics3D<ColorT>::ComputeClippedTriangleGeometry(const Vertex3D<ColorT>& a, const Vertex3D<ColorT>& b, const Vertex3D<ColorT>& c, std::vector<Vertex3D<ColorT>>& out_vertices, std::vector<int>& out_triangles_indices) const {
    // Initialize
    out_vertices.clear();
    out_triangles_indices.clear();

    // Algorithm init
    std::vector<Vertex3D<ColorT>> polygon = {a, b, c};
    std::vector<Vertex3D<ColorT>> tmp;

    for (const Plane3D& plane : planes) {
        // No triangle can be form
        if (polygon.size() < 3) break;
    
        tmp.clear();
        for (int i = 0; i < polygon.size(); ++i) {
            Vertex3D<ColorT>& prev = polygon[(polygon.size() - 1 + i) % polygon.size()];
            Vertex3D<ColorT>& curr = polygon[i];

            bool prev_inside = __isInside(prev.GetPosition(), plane);
            bool curr_inside = __isInside(curr.GetPosition(), plane);

            // Exactly one of the vertex not inside, append the intersection point
            if (curr_inside != prev_inside) {
                
                tmp.push_back(prev); // prev here is just tmp, avoiding 'no default constructor for ColorT'.
                Vertex3D<ColorT>& intersection = tmp.back();

                double t = __computePlaneIntersection(plane, prev.GetPosition(), curr.GetPosition());
                t = std::max(0.0, std::min(1.0, t)); // Clamp t to [0, 1]

                intersection.x = prev.x + t * (curr.x - prev.x);
                intersection.y = prev.y + t * (curr.y - prev.y);
                intersection.z = prev.z + t * (curr.z - prev.z);
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
inline void Engine::PlaneClippedGraphics3D<ColorT>::RenderTriangle(const Vertex3D<ColorT>& a, const Vertex3D<ColorT>& b, const Vertex3D<ColorT>& c) {
    std::vector<Vertex3D<ColorT>> vertices;
    std::vector<int> triangles_indices;

    ComputeClippedTriangleGeometry(a, b, c, vertices, triangles_indices);

    Graphics3DWrapper<ColorT>::RenderGeometry(vertices, triangles_indices);
}

template <typename ColorT>
inline void Engine::PlaneClippedGraphics3D<ColorT>::RenderGeometry(const std::vector<Vertex3D<ColorT>>& vertices, const std::vector<int>& triangles_indices) {
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
