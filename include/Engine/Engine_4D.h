#pragma once

#include "Engine_3D.h"
#include "Engine_Vector.h"
#include "Engine_Constant.h"
#include "Engine_Matrix.h"
#include "Engine_Interpolation.h"

#include <array>
#include <functional>
#include <initializer_list>
#include <stack>
#include <vector>

namespace Engine {
    /// @brief 4D vertex with position and color payload.
    /// @tparam ColorT Vertex color/payload type.
    template <typename ColorT>
    struct Vertex4D {
        /// @brief X, Y, Z, W coordinates.
        double x = 0, y = 0, z = 0, w = 0;

        /// @brief Vertex color/payload.
        ColorT color;

        Vertex4D() = default;
        Vertex4D(double _xyzw) : x(_xyzw), y(_xyzw), z(_xyzw), w(_xyzw) {}
        Vertex4D(double _x, double _y, double _z, double _w) : x(_x), y(_y), z(_z), w(_w) {}
        Vertex4D(const Vector4& p) : x(p.x), y(p.y), z(p.z), w(p.w) {}
        Vertex4D(double _xyzw, const ColorT& _color) : x(_xyzw), y(_xyzw), z(_xyzw), w(_xyzw), color(_color) {}
        Vertex4D(double _x, double _y, double _z, double _w, const ColorT& _color) : x(_x), y(_y), z(_z), w(_w), color(_color) {}
        Vertex4D(const Vector4& p, const ColorT& _color) : x(p.x), y(p.y), z(p.z), w(p.w), color(_color) {}

        /// @brief Returns vertex position as Vector4.
        Vector4 GetPosition() const { return Vector4(x, y, z, w); }
    };

    /// @brief 4D triangle rendering interface.
    /// @tparam ColorT Vertex color/payload type.
    template <typename ColorT>
    struct IGraphics4D {
        /// @brief Virtual destructor.
        virtual ~IGraphics4D() = default;

        /// @brief Renders one 4D triangle.
        virtual void RenderTriangle(const Vertex4D<ColorT>& a, const Vertex4D<ColorT>& b, const Vertex4D<ColorT>& c) = 0;

        /// @brief Renders indexed 4D triangle geometry.
        virtual void RenderGeometry(const std::vector<Vertex4D<ColorT>>& vertices, const std::vector<int>& triangles_indices);
    };

    template <typename ColorTIn, typename ColorTOut>
    /// @brief Projects 4D vertices to 3D and forwards rendering.
    /// @tparam ColorTIn Input payload type.
    /// @tparam ColorTOut Output payload type.
    class Projected3DGraphics4D : public IGraphics4D<ColorTIn> {
    private:
        IGraphics3D<ColorTOut>& graphics;
        std::function<void(const Vertex4D<ColorTIn>&, Vertex3D<ColorTOut>&)> projector;
    public:
        /// @brief Constructs a 4D-to-3D projection adapter.
        Projected3DGraphics4D(IGraphics3D<ColorTOut>& _graphics, const std::function<void(const Vertex4D<ColorTIn>&, Vertex3D<ColorTOut>&)>& _projector)
            : graphics(_graphics), projector(_projector) {}

        /// @brief Projects one 4D vertex to 3D.
        void Project(const Vertex4D<ColorTIn>& in, Vertex3D<ColorTOut>& out) const;

        /// @brief Replaces projection callback.
        void SetProjector(const std::function<void(const Vertex4D<ColorTIn>&, Vertex3D<ColorTOut>&)>& _projector) { projector = _projector; }

        /// @brief Projects and renders one triangle.
        void RenderTriangle(const Vertex4D<ColorTIn>& a, const Vertex4D<ColorTIn>& b, const Vertex4D<ColorTIn>& c) override;

        /// @brief Projects and renders indexed geometry.
        void RenderGeometry(const std::vector<Vertex4D<ColorTIn>>& vertices, const std::vector<int>& triangles_indices) override;
    
        /// @brief Gets wrapped 3D graphics target (non-const).
        IGraphics3D<ColorTOut>& GetInternal2DGraphics() { return graphics; }

        /// @brief Gets wrapped 3D graphics target (const).
        const IGraphics3D<ColorTOut>& GetInternal2DGraphics() const { return graphics; }
    };

    /// @brief Alias for payload-preserving 4D-to-3D projection.
    template <typename ColorT>
    using PositionProjected3DGraphics4D = Projected3DGraphics4D<ColorT, ColorT>;

    /// @brief Embeds 3D vertices into 4D and forwards rendering.
    /// @tparam ColorTIn Input payload type.
    /// @tparam ColorTOut Output payload type.
    template <typename ColorTIn, typename ColorTOut>
    class Embed4DGraphics3D : public IGraphics3D<ColorTIn> {
    private:
        IGraphics4D<ColorTOut>& graphics;
        std::function<void(const Vertex3D<ColorTIn>&, Vertex4D<ColorTOut>&)> embedder;
    public:
        /// @brief Constructs a 3D-to-4D embedding adapter.
        Embed4DGraphics3D(IGraphics4D<ColorTOut>& _graphics, const std::function<void(const Vertex3D<ColorTIn>&, Vertex4D<ColorTOut>&)>& _embedder)
            : graphics(_graphics), embedder(_embedder) {}

        /// @brief Embeds one 3D vertex to 4D.
        void Embed(const Vertex3D<ColorTIn>& in, Vertex4D<ColorTOut>& out) const;

        /// @brief Replaces embedding callback.
        void SetEmbedder(const std::function<void(const Vertex3D<ColorTIn>&, Vertex4D<ColorTOut>&)>& _embedder) { embedder = _embedder; }

        /// @brief Embeds and renders one triangle.
        void RenderTriangle(const Vertex3D<ColorTIn>& a, const Vertex3D<ColorTIn>& b, const Vertex3D<ColorTIn>& c) override;

        /// @brief Embeds and renders indexed geometry.
        void RenderGeometry(const std::vector<Vertex3D<ColorTIn>>& vertices, const std::vector<int>& triangles_indices) override;
    
        /// @brief Gets wrapped 4D graphics target (non-const).
        IGraphics4D<ColorTOut>& GetInternal3DGraphics() { return graphics; }

        /// @brief Gets wrapped 4D graphics target (const).
        const IGraphics4D<ColorTOut>& GetInternal3DGraphics() const { return graphics; }
    };

    /// @brief Alias for payload-preserving 3D-to-4D embedding.
    template <typename ColorT>
    using PositionEmbed4DGraphics3D = Embed4DGraphics3D<ColorT, ColorT>;

    /// @brief Forwarding wrapper for IGraphics4D.
    /// @tparam ColorT Vertex color/payload type.
    template <typename ColorT>
    class Graphics4DWrapper : public IGraphics4D<ColorT> {
    private:
        IGraphics4D<ColorT>& graphics;
    public:
        /// @brief Constructs wrapper from another 4D graphics target.
        Graphics4DWrapper(IGraphics4D<ColorT>& _graphics) : graphics(_graphics) {}

        /// @brief Forwards triangle rendering.
        void RenderTriangle(const Vertex4D<ColorT>& a, const Vertex4D<ColorT>& b, const Vertex4D<ColorT>& c) override {
            return graphics.RenderTriangle(a, b, c);
        }

        /// @brief Forwards indexed geometry rendering.
        void RenderGeometry(const std::vector<Vertex4D<ColorT>>& vertices, const std::vector<int>& triangles_indices) override {
            return graphics.RenderGeometry(vertices, triangles_indices);
        }

        /// @brief Gets wrapped graphics target (non-const).
        IGraphics4D<ColorT>& GetInternalGraphics() { return graphics; }

        /// @brief Gets wrapped graphics target (const).
        const IGraphics4D<ColorT>& GetInternalGraphics() const { return graphics; }
    };

    template <typename ColorTIn, typename ColorTOut>
    /// @brief Converts 4D vertex payload before forwarding rendering.
    /// @tparam ColorTIn Input payload type.
    /// @tparam ColorTOut Output payload type.
    class ConvertedGraphics4D : public IGraphics4D<ColorTIn> {
    private:
        IGraphics4D<ColorTOut>& graphics;
        std::function<void(const Vertex4D<ColorTIn>&, Vertex4D<ColorTOut>&)> converter;
    public:
        /// @brief Constructs a converting 4D wrapper.
        ConvertedGraphics4D(IGraphics4D<ColorTOut>& _graphics, const std::function<void(const Vertex4D<ColorTIn>&, Vertex4D<ColorTOut>&)>& _converter) : graphics(_graphics), converter(_converter) {}

        /// @brief Converts and renders one triangle.
        void RenderTriangle(const Vertex4D<ColorTIn>& a, const Vertex4D<ColorTIn>& b, const Vertex4D<ColorTIn>& c) override {
            Vertex4D<ColorTOut> v_a, v_b, v_c;
            converter(a, v_a); converter(b, v_b); converter(c, v_c);

            graphics.RenderTriangle(v_a, v_b, v_c);
        }
        /// @brief Converts and renders indexed geometry.
        void RenderGeometry(const std::vector<Vertex4D<ColorTIn>>& vertices, const std::vector<int>& triangles_indices) override {
            std::vector<Vertex4D<ColorTOut>> converted_vertices(vertices.size());
            for (int i = 0; i < vertices.size(); ++i)
                converter(vertices[i], converted_vertices[i]);
            graphics.RenderGeometry(converted_vertices, triangles_indices);
        }
    };

    /// @brief Alias for payload-preserving 4D conversion wrapper.
    template <typename ColorT>
    using PositionConvertedGraphics4D = ConvertedGraphics4D<ColorT, ColorT>;

    /// @brief Applies a 4x4 matrix stack transform before rendering.
    /// @tparam ColorT Vertex color/payload type.
    template <typename ColorT>
    class MatrixGraphics4D : public Graphics4DWrapper<ColorT> {
    private:
        std::stack<Matrix4x4> matrix_stack;
    public:
        /// @brief Constructs matrix wrapper and initializes stack with identity.
		MatrixGraphics4D(IGraphics4D<ColorT>& _graphics) : Graphics4DWrapper<ColorT>(_graphics) {
			// Initialize matrix stack
			matrix_stack.push(Matrix4x4::GetIdentity(1));
		}

    	// --- Matrix Stack Management ---

        /// @brief Gets current matrix as raw array.
        void GetMatrixData(std::array<double, 16>& res) const;
        /// @brief Gets current matrix.
        void GetMatrixData(Matrix4x4& res) const;
		
        /// @brief Replaces current top matrix data.
		void SetMatrixData(const std::array<double, 16>& data);
        /// @brief Replaces current top matrix data.
		void SetMatrixData(const Matrix4x4& data);
				
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
		void MultiplyMatrix(const std::array<double, 16>& mat);
        /// @brief Right-multiplies current matrix.
		void MultiplyMatrix(const Matrix4x4& mat);
        /// @brief Generates then multiplies a matrix.
        void MultiplyMatrix(const std::function<void(std::array<double, 16>&)>& mat_gen);

		// --- Transformation ---
		
        /// @brief Applies 3D translation.
		void Translate3D(const Vector3& amount);
        /// @brief Applies 3D translation.
		void Translate3D(double x, double y, double z);

        /// @brief Applies 3D scaling.
		void Scale3D(const Vector3& amount);
        /// @brief Applies 3D scaling.
		void Scale3D(double x, double y, double z);

        /// @brief Applies 3D rotation.
		void Rotate3D(const Vector3& amount);
        /// @brief Applies 3D rotation.
		void Rotate3D(double x, double y, double z);

        /// @brief Applies combined 3D transformation.
		void Transform3D(const Vector3& translation, const Vector3& rotation, const Vector3& scale);

		// --- Projection ---

        /// @brief Applies frustum projection transform.
		void Frustum(double left, double right, double top, double bottom, double near, double far);
        /// @brief Applies perspective projection transform.
		void Perspective(double fovY, double aspect, double near, double far);
        /// @brief Applies orthographic projection transform.
		void Orthographic(double left, double right, double top, double bottom, double near, double far);

        // -- Utility --

		/// @brief Transforms one position by current matrix.
        void TransformPosition(const Vector4& in, Vector4& out) const;
		/// @brief Transforms one position by current matrix.
        Vector4 TransformPosition(const Vector4& in) const;

        // -- Main function --

		/// @brief Renders one transformed triangle.
        void RenderTriangle(const Vertex4D<ColorT>& a, const Vertex4D<ColorT>& b, const Vertex4D<ColorT>& c) override;
		/// @brief Renders transformed indexed geometry.
        void RenderGeometry(const std::vector<Vertex4D<ColorT>>& vertices, const std::vector<int>& triangles_indices) override;
    };

    /// @brief 4D plane represented by ax + by + cz + dw + e = 0.
    struct Plane4D {
        /// @brief Plane coefficients.
        double a = 0, b = 0, c = 0, d = 0, e = 0;
    
        /// @brief Default constructor.
        Plane4D() = default;
        /// @brief Constructs from plane coefficients.
        Plane4D(double _a, double _b, double _c, double _d, double _e) : a(_a), b(_b), c(_c), d(_d), e(_e) {}
        /// @brief Constructs from normal and a point on the plane.
        Plane4D(double _vx, double _vy, double _vz, double _vw, double _x0, double _y0, double _z0, double _w0) : a(_vx), b(_vy), c(_vz), d(_vw), e(-(_vx*_x0 + _vy*_y0 + _vz*_z0 + _vw*_w0)) {}
        /// @brief Constructs from normal vector and point.
        Plane4D(const Vector4& v, const Vector4& p) : Plane4D(v.x, v.y, v.z, v.w, p.x, p.y, p.z, p.w) {}

        /// @brief Compares two planes.
        bool operator==(const Plane4D& p) const { return a == p.a && b == p.b && c == p.c && d == p.d && e == p.e; }

        /// @brief Compares two planes.
        bool operator!=(const Plane4D& p) const { return !(*this == p); }

        /// @brief Evaluates plane equation at a point.
        double EvaluatePoint(const Vector4& p) const { return a * p.x + b * p.y + c * p.z + d * p.w + e; }
    };

    /// @brief Clips 4D triangles against one or more planes before rendering.
    /// @tparam ColorT Vertex color/payload type.
    template <typename ColorT>
    class PlaneClippedGraphics4D : public Graphics4DWrapper<ColorT> {
    private:
        std::vector<Plane4D> planes;
        IInterpolator<ColorT>& color_interpolator; // Use for clipping

        bool __isInside(const Vector4& p, const Plane4D& plane) const;
        double __computePlaneIntersection(const Plane4D& plane, const Vector4& a, const Vector4& b) const;
    public:
        /// @brief Constructs clipper with no initial planes.
        PlaneClippedGraphics4D(IGraphics4D<ColorT>& _graphics, IInterpolator<ColorT>& _color_interpolator)
            : Graphics4DWrapper<ColorT>(_graphics), color_interpolator(_color_interpolator) {}

        /// @brief Constructs clipper with initial planes.
        PlaneClippedGraphics4D(IGraphics4D<ColorT>& _graphics, IInterpolator<ColorT>& _color_interpolator, std::initializer_list<Plane4D> _planes)
            : Graphics4DWrapper<ColorT>(_graphics), color_interpolator(_color_interpolator), planes(_planes) {}
        
        /// @brief Replaces clip planes.
        void SetClipPlanes(const std::vector<Plane4D>& _planes) { planes = _planes; }

        /// @brief Replaces clip planes.
        void SetClipPlanes(const std::initializer_list<Plane4D>& _planes) { planes = _planes; }

        /// @brief Computes clipped geometry for a triangle.
        void ComputeClippedTriangleGeometry(const Vertex4D<ColorT>& a, const Vertex4D<ColorT>& b, const Vertex4D<ColorT>& c, std::vector<Vertex4D<ColorT>>& out_vertices, std::vector<int>& out_triangles_indices) const;

        /// @brief Clips and renders one triangle.
        void RenderTriangle(const Vertex4D<ColorT>& a, const Vertex4D<ColorT>& b, const Vertex4D<ColorT>& c) override;

        /// @brief Clips and renders indexed geometry.
        void RenderGeometry(const std::vector<Vertex4D<ColorT>>& vertices, const std::vector<int>& triangles_indices) override;
    };
}

// -- IGraphics4D --

template <typename ColorT>
inline void Engine::IGraphics4D<ColorT>::RenderGeometry(const std::vector<Vertex4D<ColorT>>& vertices, const std::vector<int>& triangles_indices) {
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

// -- Projected3DGraphics4D --

template <typename ColorTIn, typename ColorTOut>
inline void Engine::Projected3DGraphics4D<ColorTIn, ColorTOut>::Project(const Vertex4D<ColorTIn>& in, Vertex3D<ColorTOut>& out) const {
    if (projector) projector(in, out);
    else { out.x = in.x; out.y = in.y; out.color = in.color; }
}
template <typename ColorTIn, typename ColorTOut>
inline void Engine::Projected3DGraphics4D<ColorTIn, ColorTOut>::RenderTriangle(const Vertex4D<ColorTIn>& a, const Vertex4D<ColorTIn>& b, const Vertex4D<ColorTIn>& c) {
    Vertex3D<ColorTOut> va, vb, vc;
    Project(a, va); Project(b, vb); Project(c, vc);

    return graphics.RenderTriangle(va, vb, vc);
}
template <typename ColorTIn, typename ColorTOut>
inline void Engine::Projected3DGraphics4D<ColorTIn, ColorTOut>::RenderGeometry(const std::vector<Vertex4D<ColorTIn>>& vertices, const std::vector<int>& triangles_indices) {
    std::vector<Vertex3D<ColorTOut>> vert_3d(vertices.size());
    for (int i = 0; i < vertices.size(); ++i)
        Project(vertices[i], vert_3d[i]);

    return graphics.RenderGeometry(vert_3d, triangles_indices);
}

// -- Embed4DGraphics3D --

template <typename ColorTIn, typename ColorTOut>
inline void Engine::Embed4DGraphics3D<ColorTIn, ColorTOut>::Embed(const Vertex3D<ColorTIn>& in, Vertex4D<ColorTOut>& out) const {
    if (embedder) embedder(in, out);
    else { out.x = in.x; out.y = in.y; out.z = 0; out.color = in.color; }
}
template <typename ColorTIn, typename ColorTOut>
inline void Engine::Embed4DGraphics3D<ColorTIn, ColorTOut>::RenderTriangle(const Vertex3D<ColorTIn>& a, const Vertex3D<ColorTIn>& b, const Vertex3D<ColorTIn>& c) {
    Vertex4D<ColorTOut> va, vb, vc;
    Embed(a, va); Embed(b, vb); Embed(c, vc);

    return graphics.RenderTriangle(va, vb, vc);
}
template <typename ColorTIn, typename ColorTOut>
inline void Engine::Embed4DGraphics3D<ColorTIn, ColorTOut>::RenderGeometry(const std::vector<Vertex3D<ColorTIn>>& vertices, const std::vector<int>& triangles_indices) {
    std::vector<Vertex4D<ColorTOut>> vert_4d(vertices.size());
    for (int i = 0; i < vertices.size(); ++i)
        Embed(vertices[i], vert_4d[i]);

    return graphics.RenderGeometry(vert_4d, triangles_indices);
}

// -- MatrixGraphics4D --

template <typename ColorT>
inline void Engine::MatrixGraphics4D<ColorT>::GetMatrixData(std::array<double, 16>& res) const {
    // Note that this is unexpected, the matrix_stack size are always bigger than 1. But handle just in case.
    if (matrix_stack.empty()) Matrix4x4::GetIdentity(res, 1);
    else res = matrix_stack.top().Data();
}
template <typename ColorT>
inline void Engine::MatrixGraphics4D<ColorT>::GetMatrixData(Matrix4x4& res) const { return GetMatrixData(res.Data()); }

// Only use this if you know what you're doing, will replaced the top matrix data with the given data
template <typename ColorT>
inline void Engine::MatrixGraphics4D<ColorT>::SetMatrixData(const std::array<double, 16>& data) { return SetMatrixData(Matrix4x4(data)); }
template <typename ColorT>
inline void Engine::MatrixGraphics4D<ColorT>::SetMatrixData(const Matrix4x4& data) {
    if (matrix_stack.empty()) matrix_stack.push(data);
    else matrix_stack.top() = data;
}
        
template <typename ColorT>
inline void Engine::MatrixGraphics4D<ColorT>::PushMatrix() { matrix_stack.push(matrix_stack.top()); }
template <typename ColorT>
inline void Engine::MatrixGraphics4D<ColorT>::PopMatrix() {
    // This case is unexpected and not supposed to happened, but handle just in case
    if (matrix_stack.empty()) {
        matrix_stack.push(Matrix4x4::GetIdentity(1));
        return;
    }
    // If there's only 1 matrix, set it to identity.
    if (matrix_stack.size() == 1) {
        Matrix4x4::GetIdentity(matrix_stack.top(), 1);
        return;
    }

    // Otherwise, pop it.
    matrix_stack.pop();
}

template <typename ColorT>
inline void Engine::MatrixGraphics4D<ColorT>::LoadIdentity(bool replace) {
    if (matrix_stack.empty()) {
        matrix_stack.push(Matrix4x4::GetIdentity(1));
        return;
    }

    if (replace || matrix_stack.size() == 1) {
        Matrix4x4::GetIdentity(matrix_stack.top(), 1);
        return;
    }

    // Pop + Re-push matrix
    matrix_stack.pop();
    matrix_stack.push(matrix_stack.top());
}

template <typename ColorT>
inline void Engine::MatrixGraphics4D<ColorT>::MultiplyMatrix(const std::array<double, 16>& mat) {
    if (matrix_stack.empty()) {
        matrix_stack.push(Matrix4x4(mat));
        return;
    }

    matrix_stack.top() *= mat;
}
template <typename ColorT>
inline void Engine::MatrixGraphics4D<ColorT>::MultiplyMatrix(const Matrix4x4& mat) { return MultiplyMatrix(mat.Data()); }
template <typename ColorT>
inline void Engine::MatrixGraphics4D<ColorT>::MultiplyMatrix(const std::function<void(std::array<double, 16>&)>& mat_gen) {
    std::array<double, 16> _tmp;
    mat_gen(_tmp);
    MultiplyMatrix(_tmp);
}

template <typename ColorT>
inline void Engine::MatrixGraphics4D<ColorT>::Translate3D(const Vector3& amount) {
    std::array<double, 16> _tmp;
    Matrix4x4::GetTranslation3D(_tmp, amount);
    MultiplyMatrix(_tmp);
}
template <typename ColorT>
inline void Engine::MatrixGraphics4D<ColorT>::Translate3D(double x, double y, double z) { return Translate3D(Vector3(x, y, z)); }

template <typename ColorT>
inline void Engine::MatrixGraphics4D<ColorT>::Scale3D(const Vector3& amount) {
    std::array<double, 16> _tmp;
    Matrix4x4::GetScale3D(_tmp, amount);
    MultiplyMatrix(_tmp);
}
template <typename ColorT>
inline void Engine::MatrixGraphics4D<ColorT>::Scale3D(double x, double y, double z) { return Scale3D(Vector3(x, y, z)); }

template <typename ColorT>
inline void Engine::MatrixGraphics4D<ColorT>::Rotate3D(const Vector3& amount) {
    std::array<double, 16> _tmp;
    Matrix4x4::GetRotation3D(_tmp, amount);
    MultiplyMatrix(_tmp);
}
template <typename ColorT>
inline void Engine::MatrixGraphics4D<ColorT>::Rotate3D(double x, double y, double z) { return Rotate3D(Vector3(x, y, z)); }

template <typename ColorT>
inline void Engine::MatrixGraphics4D<ColorT>::Transform3D(const Vector3& translation, const Vector3& rotation, const Vector3& scale) {
    std::array<double, 16> _tmp;
    Matrix4x4::GetTransformation3D(_tmp, translation, rotation, scale);
    MultiplyMatrix(_tmp);
}

template <typename ColorT>
inline void Engine::MatrixGraphics4D<ColorT>::Frustum(double left, double right, double top, double bottom, double near, double far) {
    std::array<double, 16> _tmp;
    Matrix4x4::GetFrustumProjection(_tmp, left, right, top, bottom, near, far);
    MultiplyMatrix(_tmp);
}
template <typename ColorT>
inline void Engine::MatrixGraphics4D<ColorT>::Perspective(double fovY, double aspect, double near, double far) {
    std::array<double, 16> _tmp;
    Matrix4x4::GetPerspectiveProjection(_tmp, fovY, aspect, near, far);
    MultiplyMatrix(_tmp);
}
template <typename ColorT>
inline void Engine::MatrixGraphics4D<ColorT>::Orthographic(double left, double right, double top, double bottom, double near, double far) {
    std::array<double, 16> _tmp;
    Matrix4x4::GetOrthographicProjection(_tmp, left, right, top, bottom, near, far);
    MultiplyMatrix(_tmp);
}

template <typename ColorT>
inline void Engine::MatrixGraphics4D<ColorT>::TransformPosition(const Vector4& in, Vector4& out) const {
    out = matrix_stack.empty() ? in : (matrix_stack.top() * in);
}
template <typename ColorT>
inline Engine::Vector4 Engine::MatrixGraphics4D<ColorT>::TransformPosition(const Vector4& in) const {
    Vector4 out = in;
    TransformPosition(in, out);
    return out;
}

template <typename ColorT>
inline void Engine::MatrixGraphics4D<ColorT>::RenderTriangle(const Vertex4D<ColorT>& a, const Vertex4D<ColorT>& b, const Vertex4D<ColorT>& c) {
    // Compute new vertices (aka compute new position)
    Vertex4D<ColorT> v_a(a), v_b(b), v_c(c);
    Vector4 pa, pb, pc;
    TransformPosition(a.GetPosition(), pa);
    TransformPosition(b.GetPosition(), pb);
    TransformPosition(c.GetPosition(), pc);

    v_a.x = pa.x; v_a.y = pa.y; v_a.z = pa.z; v_a.w = pa.w;
    v_b.x = pb.x; v_b.y = pb.y; v_b.z = pb.z; v_b.w = pb.w;
    v_c.x = pc.x; v_c.y = pc.y; v_c.z = pc.z; v_c.w = pc.w;

    return Graphics4DWrapper<ColorT>::RenderTriangle(v_a, v_b, v_c);
}
template <typename ColorT>
inline void Engine::MatrixGraphics4D<ColorT>::RenderGeometry(const std::vector<Vertex4D<ColorT>>& vertices, const std::vector<int>& triangles_indices) {
    std::vector<Vertex4D<ColorT>> tmp_v(vertices);
    for (Vertex4D<ColorT>& v : tmp_v) {
        Vector4 p;
        TransformPosition(v.GetPosition(), p);
        v.x = p.x; v.y = p.y; v.z = p.z; v.w = p.w;
    }
    
    return Graphics4DWrapper<ColorT>::RenderGeometry(tmp_v, triangles_indices);
}

// -- PlaneClippedGraphics4D --

template <typename ColorT>
inline bool Engine::PlaneClippedGraphics4D<ColorT>::__isInside(const Vector4& p, const Plane4D& plane) const {
    return NumericConstants::IsNearZeroOrPositive(plane.EvaluatePoint(p));
}
template <typename ColorT>
inline double Engine::PlaneClippedGraphics4D<ColorT>::__computePlaneIntersection(const Plane4D& plane, const Vector4& a, const Vector4& b) const {
    double a_val = plane.EvaluatePoint(a), b_val = plane.EvaluatePoint(b);
    
    double factor = a_val - b_val;
    if (NumericConstants::IsNearZero(factor))
        return 0.5; // Best effort, the line is (or almost) parallel to the plane, chose 0.5 for numerical stability.

    return a_val / factor;
}

template <typename ColorT>
inline void Engine::PlaneClippedGraphics4D<ColorT>::ComputeClippedTriangleGeometry(const Vertex4D<ColorT>& a, const Vertex4D<ColorT>& b, const Vertex4D<ColorT>& c, std::vector<Vertex4D<ColorT>>& out_vertices, std::vector<int>& out_triangles_indices) const {
    // Initialize
    out_vertices.clear();
    out_triangles_indices.clear();

    // Algorithm init
    std::vector<Vertex4D<ColorT>> polygon = {a, b, c};
    std::vector<Vertex4D<ColorT>> tmp;

    for (const Plane4D& plane : planes) {
        // No triangle can be form
        if (polygon.size() < 3) break;
    
        tmp.clear();
        for (int i = 0; i < polygon.size(); ++i) {
            Vertex4D<ColorT>& prev = polygon[(polygon.size() - 1 + i) % polygon.size()];
            Vertex4D<ColorT>& curr = polygon[i];

            bool prev_inside = __isInside(prev.GetPosition(), plane);
            bool curr_inside = __isInside(curr.GetPosition(), plane);

            // Exactly one of the vertex not inside, append the intersection point
            if (curr_inside != prev_inside) {
                
                tmp.push_back(prev); // prev here is just tmp, avoiding 'no default constructor for ColorT'.
                Vertex4D<ColorT>& intersection = tmp.back();

                double t = __computePlaneIntersection(plane, prev.GetPosition(), curr.GetPosition());
                t = std::max(0.0, std::min(1.0, t)); // Clamp t to [0, 1]

                intersection.x = prev.x + t * (curr.x - prev.x);
                intersection.y = prev.y + t * (curr.y - prev.y);
                intersection.z = prev.z + t * (curr.z - prev.z);
                intersection.w = prev.w + t * (curr.w - prev.w);
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
inline void Engine::PlaneClippedGraphics4D<ColorT>::RenderTriangle(const Vertex4D<ColorT>& a, const Vertex4D<ColorT>& b, const Vertex4D<ColorT>& c) {
    std::vector<Vertex4D<ColorT>> vertices;
    std::vector<int> triangles_indices;

    ComputeClippedTriangleGeometry(a, b, c, vertices, triangles_indices);

    Graphics4DWrapper<ColorT>::RenderGeometry(vertices, triangles_indices);
}

template <typename ColorT>
inline void Engine::PlaneClippedGraphics4D<ColorT>::RenderGeometry(const std::vector<Vertex4D<ColorT>>& vertices, const std::vector<int>& triangles_indices) {
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
