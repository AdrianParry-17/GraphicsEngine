#pragma once

#include "Engine_WorldGraphics.h"

#include <cstddef>
#include <vector>

namespace Engine {
    /**
     * @brief Indexed triangle mesh in D-dimensional world space.
     *
     * A mesh is a data container only. Its public vectors allow callers to build
     * or modify geometry directly, while `RenderMesh()` interprets each group of
     * three indices as one triangle.
     * @tparam ColorT The vertex payload type.
     * @tparam D The dimensionality of the vertex positions.
     * @note Mesh does not validate index bounds or require the index count to be
     *       divisible by three. Incomplete trailing indices are not rendered.
     */
    template <typename ColorT, size_t D>
    struct Mesh {
        /**
         * @brief Vertex list used by the indexed triangle data.
         *
         * The payload of each vertex is independent of the mesh topology and may
         * contain a color, texture coordinates, or any other interpolated data.
         */
        std::vector<WorldVertex<ColorT, D>> vertices;
        /**
         * @brief Triangle index list, with each consecutive group of three
         *        entries referencing one triangle in @c vertices.
         */
        std::vector<int> indices;

        /** @brief Default constructor. */
        Mesh() = default;
        /**
         * @brief Constructs a mesh from vertices and indices.
         * @param _vertices Mesh vertices.
         * @param _indices Triangle index list, grouped by threes.
         * @note Both input vectors are copied into the mesh.
         */
        Mesh(const std::vector<WorldVertex<ColorT, D>>& _vertices, const std::vector<int>& _indices)
            : vertices(_vertices), indices(_indices) {}

        /**
         * @brief Clears all vertex and index data.
         */
        void Clear() { vertices.clear(); indices.clear(); }
        /**
         * @brief Checks whether this mesh has enough data to describe a triangle.
         * @return @c true if no renderable triangle data exists.
         * @note This does not validate whether the stored indices are in range.
         */
        bool IsEmpty() const { return vertices.empty() || indices.size() < 3; }
        /**
         * @brief Returns the number of complete indexed triangles.
         * @return The index count divided by three.
         */
        size_t TriangleCount() const { return indices.size() / 3; }
    };
}

template <typename ColorT, size_t D>
inline void Engine::IWorldGraphics<ColorT, D>::RenderMesh(const Engine::Mesh<ColorT, D>& mesh) {
    RenderGeometry(mesh.vertices, mesh.indices);
}
