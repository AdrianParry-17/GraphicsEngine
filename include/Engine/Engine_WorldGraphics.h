#pragma once

#include "Engine_Matrix.h"
#include "Engine_Interpolation.h"
#include "Engine_Range.h"

#include <vector>
#include <stack>
#include <type_traits>
#include <functional>

namespace Engine {
    /**
     * @brief A vertex in D-dimensional world space, holding a position and a color.
     * @tparam ColorT The color type associated with the vertex.
     * @tparam D The dimensionality of the vertex position.
     */
    template <typename ColorT, size_t D>
    struct WorldVertex {
        /**
         * @brief The dimensionality of the vertex position.
         */
        static const size_t Dimension = D;

        /**
         * @brief The position of the vertex in world space.
         */
        Vector<Dimension> position;
        /**
         * @brief The color associated with the vertex.
         */
        ColorT color;

        /** @brief Default constructor. */
        WorldVertex() = default;
        /**
         * @brief Constructor with position only.
         * @param _position The world-space position.
         */
        WorldVertex(const Vector<Dimension>& _position) : position(_position) {}
        /**
         * @brief Constructor with color only.
         * @param _color The vertex color.
         */
        WorldVertex(const ColorT& _color) : color(_color) {}
        /**
         * @brief Constructor with position and color.
         * @param _position The world-space position.
         * @param _color The vertex color.
         */
        WorldVertex(const Vector<Dimension>& _position, const ColorT& _color) : position(_position), color(_color) {}
    };

    /**
     * @brief Abstract interface for a D-dimensional world graphics renderer.
     * @tparam ColorT The color type used by vertices.
     * @tparam D The dimensionality of the world space.
     */
    template <typename ColorT, size_t D>
    struct IWorldGraphics {
        /**
         * @brief The dimensionality of the world space.
         */
        static const size_t Dimension = D;

        /** @brief Virtual destructor. */
        virtual ~IWorldGraphics() = default;

        /**
         * @brief Renders a single triangle defined by three vertices.
         * @param a The first vertex.
         * @param b The second vertex.
         * @param c The third vertex.
         */
        virtual void RenderTriangle(const WorldVertex<ColorT, D>& a, const WorldVertex<ColorT, D>& b, const WorldVertex<ColorT, D>& c) = 0;
        /**
         * @brief Renders geometry defined by a vertex list and a triangle index list.
         * @param vertices The list of vertices.
         * @param triangles_indices The indices into the vertex list, in groups of three per triangle.
         */
        virtual void RenderGeometry(const std::vector<WorldVertex<ColorT, D>>& vertices, const std::vector<int>& triangles_indices);
    };

    /**
     * @brief A wrapper around an IWorldGraphics instance that forwards all calls to it.
     *        Useful as a base class for graphics decorators.
     * @tparam ColorT The color type used by vertices.
     * @tparam D The dimensionality of the world space.
     */
    template <typename ColorT, size_t D>
    class WorldGraphicsWrapper : public IWorldGraphics<ColorT, D> {
    private:
        IWorldGraphics<ColorT, D>& graphics;
    public:
        /**
         * @brief Constructor.
         * @param _graphics The underlying graphics interface to wrap.
         */
        WorldGraphicsWrapper(IWorldGraphics<ColorT, D>& _graphics) : graphics(_graphics) {}

        void RenderTriangle(const WorldVertex<ColorT, D>& a, const WorldVertex<ColorT, D>& b, const WorldVertex<ColorT, D>& c) override {
            return graphics.RenderTriangle(a, b, c);
        }
        void RenderGeometry(const std::vector<WorldVertex<ColorT, D>>& vertices, const std::vector<int>& triangles_indices) override {
            return graphics.RenderGeometry(vertices, triangles_indices);
        }

        /**
         * @brief Returns a reference to the wrapped graphics interface.
         * @return A reference to the internal IWorldGraphics.
         */
        IWorldGraphics<ColorT, D>& GetInternalGraphics() { return graphics; }
        /**
         * @brief Returns a const reference to the wrapped graphics interface.
         * @return A const reference to the internal IWorldGraphics.
         */
        const IWorldGraphics<ColorT, D>& GetInternalGraphics() const { return graphics; }
    };

    /**
     * @brief An IWorldGraphics implementation that dispatches RenderTriangle to a user-provided function.
     * @tparam ColorT The color type used by vertices.
     * @tparam D The dimensionality of the world space.
     */
    template <typename ColorT, size_t D>
    class FunctionWorldGraphics : public IWorldGraphics<ColorT, D> {
    private:
        std::function<void(const WorldVertex<ColorT, D>&, const WorldVertex<ColorT, D>&, const WorldVertex<ColorT, D>&)> func;
    public:
        /**
         * @brief Constructor.
         * @param _func The function to call when rendering a triangle.
         */
        FunctionWorldGraphics(const std::function<void(const WorldVertex<ColorT, D>&, const WorldVertex<ColorT, D>&, const WorldVertex<ColorT, D>&)>& _func) : func(_func) {}
        
        /**
         * @brief Replaces the rendering function.
         * @param _func The new function to use for rendering triangles.
         */
        void SetFunction(const std::function<void(const WorldVertex<ColorT, D>&, const WorldVertex<ColorT, D>&, const WorldVertex<ColorT, D>&)>& _func) { func = _func; }
        
        void RenderTriangle(const WorldVertex<ColorT, D>& a, const WorldVertex<ColorT, D>& b, const WorldVertex<ColorT, D>& c) override { func(a, b, c); }
    };

    /**
     * @brief An IWorldGraphics decorator that converts each vertex from one type/dimension to another
     *        before forwarding to an inner graphics interface.
     * @tparam ColorTIn The input color type.
     * @tparam DIn The input dimensionality.
     * @tparam ColorTOut The output color type.
     * @tparam DOut The output dimensionality.
     */
    template <typename ColorTIn, size_t DIn, typename ColorTOut, size_t DOut>
    class VertexConvertWorldGraphics : public IWorldGraphics<ColorTIn, DIn> {
    private:
        IWorldGraphics<ColorTOut, DOut>& graphics;
        std::function<void(const WorldVertex<ColorTIn, DIn>&, WorldVertex<ColorTOut, DOut>&)> converter;
    public:
        /**
         * @brief Constructor.
         * @param _graphics The output graphics interface to forward converted vertices to.
         * @param _converter A function that converts an input vertex to an output vertex.
         */
        VertexConvertWorldGraphics(IWorldGraphics<ColorTOut, DOut>& _graphics, const std::function<void(const WorldVertex<ColorTIn, DIn>&, WorldVertex<ColorTOut, DOut>&)>& _converter) : graphics(_graphics), converter(_converter) {}
    
        void RenderTriangle(const WorldVertex<ColorTIn, DIn>& a, const WorldVertex<ColorTIn, DIn>& b, const WorldVertex<ColorTIn, DIn>& c) override {
            WorldVertex<ColorTOut, DOut> v_a, v_b, v_c;
            converter(a, v_a); converter(b, v_b); converter(c, v_c);

            graphics.RenderTriangle(v_a, v_b, v_c);
        }
        void RenderGeometry(const std::vector<WorldVertex<ColorTIn, DIn>>& vertices, const std::vector<int>& triangles_indices) override {
            std::vector<WorldVertex<ColorTOut, DOut>> converted_vertices(vertices.size());
            for (int i = 0; i < vertices.size(); ++i)
                converter(vertices[i], converted_vertices[i]);
            graphics.RenderGeometry(converted_vertices, triangles_indices);
        }
    };

    /**
     * @brief Alias for a vertex converter that only changes the color type, keeping the dimension the same.
     */
    template <typename ColorTIn, typename ColorTOut, size_t D>
    using VertexColorConvertWorldGraphics = VertexConvertWorldGraphics<ColorTIn, D, ColorTOut, D>;
    /**
     * @brief Alias for a vertex converter that only changes the dimension, keeping the color type the same.
     */
    template <size_t DIn, size_t DOut, typename ColorT>
    using VertexDimensionConvertWorldGraphics = VertexConvertWorldGraphics<ColorT, DIn, ColorT, DOut>;
    /**
     * @brief Alias for a vertex converter that keeps both color type and dimension the same (i.e. a vertex modifier).
     */
    template <typename ColorT, size_t D>
    using VertexModifierWorldGraphics = VertexConvertWorldGraphics<ColorT, D, ColorT, D>;

    /**
     * @brief A WorldGraphicsWrapper that applies a matrix transformation to vertex positions before rendering.
     *        Maintains an internal matrix stack for managing hierarchical transforms.
     * @tparam ColorT The color type used by vertices.
     * @tparam D The dimensionality of the world space.
     */
    template <typename ColorT, size_t D>
    class MatrixWorldGraphics : public WorldGraphicsWrapper<ColorT, D> {
    public:
        /**
         * @brief The total number of elements in the transformation matrix.
         */
        static const size_t MatrixCellCount = SquareMatrix<D>::CellCount;
    private:
        std::stack<SquareMatrix<D>> matrix_stack;
    public:
		/**
		 * @brief Constructor. Initializes the matrix stack with an identity matrix.
		 * @param _graphics The underlying graphics interface to render to.
		 */
		MatrixWorldGraphics(IWorldGraphics<ColorT, D>& _graphics) : WorldGraphicsWrapper<ColorT, D>(_graphics) {
			matrix_stack.push(SquareMatrix<D>::GetIdentity(1));
		}

		// --- Matrix Stack Management ---

		/**
		 * @brief Gets the current top-of-stack matrix data into an array.
		 * @param res The array to write the matrix data into.
		 */
        void GetMatrixData(std::array<double, MatrixCellCount>& res) const;
		/**
		 * @brief Gets the current top-of-stack matrix data into a SquareMatrix.
		 * @param res The matrix to write into.
		 */
        void GetMatrixData(SquareMatrix<D>& res) const;
		
		/**
		 * @brief Replaces the top-of-stack matrix with the given array data.
		 * @param data The raw matrix data to set.
		 */
		void SetMatrixData(const std::array<double, MatrixCellCount>& data);
		/**
		 * @brief Replaces the top-of-stack matrix with the given matrix.
		 * @param data The matrix to set.
		 */
		void SetMatrixData(const SquareMatrix<D>& data);
		
		/**
		 * @brief Pushes a copy of the current matrix onto the stack.
		 */
		void PushMatrix();
		/**
		 * @brief Pops the top matrix off the stack.
		 *        If only one matrix remains it is reset to identity instead of being removed.
		 */
		void PopMatrix();

		/**
		 * @brief Loads the previous matrix from the stack into the current slot, or loads identity
		 *        if this is the only matrix on the stack.
		 * @param replace If true, the current matrix is replaced with the identity matrix regardless.
		 */
		void LoadIdentity(bool replace = false);
        
		/**
		 * @brief Post-multiplies the current matrix by the given array.
		 * @param mat The raw matrix data to multiply by.
		 */
		void MultiplyMatrix(const std::array<double, MatrixCellCount>& mat);
		/**
		 * @brief Post-multiplies the current matrix by the given SquareMatrix.
		 * @param mat The matrix to multiply by.
		 */
		void MultiplyMatrix(const SquareMatrix<D>& mat);
		/**
		 * @brief Post-multiplies the current matrix by the matrix generated by the given function.
		 * @param mat_gen A function that fills a matrix array with the desired transform.
		 */
        void MultiplyMatrix(const std::function<void(std::array<double, MatrixCellCount>&)>& mat_gen);
		
        // --- Utility ---

		/**
		 * @brief Transforms a position vector by the current top-of-stack matrix.
		 * @param in The input position.
		 * @param out The transformed output position.
		 */
        void TransformPosition(const Vector<D>& in, Vector<D>& out) const;
		/**
		 * @brief Transforms a position vector by the current top-of-stack matrix.
		 * @param in The input position.
		 * @return The transformed position.
		 */
        Vector<D> TransformPosition(const Vector<D>& in) const;

        void RenderTriangle(const WorldVertex<ColorT, D>& a, const WorldVertex<ColorT, D>& b, const WorldVertex<ColorT, D>& c) override;
        void RenderGeometry(const std::vector<WorldVertex<ColorT, D>>& vertices, const std::vector<int>& triangles_indices) override;
    };

    /**
     * @brief Represents a hyperplane in D-dimensional space, defined by the equation:
     *        v_x1*x1 + v_x2*x2 + ... + v_xD*xD + c = 0
     * @tparam D The dimensionality of the space.
     */
    template <size_t D>
    struct WorldPlane {
        /**
         * @brief The normal vector of the plane.
         */
        Vector<D> vector;
        /**
         * @brief The constant term of the plane equation.
         */
        double c;

        /** @brief Default constructor. */
        WorldPlane() = default;
        /**
         * @brief Constructor from a normal vector and an optional constant.
         * @param _vector The normal vector.
         * @param _c The constant term (default 0).
         */
        WorldPlane(const Vector<D>& _vector, double _c = 0.0) : vector(_vector), c(_c) {}
        /**
         * @brief Constructor from a normal array and an optional constant.
         * @param _vector The normal vector as an array.
         * @param _c The constant term (default 0).
         */
        WorldPlane(const std::array<double, D>& _vector, double _c = 0.0) : vector(_vector), c(_c) {}
        /**
         * @brief Constructor from a normal vector and a point on the plane.
         * @param _vector The normal vector.
         * @param _p A point on the plane.
         */
        WorldPlane(const Vector<D>& _vector, const Vector<D>& _p) : vector(_vector), c(-_vector.DotProductWith(_p)) {}
        /**
         * @brief Constructor from a normal array and a point array on the plane.
         * @param _vector The normal vector as an array.
         * @param _p A point on the plane as an array.
         */
        WorldPlane(const std::array<double, D>& _vector, const std::array<double, D>& _p) : vector(_vector), c(-Vector<D>::DotProduct(_vector, _p)) {}
        /** @brief Component constructor for 1D planes. */
        template <size_t _D = D>
        WorldPlane(double _x, typename std::enable_if<(_D == D) && (_D == 1), double>::type _c) : vector(_x), c(_c) {}
        /** @brief Component constructor for 2D planes. */
        template <size_t _D = D>
        WorldPlane(double _x, double _y, typename std::enable_if<(_D == D) && (_D == 2), double>::type _c) : vector(_x, _y), c(_c) {}
        /** @brief Component constructor for 3D planes. */
        template <size_t _D = D>
        WorldPlane(double _x, double _y, double _z, typename std::enable_if<(_D == D) && (_D == 3), double>::type _c) : vector(_x, _y, _z), c(_c) {}
        /** @brief Component constructor for 4D planes. */
        template <size_t _D = D>
        WorldPlane(double _x, double _y, double _z, double _w, typename std::enable_if<(_D == D) && (_D == 4), double>::type _c) : vector(_x, _y, _z, _w), c(_c) {}

        bool operator==(const WorldPlane<D>& wp) const { return vector == wp.vector && c == wp.c; }
        bool operator!=(const WorldPlane<D>& wp) const { return !operator==(wp); }

        /**
         * @brief Checks whether this plane is identical to another within a small tolerance.
         * @param wp The plane to compare against.
         * @return True if identical, false otherwise.
         */
        bool IsIdentical(const WorldPlane<D>& wp) const { return vector.IsIdentical(wp.vector) && NumericConstants::IsNearZero(c - wp.c); }

        /**
         * @brief Evaluates the plane equation at a given point.
         *        A positive result means the point is on the inside (positive half-space).
         * @param p The point to evaluate.
         * @return The signed distance from the plane.
         */
        double EvaluatePoint(const Vector<D>& p) const { return vector.DotProductWith(p) + c; }
    };

    /**
     * @brief A WorldGraphicsWrapper that clips triangles against a set of half-space planes
     *        before forwarding the resulting geometry for rendering.
     * @tparam ColorT The color type used by vertices.
     * @tparam D The dimensionality of the world space.
     */
    template <typename ColorT, size_t D>
    class PlaneClipWorldGraphics : public WorldGraphicsWrapper<ColorT, D> {
    private:
        std::vector<WorldPlane<D>> planes;
        IInterpolator<ColorT>& color_interpolator;

        bool __isInside(const Vector<D>& p, const WorldPlane<D>& plane) const;
        double __computePlaneIntersection(const WorldPlane<D>& plane, const Vector<D>& a, const Vector<D>& b) const;
    public:
        /**
         * @brief Constructor.
         * @param _graphics The underlying graphics interface to render clipped geometry to.
         * @param _color_interpolator The interpolator used to blend vertex colors at clip intersections.
         */
        PlaneClipWorldGraphics(IWorldGraphics<ColorT, D>& _graphics, IInterpolator<ColorT>& _color_interpolator)
            : WorldGraphicsWrapper<ColorT, D>(_graphics), color_interpolator(_color_interpolator) {}

        /**
         * @brief Constructor with an initial set of clip planes.
         * @param _graphics The underlying graphics interface to render clipped geometry to.
         * @param _color_interpolator The interpolator used to blend vertex colors at clip intersections.
         * @param _planes The initial set of clip planes.
         */
        PlaneClipWorldGraphics(IWorldGraphics<ColorT, D>& _graphics, IInterpolator<ColorT>& _color_interpolator, std::initializer_list<WorldPlane<D>> _planes)
            : WorldGraphicsWrapper<ColorT, D>(_graphics), color_interpolator(_color_interpolator), planes(_planes) {}
        
        /**
         * @brief Replaces the full set of clip planes.
         * @param _planes The new clip planes.
         */
        void SetClipPlanes(const std::vector<WorldPlane<D>>& _planes) { planes = _planes; }
        /**
         * @brief Replaces the full set of clip planes.
         * @param _planes The new clip planes.
         */
        void SetClipPlanes(const std::initializer_list<WorldPlane<D>>& _planes) { planes = _planes; }

        /**
         * @brief Computes the clipped polygon geometry for a given triangle against all clip planes.
         *        The result is a triangle-fan ready for rendering.
         * @param a The first input vertex.
         * @param b The second input vertex.
         * @param c The third input vertex.
         * @param out_vertices The output clipped vertex list.
         * @param out_triangles_indices The output triangle index list.
         */
        void ComputeClippedTriangleGeometry(const WorldVertex<ColorT, D>& a, const WorldVertex<ColorT, D>& b, const WorldVertex<ColorT, D>& c, std::vector<WorldVertex<ColorT, D>>& out_vertices, std::vector<int>& out_triangles_indices) const;

        void RenderTriangle(const WorldVertex<ColorT, D>& a, const WorldVertex<ColorT, D>& b, const WorldVertex<ColorT, D>& c) override;
        void RenderGeometry(const std::vector<WorldVertex<ColorT, D>>& vertices, const std::vector<int>& triangles_indices) override;
    };

    
    /**
     * @brief A WorldGraphicsWrapper that applies a per-axis range mapping (viewport transform)
     *        to vertex positions before forwarding to the inner graphics interface.
     * @tparam ColorT The color type used by vertices.
     * @tparam D The dimensionality of the world space.
     */
    template <typename ColorT, size_t D>
    class ViewportTransformWorldGraphics : public WorldGraphicsWrapper<ColorT, D> {
    private:
        std::array<RangeMapper, D> mapper;
    public:
        /**
         * @brief Constructor with default (identity) mappers.
         * @param _graphics The underlying graphics interface.
         */
        ViewportTransformWorldGraphics(IWorldGraphics<ColorT, D>& _graphics) : WorldGraphicsWrapper<ColorT, D>(_graphics) {}
        /**
         * @brief Constructor with an explicit set of per-axis mappers.
         * @param _graphics The underlying graphics interface.
         * @param _mapper An array of RangeMapper, one per axis.
         */
        ViewportTransformWorldGraphics(IWorldGraphics<ColorT, D>& _graphics, const std::array<RangeMapper, D>& _mapper) :
            WorldGraphicsWrapper<ColorT, D>(_graphics), mapper(_mapper) {}

        /**
         * @brief Returns the range mapper for a specific axis.
         * @param d The axis index.
         * @return A reference to the RangeMapper for axis d.
         */
        RangeMapper& GetMapper(size_t d) { return mapper[d]; }
        /**
         * @brief Returns the range mapper for a specific axis as const.
         * @param d The axis index.
         * @return A const reference to the RangeMapper for axis d.
         */
        const RangeMapper& GetMapper(size_t d) const { return mapper[d]; }

        /**
         * @brief Returns all per-axis mappers.
         * @return A reference to the array of RangeMappers.
         */
        std::array<RangeMapper, D>& GetMappers() { return mapper; }
        /**
         * @brief Returns all per-axis mappers as const.
         * @return A const reference to the array of RangeMappers.
         */
        const std::array<RangeMapper, D>& GetMappers() const { return mapper; }

        /**
         * @brief Replaces all per-axis mappers.
         * @param _mapper The new array of RangeMappers.
         */
        void SetMappers(const std::array<RangeMapper, D>& _mapper) { mapper = _mapper; }
        /**
         * @brief Replaces the mapper for a single axis.
         * @param d The axis index.
         * @param _mapper The new RangeMapper for that axis.
         */
        void SetMapper(size_t d, const RangeMapper& _mapper) { mapper[d] = _mapper; }

        /**
         * @brief Transforms a position vector through the viewport mapping.
         * @param in The input position.
         * @param out The transformed output position.
         */
        void TransformPosition(const Vector<D>& in, Vector<D>& out) const;
        /**
         * @brief Transforms a position vector through the viewport mapping.
         * @param in The input position.
         * @return The transformed position.
         */
        Vector<D> TransformPosition(const Vector<D>& in) const;

        void RenderTriangle(const WorldVertex<ColorT, D>& a, const WorldVertex<ColorT, D>& b, const WorldVertex<ColorT, D>& c) override;
        void RenderGeometry(const std::vector<WorldVertex<ColorT, D>>& vertices, const std::vector<int>& triangles_indices) override;
    };
}

// -- IWorldGraphics --

template <typename ColorT, size_t D>
inline void Engine::IWorldGraphics<ColorT, D>::RenderGeometry(const std::vector<WorldVertex<ColorT, D>>& vertices, const std::vector<int>& triangles_indices) {
    // Not enough to render
    if (vertices.empty() || triangles_indices.size() < 3) return;

    for (int i = 2; i < triangles_indices.size(); i += 3) {
        int i1 = triangles_indices[i-2], i2 = triangles_indices[i-1], i3 = triangles_indices[i];
        // Invalid triangle check
        if (i1 < 0 || i2 < 0 || i3 < 0 || i1 >= vertices.size() || i2 >= vertices.size() || i3 >= vertices.size())
            continue;
        
        this->RenderTriangle(vertices[i1], vertices[i2], vertices[i3]);
    }
}

// -- MatrixWorldGraphics --

template <typename ColorT, size_t D>
inline void Engine::MatrixWorldGraphics<ColorT, D>::GetMatrixData(std::array<double, MatrixCellCount>& res) const {
    // Note that this is unexpected, the matrix_stack size are always bigger than 1. But handle just in case.
    if (matrix_stack.empty()) SquareMatrix<D>::GetIdentity(res, 1);
    else res = matrix_stack.top().Data();
}
template <typename ColorT, size_t D>
inline void Engine::MatrixWorldGraphics<ColorT, D>::GetMatrixData(SquareMatrix<D>& res) const { return GetMatrixData(res.Data()); }

// Only use this if you know what you're doing, will replaced the top matrix data with the given data
template <typename ColorT, size_t D>
inline void Engine::MatrixWorldGraphics<ColorT, D>::SetMatrixData(const std::array<double, MatrixCellCount>& data) { return SetMatrixData(SquareMatrix<D>(data)); }
template <typename ColorT, size_t D>
inline void Engine::MatrixWorldGraphics<ColorT, D>::SetMatrixData(const SquareMatrix<D>& data) {
    if (matrix_stack.empty()) matrix_stack.push(data);
    else matrix_stack.top() = data;
}
        
template <typename ColorT, size_t D>
inline void Engine::MatrixWorldGraphics<ColorT, D>::PushMatrix() { matrix_stack.push(matrix_stack.top()); }
template <typename ColorT, size_t D>
inline void Engine::MatrixWorldGraphics<ColorT, D>::PopMatrix() {
    // This case is unexpected and not supposed to happened, but handle just in case
    if (matrix_stack.empty()) {
        matrix_stack.push(SquareMatrix<D>::GetIdentity(1));
        return;
    }
    // If there's only 1 matrix, set it to identity.
    if (matrix_stack.size() == 1) {
        SquareMatrix<D>::GetIdentity(matrix_stack.top(), 1);
        return;
    }

    // Otherwise, pop it.
    matrix_stack.pop();
}

template <typename ColorT, size_t D>
inline void Engine::MatrixWorldGraphics<ColorT, D>::LoadIdentity(bool replace) {
    if (matrix_stack.empty()) {
        matrix_stack.push(SquareMatrix<D>::GetIdentity(1));
        return;
    }

    if (replace || matrix_stack.size() == 1) {
        SquareMatrix<D>::GetIdentity(matrix_stack.top(), 1);
        return;
    }

    // Pop + Re-push matrix
    matrix_stack.pop();
    matrix_stack.push(matrix_stack.top());
}

template <typename ColorT, size_t D>
inline void Engine::MatrixWorldGraphics<ColorT, D>::MultiplyMatrix(const std::array<double, MatrixCellCount>& mat) {
    if (matrix_stack.empty()) {
        matrix_stack.push(SquareMatrix<D>(mat));
        return;
    }

    matrix_stack.top() *= mat;
}
template <typename ColorT, size_t D>
inline void Engine::MatrixWorldGraphics<ColorT, D>::MultiplyMatrix(const SquareMatrix<D>& mat) { return MultiplyMatrix(mat.Data()); }
template <typename ColorT, size_t D>
inline void Engine::MatrixWorldGraphics<ColorT, D>::MultiplyMatrix(const std::function<void(std::array<double, MatrixCellCount>&)>& mat_gen) {
    std::array<double, MatrixCellCount> _tmp;
    mat_gen(_tmp);
    MultiplyMatrix(_tmp);
}

template <typename ColorT, size_t D>
inline void Engine::MatrixWorldGraphics<ColorT, D>::TransformPosition(const Vector<D>& in, Vector<D>& out) const {
    out = matrix_stack.empty() ? in : (matrix_stack.top() * in);
}
template <typename ColorT, size_t D>
inline Engine::Vector<D> Engine::MatrixWorldGraphics<ColorT, D>::TransformPosition(const Vector<D>& in) const {
    Vector<D> out(in); TransformPosition(in, out); return out;
}

template <typename ColorT, size_t D>
inline void Engine::MatrixWorldGraphics<ColorT, D>::RenderTriangle(const WorldVertex<ColorT, D>& a, const WorldVertex<ColorT, D>& b, const WorldVertex<ColorT, D>& c) {
    // Compute new vertices (aka compute new position)
    WorldVertex<ColorT, D> v_a(a), v_b(b), v_c(c);
    TransformPosition(a.position, v_a.position);
    TransformPosition(b.position, v_b.position);
    TransformPosition(c.position, v_c.position);

    return WorldGraphicsWrapper<ColorT, D>::RenderTriangle(v_a, v_b, v_c);
}
template <typename ColorT, size_t D>
inline void Engine::MatrixWorldGraphics<ColorT, D>::RenderGeometry(const std::vector<WorldVertex<ColorT, D>>& vertices, const std::vector<int>& triangles_indices) {
    std::vector<WorldVertex<ColorT, D>> tmp_v(vertices);
    for (WorldVertex<ColorT, D>& v : tmp_v)
        TransformPosition(v.position, v.position);
    
    return WorldGraphicsWrapper<ColorT, D>::RenderGeometry(tmp_v, triangles_indices);
}

// -- PlaneClipWorldGraphics --

template <typename ColorT, size_t D>
inline bool Engine::PlaneClipWorldGraphics<ColorT, D>::__isInside(const Vector<D>& p, const WorldPlane<D>& plane) const {
    return NumericConstants::IsNearZeroOrPositive(plane.EvaluatePoint(p));
}
template <typename ColorT, size_t D>
inline double Engine::PlaneClipWorldGraphics<ColorT, D>::__computePlaneIntersection(const WorldPlane<D>& plane, const Vector<D>& a, const Vector<D>& b) const {
    double a_val = plane.EvaluatePoint(a), b_val = plane.EvaluatePoint(b);
    
    double factor = a_val - b_val;
    if (NumericConstants::IsNearZero(factor))
        return 0.5; // Best effort, the line is (or almost) parallel to the plane, chose 0.5 for numerical stability.

    return a_val / factor;
}

template <typename ColorT, size_t D>
inline void Engine::PlaneClipWorldGraphics<ColorT, D>::ComputeClippedTriangleGeometry(const WorldVertex<ColorT, D>& a, const WorldVertex<ColorT, D>& b, const WorldVertex<ColorT, D>& c, std::vector<WorldVertex<ColorT, D>>& out_vertices, std::vector<int>& out_triangles_indices) const {
    // Initialize
    out_vertices.clear();
    out_triangles_indices.clear();

    // Algorithm init
    std::vector<WorldVertex<ColorT, D>> polygon = {a, b, c};
    std::vector<WorldVertex<ColorT, D>> tmp;

    for (const WorldPlane<D>& plane : planes) {
        // No triangle can be form
        if (polygon.size() < 3) break;
    
        tmp.clear();
        for (int i = 0; i < polygon.size(); ++i) {
            WorldVertex<ColorT, D>& prev = polygon[(polygon.size() - 1 + i) % polygon.size()];
            WorldVertex<ColorT, D>& curr = polygon[i];

            bool prev_inside = __isInside(prev.position, plane);
            bool curr_inside = __isInside(curr.position, plane);

            // Exactly one of the vertex not inside, append the intersection point
            if (curr_inside != prev_inside) {
                
                tmp.push_back(prev); // prev here is just tmp, avoiding 'no default constructor for ColorT'.
                WorldVertex<ColorT, D>& intersection = tmp.back();

                double t = __computePlaneIntersection(plane, prev.position, curr.position);
                t = std::max(0.0, std::min(1.0, t)); // Clamp t to [0, 1]

                Vector<D>::Lerp(prev.position, curr.position, t, intersection.position);
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

template <typename ColorT, size_t D>
inline void Engine::PlaneClipWorldGraphics<ColorT, D>::RenderTriangle(const WorldVertex<ColorT, D>& a, const WorldVertex<ColorT, D>& b, const WorldVertex<ColorT, D>& c) {
    std::vector<WorldVertex<ColorT, D>> vertices;
    std::vector<int> triangles_indices;

    ComputeClippedTriangleGeometry(a, b, c, vertices, triangles_indices);

    WorldGraphicsWrapper<ColorT, D>::RenderGeometry(vertices, triangles_indices);
}

template <typename ColorT, size_t D>
inline void Engine::PlaneClipWorldGraphics<ColorT, D>::RenderGeometry(const std::vector<WorldVertex<ColorT, D>>& vertices, const std::vector<int>& triangles_indices) {
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

// -- ViewportTransformWorldGraphics --

template <typename ColorT, size_t D>
inline void Engine::ViewportTransformWorldGraphics<ColorT, D>::TransformPosition(const Vector<D>& in, Vector<D>& out) const {
    std::transform(in.Data().begin(), in.Data().end(), mapper.begin(), out.Data().begin(), [](const double& v, const RangeMapper& map) { return map.Map(v); });
}
template <typename ColorT, size_t D>
inline Engine::Vector<D> Engine::ViewportTransformWorldGraphics<ColorT, D>::TransformPosition(const Vector<D>& in) const {
    Vector<D> out(in); TransformPosition(in, out); return out;
}

template <typename ColorT, size_t D>
inline void Engine::ViewportTransformWorldGraphics<ColorT, D>::RenderTriangle(const WorldVertex<ColorT, D>& a, const WorldVertex<ColorT, D>& b, const WorldVertex<ColorT, D>& c) {
    // Compute new vertices (aka compute new position)
    WorldVertex<ColorT, D> v_a(a), v_b(b), v_c(c);
    TransformPosition(a.position, v_a.position);
    TransformPosition(b.position, v_b.position);
    TransformPosition(c.position, v_c.position);

    return WorldGraphicsWrapper<ColorT, D>::RenderTriangle(v_a, v_b, v_c);
}
template <typename ColorT, size_t D>
inline void Engine::ViewportTransformWorldGraphics<ColorT, D>::RenderGeometry(const std::vector<WorldVertex<ColorT, D>>& vertices, const std::vector<int>& triangles_indices) {
    std::vector<WorldVertex<ColorT, D>> tmp_v(vertices);
    for (WorldVertex<ColorT, D>& v : tmp_v)
        TransformPosition(v.position, v.position);
    
    return WorldGraphicsWrapper<ColorT, D>::RenderGeometry(tmp_v, triangles_indices);
}
