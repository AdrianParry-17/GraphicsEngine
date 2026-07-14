#pragma once

#include "Engine_2D.h"
#include "Engine_3D.h"

#include <cmath>

namespace Engine {
    /**
     * @brief Built-in 2D primitive mesh types.
     */
    enum class MeshPrimitive2DType {
        /** @brief A single origin-centered triangle. */
        Triangle = 0,
        /** @brief A unit square composed of two triangles. */
        Quad = 1,
        /** @brief A filled unit circle using 32 perimeter segments. */
        Circle = 2
    };

    /**
     * @brief Built-in 3D primitive mesh types.
     */
    enum class MeshPrimitive3DType {
        /** @brief A unit square on the XZ plane. */
        Plane = 0,
        /** @brief A unit cube with separate vertices for each face. */
        Cube = 1,
        /** @brief A unit square-base pyramid. */
        Pyramid = 2,
        /** @brief A unit-bounds tetrahedron. */
        Tetrahedron = 3,
        /** @brief A unit-bounds octahedron. */
        Octahedron = 4,
        /** @brief A unit cylinder using 24 radial segments. */
        Cylinder = 5,
        /** @brief A unit cone using 24 radial segments. */
        Cone = 6,
        /** @brief A unit-diameter UV sphere using 24 slices and 12 stacks. */
        Sphere = 7
    };

    /**
     * @brief Static factory for common origin-centered primitive meshes.
     *
     * Every generated vertex receives a copy of the supplied payload. The
     * return-by-value overloads provide concise construction, while matching
     * output-parameter overloads overwrite an existing mesh without requiring
     * callers to assign a temporary result.
     *
     * Primitive dimensions are expressed in world-space units. Unless stated
     * otherwise, generated geometry is centered around the origin and fits
     * within the range [-0.5, 0.5] on each used axis.
     */
    class MeshPrimitive final {
    public:
        /**
         * @brief Generates a 2D primitive mesh with a uniform vertex payload.
         * @tparam ColorT Vertex payload type.
         * @param type Primitive type.
         * @param color Payload copied into every vertex.
         * @return Generated mesh.
         * @note Returns an empty mesh when @p type is not a valid enumerator.
         */
        template <typename ColorT>
        static Mesh2D<ColorT> GeneratePrimitiveMesh(MeshPrimitive2DType type, const ColorT& color) {
            Mesh2D<ColorT> output;
            GeneratePrimitiveMesh(type, color, output);
            return output;
        }

        /**
         * @brief Generates a 2D primitive mesh with a uniform vertex payload.
         * @tparam ColorT Vertex payload type.
         * @param type Primitive type.
         * @param color Payload copied into every vertex.
         * @param output Mesh to overwrite with generated data.
         * @note Clears @p output when @p type is not a valid enumerator.
         */
        template <typename ColorT>
        static void GeneratePrimitiveMesh(MeshPrimitive2DType type, const ColorT& color, Mesh2D<ColorT>& output) {
            switch (type) {
            case MeshPrimitive2DType::Triangle:
                GenerateTriangle2D(color, output);
                return;
            case MeshPrimitive2DType::Quad:
                GenerateQuad2D(color, output);
                return;
            case MeshPrimitive2DType::Circle:
                GenerateCircle2D(color, output);
                return;
            default:
                output.Clear();
                return;
            }
        }

        /**
         * @brief Generates a 3D primitive mesh with a uniform vertex payload.
         * @tparam ColorT Vertex payload type.
         * @param type Primitive type.
         * @param color Payload copied into every vertex.
         * @return Generated mesh.
         * @note Returns an empty mesh when @p type is not a valid enumerator.
         */
        template <typename ColorT>
        static Mesh3D<ColorT> GeneratePrimitiveMesh(MeshPrimitive3DType type, const ColorT& color) {
            Mesh3D<ColorT> output;
            GeneratePrimitiveMesh(type, color, output);
            return output;
        }

        /**
         * @brief Generates a 3D primitive mesh with a uniform vertex payload.
         * @tparam ColorT Vertex payload type.
         * @param type Primitive type.
         * @param color Payload copied into every vertex.
         * @param output Mesh to overwrite with generated data.
         * @note Clears @p output when @p type is not a valid enumerator.
         */
        template <typename ColorT>
        static void GeneratePrimitiveMesh(MeshPrimitive3DType type, const ColorT& color, Mesh3D<ColorT>& output) {
            switch (type) {
            case MeshPrimitive3DType::Plane:
                GeneratePlane3D(color, output);
                return;
            case MeshPrimitive3DType::Cube:
                GenerateCube3D(color, output);
                return;
            case MeshPrimitive3DType::Pyramid:
                GeneratePyramid3D(color, output);
                return;
            case MeshPrimitive3DType::Tetrahedron:
                GenerateTetrahedron3D(color, output);
                return;
            case MeshPrimitive3DType::Octahedron:
                GenerateOctahedron3D(color, output);
                return;
            case MeshPrimitive3DType::Cylinder:
                GenerateCylinder3D(color, output);
                return;
            case MeshPrimitive3DType::Cone:
                GenerateCone3D(color, output);
                return;
            case MeshPrimitive3DType::Sphere:
                GenerateSphere3D(color, output);
                return;
            default:
                output.Clear();
                return;
            }
        }

        /**
         * @brief Generates a unit 2D triangle centered around the origin.
         * @tparam ColorT Vertex payload type.
         * @param color Payload copied into every vertex.
         * @return Generated mesh containing one triangle.
         */
        template <typename ColorT>
        static Mesh2D<ColorT> GenerateTriangle2D(const ColorT& color) {
            Mesh2D<ColorT> output;
            GenerateTriangle2D(color, output);
            return output;
        }

        /**
         * @brief Generates a unit 2D triangle centered around the origin.
         * @tparam ColorT Vertex payload type.
         * @param color Payload copied into every vertex.
         * @param output Mesh to overwrite with one triangle.
         */
        template <typename ColorT>
        static void GenerateTriangle2D(const ColorT& color, Mesh2D<ColorT>& output) {
            output.vertices = {
                WorldVertex2D<ColorT>({-0.5, -0.5}, color),
                WorldVertex2D<ColorT>({ 0.5, -0.5}, color),
                WorldVertex2D<ColorT>({ 0.0,  0.5}, color)
            };
            output.indices = {0, 1, 2};
        }

        /**
         * @brief Generates a unit 2D quad centered around the origin.
         * @tparam ColorT Vertex payload type.
         * @param color Payload copied into every vertex.
         * @return Generated mesh containing two triangles.
         */
        template <typename ColorT>
        static Mesh2D<ColorT> GenerateQuad2D(const ColorT& color) {
            Mesh2D<ColorT> output;
            GenerateQuad2D(color, output);
            return output;
        }

        /**
         * @brief Generates a unit 2D quad centered around the origin.
         * @tparam ColorT Vertex payload type.
         * @param color Payload copied into every vertex.
         * @param output Mesh to overwrite with two triangles.
         */
        template <typename ColorT>
        static void GenerateQuad2D(const ColorT& color, Mesh2D<ColorT>& output) {
            output.vertices = {
                WorldVertex2D<ColorT>({-0.5,  0.5}, color),
                WorldVertex2D<ColorT>({ 0.5,  0.5}, color),
                WorldVertex2D<ColorT>({-0.5, -0.5}, color),
                WorldVertex2D<ColorT>({ 0.5, -0.5}, color)
            };
            output.indices = {0, 1, 2, 1, 2, 3};
        }

        /**
         * @brief Generates a filled unit-diameter circle centered around the origin.
         * @tparam ColorT Vertex payload type.
         * @param color Payload copied into every vertex.
         * @param segments Number of perimeter segments.
         * @return Generated triangle-fan mesh, or an empty mesh when @p segments is less than three.
         */
        template <typename ColorT>
        static Mesh2D<ColorT> GenerateCircle2D(const ColorT& color, size_t segments = 32) {
            Mesh2D<ColorT> output;
            GenerateCircle2D(color, output, segments);
            return output;
        }

        /**
         * @brief Generates a filled unit-diameter circle centered around the origin.
         * @tparam ColorT Vertex payload type.
         * @param color Payload copied into every vertex.
         * @param output Mesh to overwrite with generated data.
         * @param segments Number of perimeter segments.
         * @note Clears @p output when @p segments is less than three.
         */
        template <typename ColorT>
        static void GenerateCircle2D(const ColorT& color, Mesh2D<ColorT>& output, size_t segments = 32) {
            if (segments < 3) {
                output.Clear();
                return;
            }

            const double full_rotation = 2.0 * std::acos(-1.0);
            output.vertices.clear();
            output.indices.clear();
            output.vertices.reserve(segments + 1);
            output.indices.reserve(segments * 3);

            output.vertices.push_back(WorldVertex2D<ColorT>({0.0, 0.0}, color));
            for (size_t i = 0; i < segments; ++i) {
                const double angle = full_rotation * i / segments;
                output.vertices.push_back(WorldVertex2D<ColorT>(
                    {0.5 * std::cos(angle), 0.5 * std::sin(angle)},
                    color
                ));
            }

            for (size_t i = 0; i < segments; ++i) {
                output.indices.push_back(0);
                output.indices.push_back(static_cast<int>(i + 1));
                output.indices.push_back(static_cast<int>((i + 1) % segments + 1));
            }
        }

        /**
         * @brief Generates a unit XZ plane centered around the origin.
         * @tparam ColorT Vertex payload type.
         * @param color Payload copied into every vertex.
         * @return Generated mesh containing two triangles.
         */
        template <typename ColorT>
        static Mesh3D<ColorT> GeneratePlane3D(const ColorT& color) {
            Mesh3D<ColorT> output;
            GeneratePlane3D(color, output);
            return output;
        }

        /**
         * @brief Generates a unit XZ plane centered around the origin.
         * @tparam ColorT Vertex payload type.
         * @param color Payload copied into every vertex.
         * @param output Mesh to overwrite with two triangles.
         */
        template <typename ColorT>
        static void GeneratePlane3D(const ColorT& color, Mesh3D<ColorT>& output) {
            output.vertices = {
                WorldVertex3D<ColorT>({-0.5, 0.0, -0.5}, color),
                WorldVertex3D<ColorT>({ 0.5, 0.0, -0.5}, color),
                WorldVertex3D<ColorT>({-0.5, 0.0,  0.5}, color),
                WorldVertex3D<ColorT>({ 0.5, 0.0,  0.5}, color)
            };
            output.indices = {0, 1, 2, 1, 3, 2};
        }

        /**
         * @brief Generates a unit cube centered around the origin.
         *
         * Uses four vertices per face so callers can later assign different
         * payloads per face without changing the topology.
         *
         * @tparam ColorT Vertex payload type.
         * @param color Payload copied into every vertex.
         * @return Generated mesh containing twelve triangles.
         */
        template <typename ColorT>
        static Mesh3D<ColorT> GenerateCube3D(const ColorT& color) {
            Mesh3D<ColorT> output;
            GenerateCube3D(color, output);
            return output;
        }

        /**
         * @brief Generates a unit cube centered around the origin.
         *
         * Uses four vertices per face so callers can later assign different
         * payloads per face without changing the topology.
         *
         * @tparam ColorT Vertex payload type.
         * @param color Payload copied into every vertex.
         * @param output Mesh to overwrite with twelve triangles.
         */
        template <typename ColorT>
        static void GenerateCube3D(const ColorT& color, Mesh3D<ColorT>& output) {
            output.vertices = {
                // Front (+Z)
                WorldVertex3D<ColorT>({-0.5, -0.5,  0.5}, color),
                WorldVertex3D<ColorT>({ 0.5, -0.5,  0.5}, color),
                WorldVertex3D<ColorT>({ 0.5,  0.5,  0.5}, color),
                WorldVertex3D<ColorT>({-0.5,  0.5,  0.5}, color),
                // Back (-Z)
                WorldVertex3D<ColorT>({-0.5, -0.5, -0.5}, color),
                WorldVertex3D<ColorT>({ 0.5, -0.5, -0.5}, color),
                WorldVertex3D<ColorT>({ 0.5,  0.5, -0.5}, color),
                WorldVertex3D<ColorT>({-0.5,  0.5, -0.5}, color),
                // Left (-X)
                WorldVertex3D<ColorT>({-0.5, -0.5, -0.5}, color),
                WorldVertex3D<ColorT>({-0.5, -0.5,  0.5}, color),
                WorldVertex3D<ColorT>({-0.5,  0.5,  0.5}, color),
                WorldVertex3D<ColorT>({-0.5,  0.5, -0.5}, color),
                // Right (+X)
                WorldVertex3D<ColorT>({ 0.5, -0.5, -0.5}, color),
                WorldVertex3D<ColorT>({ 0.5, -0.5,  0.5}, color),
                WorldVertex3D<ColorT>({ 0.5,  0.5,  0.5}, color),
                WorldVertex3D<ColorT>({ 0.5,  0.5, -0.5}, color),
                // Top (+Y)
                WorldVertex3D<ColorT>({-0.5,  0.5, -0.5}, color),
                WorldVertex3D<ColorT>({ 0.5,  0.5, -0.5}, color),
                WorldVertex3D<ColorT>({ 0.5,  0.5,  0.5}, color),
                WorldVertex3D<ColorT>({-0.5,  0.5,  0.5}, color),
                // Bottom (-Y)
                WorldVertex3D<ColorT>({-0.5, -0.5, -0.5}, color),
                WorldVertex3D<ColorT>({ 0.5, -0.5, -0.5}, color),
                WorldVertex3D<ColorT>({ 0.5, -0.5,  0.5}, color),
                WorldVertex3D<ColorT>({-0.5, -0.5,  0.5}, color)
            };
            output.indices = {
                0, 1, 2, 0, 2, 3,
                5, 4, 7, 5, 7, 6,
                8, 9, 10, 8, 10, 11,
                13, 12, 15, 13, 15, 14,
                16, 19, 18, 16, 18, 17,
                20, 21, 22, 20, 22, 23
            };
        }

        /**
         * @brief Generates a unit square-base pyramid centered around the origin.
         *
         * Uses separate vertices for the base and each side face so per-face
         * payload changes do not require rebuilding the mesh topology.
         *
         * @tparam ColorT Vertex payload type.
         * @param color Payload copied into every vertex.
         * @return Generated mesh containing six triangles.
         */
        template <typename ColorT>
        static Mesh3D<ColorT> GeneratePyramid3D(const ColorT& color) {
            Mesh3D<ColorT> output;
            GeneratePyramid3D(color, output);
            return output;
        }

        /**
         * @brief Generates a unit square-base pyramid centered around the origin.
         *
         * Uses separate vertices for the base and each side face so per-face
         * payload changes do not require rebuilding the mesh topology.
         *
         * @tparam ColorT Vertex payload type.
         * @param color Payload copied into every vertex.
         * @param output Mesh to overwrite with six triangles.
         */
        template <typename ColorT>
        static void GeneratePyramid3D(const ColorT& color, Mesh3D<ColorT>& output) {
            output.vertices = {
                // Base (-Y)
                WorldVertex3D<ColorT>({-0.5, -0.5, -0.5}, color),
                WorldVertex3D<ColorT>({ 0.5, -0.5, -0.5}, color),
                WorldVertex3D<ColorT>({ 0.5, -0.5,  0.5}, color),
                WorldVertex3D<ColorT>({-0.5, -0.5,  0.5}, color),
                // Front (+Z)
                WorldVertex3D<ColorT>({-0.5, -0.5,  0.5}, color),
                WorldVertex3D<ColorT>({ 0.5, -0.5,  0.5}, color),
                WorldVertex3D<ColorT>({ 0.0,  0.5,  0.0}, color),
                // Right (+X)
                WorldVertex3D<ColorT>({ 0.5, -0.5,  0.5}, color),
                WorldVertex3D<ColorT>({ 0.5, -0.5, -0.5}, color),
                WorldVertex3D<ColorT>({ 0.0,  0.5,  0.0}, color),
                // Back (-Z)
                WorldVertex3D<ColorT>({ 0.5, -0.5, -0.5}, color),
                WorldVertex3D<ColorT>({-0.5, -0.5, -0.5}, color),
                WorldVertex3D<ColorT>({ 0.0,  0.5,  0.0}, color),
                // Left (-X)
                WorldVertex3D<ColorT>({-0.5, -0.5, -0.5}, color),
                WorldVertex3D<ColorT>({-0.5, -0.5,  0.5}, color),
                WorldVertex3D<ColorT>({ 0.0,  0.5,  0.0}, color)
            };
            output.indices = {
                0, 1, 2, 0, 2, 3,
                4, 5, 6,
                7, 8, 9,
                10, 11, 12,
                13, 14, 15
            };
        }

        /**
         * @brief Generates a tetrahedron centered within unit bounds.
         *
         * Uses separate vertices for each face so callers can assign per-face
         * payloads without changing the topology.
         *
         * @tparam ColorT Vertex payload type.
         * @param color Payload copied into every vertex.
         * @return Generated mesh containing four triangles.
         */
        template <typename ColorT>
        static Mesh3D<ColorT> GenerateTetrahedron3D(const ColorT& color) {
            Mesh3D<ColorT> output;
            GenerateTetrahedron3D(color, output);
            return output;
        }

        /**
         * @brief Generates a tetrahedron centered within unit bounds.
         *
         * Uses separate vertices for each face so callers can assign per-face
         * payloads without changing the topology.
         *
         * @tparam ColorT Vertex payload type.
         * @param color Payload copied into every vertex.
         * @param output Mesh to overwrite with four triangles.
         */
        template <typename ColorT>
        static void GenerateTetrahedron3D(const ColorT& color, Mesh3D<ColorT>& output) {
            const Vector3 top(0.0, 0.5, 0.0);
            const Vector3 front_left(-0.5, -0.5, 0.5);
            const Vector3 front_right(0.5, -0.5, 0.5);
            const Vector3 back(0.0, -0.5, -0.5);

            output.vertices = {
                WorldVertex3D<ColorT>(front_left, color),
                WorldVertex3D<ColorT>(front_right, color),
                WorldVertex3D<ColorT>(top, color),

                WorldVertex3D<ColorT>(front_right, color),
                WorldVertex3D<ColorT>(back, color),
                WorldVertex3D<ColorT>(top, color),

                WorldVertex3D<ColorT>(back, color),
                WorldVertex3D<ColorT>(front_left, color),
                WorldVertex3D<ColorT>(top, color),

                WorldVertex3D<ColorT>(front_left, color),
                WorldVertex3D<ColorT>(back, color),
                WorldVertex3D<ColorT>(front_right, color)
            };
            output.indices = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
        }

        /**
         * @brief Generates an octahedron centered within unit bounds.
         *
         * Uses separate vertices for each face so callers can assign per-face
         * payloads without changing the topology.
         *
         * @tparam ColorT Vertex payload type.
         * @param color Payload copied into every vertex.
         * @return Generated mesh containing eight triangles.
         */
        template <typename ColorT>
        static Mesh3D<ColorT> GenerateOctahedron3D(const ColorT& color) {
            Mesh3D<ColorT> output;
            GenerateOctahedron3D(color, output);
            return output;
        }

        /**
         * @brief Generates an octahedron centered within unit bounds.
         *
         * Uses separate vertices for each face so callers can assign per-face
         * payloads without changing the topology.
         *
         * @tparam ColorT Vertex payload type.
         * @param color Payload copied into every vertex.
         * @param output Mesh to overwrite with eight triangles.
         */
        template <typename ColorT>
        static void GenerateOctahedron3D(const ColorT& color, Mesh3D<ColorT>& output) {
            const Vector3 top(0.0, 0.5, 0.0);
            const Vector3 bottom(0.0, -0.5, 0.0);
            const Vector3 front(0.0, 0.0, 0.5);
            const Vector3 right(0.5, 0.0, 0.0);
            const Vector3 back(0.0, 0.0, -0.5);
            const Vector3 left(-0.5, 0.0, 0.0);

            output.vertices = {
                WorldVertex3D<ColorT>(front, color), WorldVertex3D<ColorT>(right, color), WorldVertex3D<ColorT>(top, color),
                WorldVertex3D<ColorT>(right, color), WorldVertex3D<ColorT>(back, color), WorldVertex3D<ColorT>(top, color),
                WorldVertex3D<ColorT>(back, color), WorldVertex3D<ColorT>(left, color), WorldVertex3D<ColorT>(top, color),
                WorldVertex3D<ColorT>(left, color), WorldVertex3D<ColorT>(front, color), WorldVertex3D<ColorT>(top, color),

                WorldVertex3D<ColorT>(front, color), WorldVertex3D<ColorT>(bottom, color), WorldVertex3D<ColorT>(right, color),
                WorldVertex3D<ColorT>(right, color), WorldVertex3D<ColorT>(bottom, color), WorldVertex3D<ColorT>(back, color),
                WorldVertex3D<ColorT>(back, color), WorldVertex3D<ColorT>(bottom, color), WorldVertex3D<ColorT>(left, color),
                WorldVertex3D<ColorT>(left, color), WorldVertex3D<ColorT>(bottom, color), WorldVertex3D<ColorT>(front, color)
            };
            output.indices = {
                0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
                12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23
            };
        }

        /**
         * @brief Generates a unit-height, unit-diameter cylinder centered around the origin.
         * @tparam ColorT Vertex payload type.
         * @param color Payload copied into every vertex.
         * @param segments Number of radial segments.
         * @return Generated mesh, or an empty mesh when @p segments is less than three.
         */
        template <typename ColorT>
        static Mesh3D<ColorT> GenerateCylinder3D(const ColorT& color, size_t segments = 24) {
            Mesh3D<ColorT> output;
            GenerateCylinder3D(color, output, segments);
            return output;
        }

        /**
         * @brief Generates a unit-height, unit-diameter cylinder centered around the origin.
         * @tparam ColorT Vertex payload type.
         * @param color Payload copied into every vertex.
         * @param output Mesh to overwrite with generated data.
         * @param segments Number of radial segments.
         * @note Clears @p output when @p segments is less than three.
         */
        template <typename ColorT>
        static void GenerateCylinder3D(const ColorT& color, Mesh3D<ColorT>& output, size_t segments = 24) {
            if (segments < 3) {
                output.Clear();
                return;
            }

            const double full_rotation = 2.0 * std::acos(-1.0);
            output.vertices.clear();
            output.indices.clear();
            output.vertices.reserve(segments * 6 + 2);
            output.indices.reserve(segments * 12);

            for (size_t i = 0; i < segments; ++i) {
                const double angle = full_rotation * i / segments;
                const double next_angle = full_rotation * ((i + 1) % segments) / segments;
                const double x = 0.5 * std::cos(angle);
                const double z = 0.5 * std::sin(angle);
                const double next_x = 0.5 * std::cos(next_angle);
                const double next_z = 0.5 * std::sin(next_angle);
                const int base = static_cast<int>(output.vertices.size());

                output.vertices.push_back(WorldVertex3D<ColorT>({x, -0.5, z}, color));
                output.vertices.push_back(WorldVertex3D<ColorT>({x, 0.5, z}, color));
                output.vertices.push_back(WorldVertex3D<ColorT>({next_x, 0.5, next_z}, color));
                output.vertices.push_back(WorldVertex3D<ColorT>({next_x, -0.5, next_z}, color));
                output.indices.insert(output.indices.end(), {base, base + 1, base + 2, base, base + 2, base + 3});
            }

            const int top_center = static_cast<int>(output.vertices.size());
            output.vertices.push_back(WorldVertex3D<ColorT>({0.0, 0.5, 0.0}, color));
            for (size_t i = 0; i < segments; ++i) {
                const double angle = full_rotation * i / segments;
                output.vertices.push_back(WorldVertex3D<ColorT>(
                    {0.5 * std::cos(angle), 0.5, 0.5 * std::sin(angle)},
                    color
                ));
            }
            for (size_t i = 0; i < segments; ++i) {
                const int current = top_center + 1 + static_cast<int>(i);
                const int next = top_center + 1 + static_cast<int>((i + 1) % segments);
                output.indices.insert(output.indices.end(), {top_center, next, current});
            }

            const int bottom_center = static_cast<int>(output.vertices.size());
            output.vertices.push_back(WorldVertex3D<ColorT>({0.0, -0.5, 0.0}, color));
            for (size_t i = 0; i < segments; ++i) {
                const double angle = full_rotation * i / segments;
                output.vertices.push_back(WorldVertex3D<ColorT>(
                    {0.5 * std::cos(angle), -0.5, 0.5 * std::sin(angle)},
                    color
                ));
            }
            for (size_t i = 0; i < segments; ++i) {
                const int current = bottom_center + 1 + static_cast<int>(i);
                const int next = bottom_center + 1 + static_cast<int>((i + 1) % segments);
                output.indices.insert(output.indices.end(), {bottom_center, current, next});
            }
        }

        /**
         * @brief Generates a unit-height, unit-diameter cone centered around the origin.
         * @tparam ColorT Vertex payload type.
         * @param color Payload copied into every vertex.
         * @param segments Number of radial segments.
         * @return Generated mesh, or an empty mesh when @p segments is less than three.
         */
        template <typename ColorT>
        static Mesh3D<ColorT> GenerateCone3D(const ColorT& color, size_t segments = 24) {
            Mesh3D<ColorT> output;
            GenerateCone3D(color, output, segments);
            return output;
        }

        /**
         * @brief Generates a unit-height, unit-diameter cone centered around the origin.
         * @tparam ColorT Vertex payload type.
         * @param color Payload copied into every vertex.
         * @param output Mesh to overwrite with generated data.
         * @param segments Number of radial segments.
         * @note Clears @p output when @p segments is less than three.
         */
        template <typename ColorT>
        static void GenerateCone3D(const ColorT& color, Mesh3D<ColorT>& output, size_t segments = 24) {
            if (segments < 3) {
                output.Clear();
                return;
            }

            const double full_rotation = 2.0 * std::acos(-1.0);
            output.vertices.clear();
            output.indices.clear();
            output.vertices.reserve(segments * 4 + 1);
            output.indices.reserve(segments * 6);

            for (size_t i = 0; i < segments; ++i) {
                const double angle = full_rotation * i / segments;
                const double next_angle = full_rotation * ((i + 1) % segments) / segments;
                const int base = static_cast<int>(output.vertices.size());

                output.vertices.push_back(WorldVertex3D<ColorT>(
                    {0.5 * std::cos(angle), -0.5, 0.5 * std::sin(angle)},
                    color
                ));
                output.vertices.push_back(WorldVertex3D<ColorT>({0.0, 0.5, 0.0}, color));
                output.vertices.push_back(WorldVertex3D<ColorT>(
                    {0.5 * std::cos(next_angle), -0.5, 0.5 * std::sin(next_angle)},
                    color
                ));
                output.indices.insert(output.indices.end(), {base, base + 1, base + 2});
            }

            const int bottom_center = static_cast<int>(output.vertices.size());
            output.vertices.push_back(WorldVertex3D<ColorT>({0.0, -0.5, 0.0}, color));
            for (size_t i = 0; i < segments; ++i) {
                const double angle = full_rotation * i / segments;
                output.vertices.push_back(WorldVertex3D<ColorT>(
                    {0.5 * std::cos(angle), -0.5, 0.5 * std::sin(angle)},
                    color
                ));
            }
            for (size_t i = 0; i < segments; ++i) {
                const int current = bottom_center + 1 + static_cast<int>(i);
                const int next = bottom_center + 1 + static_cast<int>((i + 1) % segments);
                output.indices.insert(output.indices.end(), {bottom_center, current, next});
            }
        }

        /**
         * @brief Generates a unit-diameter UV sphere centered around the origin.
         * @tparam ColorT Vertex payload type.
         * @param color Payload copied into every vertex.
         * @param slices Number of longitudinal segments.
         * @param stacks Number of latitudinal segments.
         * @return Generated mesh, or an empty mesh when fewer than three slices
         *         or fewer than two stacks are requested.
         */
        template <typename ColorT>
        static Mesh3D<ColorT> GenerateSphere3D(const ColorT& color, size_t slices = 24, size_t stacks = 12) {
            Mesh3D<ColorT> output;
            GenerateSphere3D(color, output, slices, stacks);
            return output;
        }

        /**
         * @brief Generates a unit-diameter UV sphere centered around the origin.
         * @tparam ColorT Vertex payload type.
         * @param color Payload copied into every vertex.
         * @param output Mesh to overwrite with generated data.
         * @param slices Number of longitudinal segments.
         * @param stacks Number of latitudinal segments.
         * @note Clears @p output when @p slices is less than three or @p stacks
         *       is less than two.
         */
        template <typename ColorT>
        static void GenerateSphere3D(
            const ColorT& color,
            Mesh3D<ColorT>& output,
            size_t slices = 24,
            size_t stacks = 12
        ) {
            if (slices < 3 || stacks < 2) {
                output.Clear();
                return;
            }

            const double pi = std::acos(-1.0);
            const size_t row_size = slices + 1;
            output.vertices.clear();
            output.indices.clear();
            output.vertices.reserve((stacks + 1) * row_size);
            output.indices.reserve(slices * (stacks - 1) * 6);

            for (size_t stack = 0; stack <= stacks; ++stack) {
                const double latitude = pi * stack / stacks;
                const double y = 0.5 * std::cos(latitude);
                const double radius = 0.5 * std::sin(latitude);

                for (size_t slice = 0; slice <= slices; ++slice) {
                    const double longitude = 2.0 * pi * slice / slices;
                    output.vertices.push_back(WorldVertex3D<ColorT>(
                        {radius * std::cos(longitude), y, radius * std::sin(longitude)},
                        color
                    ));
                }
            }

            for (size_t stack = 0; stack < stacks; ++stack) {
                for (size_t slice = 0; slice < slices; ++slice) {
                    const int a = static_cast<int>(stack * row_size + slice);
                    const int b = static_cast<int>((stack + 1) * row_size + slice);
                    const int c = a + 1;
                    const int d = b + 1;

                    if (stack != 0)
                        output.indices.insert(output.indices.end(), {a, c, b});
                    if (stack + 1 != stacks)
                        output.indices.insert(output.indices.end(), {c, d, b});
                }
            }
        }
    };
}
