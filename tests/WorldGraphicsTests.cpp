#include "TestHarness.h"
#include "TestSupport.h"

namespace {
    template <size_t D>
    void ExpectPositionInsideUnitBounds(const Engine::Vector<D>& position) {
        for (size_t i = 0; i < D; ++i) {
            EXPECT_TRUE(position[i] >= -0.5);
            EXPECT_TRUE(position[i] <= 0.5);
        }
    }

    template <typename MeshT>
    void ExpectMeshInsideUnitBounds(const MeshT& mesh) {
        for (size_t i = 0; i < mesh.vertices.size(); ++i)
            ExpectPositionInsideUnitBounds(mesh.vertices[i].position);
    }

    template <typename MeshT>
    void ExpectMeshIndicesValid(const MeshT& mesh) {
        for (size_t i = 0; i < mesh.indices.size(); ++i) {
            EXPECT_TRUE(mesh.indices[i] >= 0);
            EXPECT_TRUE(static_cast<size_t>(mesh.indices[i]) < mesh.vertices.size());
        }
    }
}

TEST_CASE(Mesh_DataHelpersAndRenderMeshForwarding) {
    Engine::Mesh2D<double> mesh;
    EXPECT_TRUE(mesh.IsEmpty());
    EXPECT_EQ(0u, mesh.TriangleCount());

    mesh.vertices = {
        Engine::WorldVertex2D<double>({0, 0}, 1),
        Engine::WorldVertex2D<double>({1, 0}, 2),
        Engine::WorldVertex2D<double>({0, 1}, 3)
    };
    mesh.indices = {0, 1, 2, 0, 1};

    EXPECT_TRUE(!mesh.IsEmpty());
    EXPECT_EQ(1u, mesh.TriangleCount());

    Tests::CountingGraphics2D sink;
    sink.RenderMesh(mesh);
    EXPECT_EQ(1, sink.triangles);
    EXPECT_NEAR(1.0, sink.lastA.color, 1e-12);

    mesh.Clear();
    EXPECT_TRUE(mesh.IsEmpty());
    EXPECT_EQ(0u, mesh.TriangleCount());
}

TEST_CASE(MeshPrimitive_GeneratesCoreUnitMeshes) {
    const Engine::Mesh2D<double> triangle = Engine::MeshPrimitive::GeneratePrimitiveMesh(Engine::MeshPrimitive2DType::Triangle, 2.0);
    EXPECT_EQ(3u, triangle.vertices.size());
    EXPECT_EQ(3u, triangle.indices.size());
    EXPECT_EQ(1u, triangle.TriangleCount());
    ExpectMeshInsideUnitBounds(triangle);

    const Engine::Mesh2D<double> quad = Engine::MeshPrimitive::GenerateQuad2D(3.0);
    EXPECT_EQ(4u, quad.vertices.size());
    EXPECT_EQ(6u, quad.indices.size());
    EXPECT_EQ(2u, quad.TriangleCount());
    ExpectMeshInsideUnitBounds(quad);

    const Engine::Mesh3D<double> plane = Engine::MeshPrimitive::GeneratePrimitiveMesh(Engine::MeshPrimitive3DType::Plane, 4.0);
    EXPECT_EQ(4u, plane.vertices.size());
    EXPECT_EQ(6u, plane.indices.size());
    EXPECT_EQ(2u, plane.TriangleCount());
    ExpectMeshInsideUnitBounds(plane);

    const Engine::Mesh3D<double> cube = Engine::MeshPrimitive::GenerateCube3D(5.0);
    EXPECT_EQ(24u, cube.vertices.size());
    EXPECT_EQ(36u, cube.indices.size());
    EXPECT_EQ(12u, cube.TriangleCount());
    ExpectMeshInsideUnitBounds(cube);

    const Engine::Mesh3D<double> pyramid = Engine::MeshPrimitive::GeneratePrimitiveMesh(Engine::MeshPrimitive3DType::Pyramid, 6.0);
    EXPECT_EQ(16u, pyramid.vertices.size());
    EXPECT_EQ(18u, pyramid.indices.size());
    EXPECT_EQ(6u, pyramid.TriangleCount());
    ExpectMeshInsideUnitBounds(pyramid);
}

TEST_CASE(MeshPrimitive_OutputOverloadsOverwriteMeshData) {
    Engine::Mesh2D<double> quad;
    quad.vertices.push_back(Engine::WorldVertex2D<double>({9.0, 9.0}, 9.0));
    quad.indices.push_back(99);

    Engine::MeshPrimitive::GeneratePrimitiveMesh(Engine::MeshPrimitive2DType::Quad, 3.0, quad);
    EXPECT_EQ(4u, quad.vertices.size());
    EXPECT_EQ(6u, quad.indices.size());
    EXPECT_EQ(2u, quad.TriangleCount());
    EXPECT_NEAR(3.0, quad.vertices[0].color, 1e-12);
    ExpectMeshInsideUnitBounds(quad);

    Engine::Mesh3D<double> cube;
    Engine::MeshPrimitive::GenerateCube3D(5.0, cube);
    EXPECT_EQ(24u, cube.vertices.size());
    EXPECT_EQ(36u, cube.indices.size());
    EXPECT_EQ(12u, cube.TriangleCount());
    EXPECT_NEAR(5.0, cube.vertices[0].color, 1e-12);
    ExpectMeshInsideUnitBounds(cube);
}

TEST_CASE(MeshPrimitive_GeneratesAdditional2DAnd3DMeshes) {
    const Engine::Mesh2D<double> circle = Engine::MeshPrimitive::GenerateCircle2D(2.0, 8);
    EXPECT_EQ(9u, circle.vertices.size());
    EXPECT_EQ(24u, circle.indices.size());
    EXPECT_EQ(8u, circle.TriangleCount());
    ExpectMeshInsideUnitBounds(circle);
    ExpectMeshIndicesValid(circle);

    const Engine::Mesh3D<double> tetrahedron = Engine::MeshPrimitive::GenerateTetrahedron3D(3.0);
    EXPECT_EQ(12u, tetrahedron.vertices.size());
    EXPECT_EQ(12u, tetrahedron.indices.size());
    EXPECT_EQ(4u, tetrahedron.TriangleCount());
    ExpectMeshInsideUnitBounds(tetrahedron);
    ExpectMeshIndicesValid(tetrahedron);

    const Engine::Mesh3D<double> octahedron = Engine::MeshPrimitive::GenerateOctahedron3D(4.0);
    EXPECT_EQ(24u, octahedron.vertices.size());
    EXPECT_EQ(24u, octahedron.indices.size());
    EXPECT_EQ(8u, octahedron.TriangleCount());
    ExpectMeshInsideUnitBounds(octahedron);
    ExpectMeshIndicesValid(octahedron);

    Engine::Mesh3D<double> cylinder;
    Engine::MeshPrimitive::GenerateCylinder3D(5.0, cylinder, 8);
    EXPECT_EQ(50u, cylinder.vertices.size());
    EXPECT_EQ(96u, cylinder.indices.size());
    EXPECT_EQ(32u, cylinder.TriangleCount());
    ExpectMeshInsideUnitBounds(cylinder);
    ExpectMeshIndicesValid(cylinder);

    const Engine::Mesh3D<double> cone = Engine::MeshPrimitive::GenerateCone3D(6.0, 8);
    EXPECT_EQ(33u, cone.vertices.size());
    EXPECT_EQ(48u, cone.indices.size());
    EXPECT_EQ(16u, cone.TriangleCount());
    ExpectMeshInsideUnitBounds(cone);
    ExpectMeshIndicesValid(cone);

    const Engine::Mesh3D<double> sphere = Engine::MeshPrimitive::GenerateSphere3D(7.0, 8, 4);
    EXPECT_EQ(45u, sphere.vertices.size());
    EXPECT_EQ(144u, sphere.indices.size());
    EXPECT_EQ(48u, sphere.TriangleCount());
    ExpectMeshInsideUnitBounds(sphere);
    ExpectMeshIndicesValid(sphere);

    const Engine::Mesh3D<double> dispatched_sphere = Engine::MeshPrimitive::GeneratePrimitiveMesh(
        Engine::MeshPrimitive3DType::Sphere,
        8.0
    );
    EXPECT_TRUE(!dispatched_sphere.IsEmpty());
    ExpectMeshInsideUnitBounds(dispatched_sphere);
    ExpectMeshIndicesValid(dispatched_sphere);
}

TEST_CASE(MeshPrimitive_InvalidResolutionClearsOutput) {
    Engine::Mesh2D<double> circle = Engine::MeshPrimitive::GenerateQuad2D(1.0);
    Engine::MeshPrimitive::GenerateCircle2D(1.0, circle, 2);
    EXPECT_TRUE(circle.vertices.empty());
    EXPECT_TRUE(circle.indices.empty());

    Engine::Mesh3D<double> sphere = Engine::MeshPrimitive::GenerateCube3D(1.0);
    Engine::MeshPrimitive::GenerateSphere3D(1.0, sphere, 2, 1);
    EXPECT_TRUE(sphere.vertices.empty());
    EXPECT_TRUE(sphere.indices.empty());
}

TEST_CASE(World_2DRenderGeometrySkipsInvalidIndices) {
    Tests::CountingGraphics2D sink;

    std::vector<Engine::WorldVertex2D<double>> v = {
        Engine::WorldVertex2D<double>({0, 0}, 1),
        Engine::WorldVertex2D<double>({1, 0}, 2),
        Engine::WorldVertex2D<double>({0, 1}, 3),
        Engine::WorldVertex2D<double>({1, 1}, 4)
    };
    std::vector<int> idx = {
        0, 1, 2,
        0, 2, 3,
        -1, 0, 1,
        0, 1, 100
    };

    sink.RenderGeometry(v, idx);
    EXPECT_EQ(2, sink.triangles);
}

TEST_CASE(World_3DProjectorAnd4DEmbedderAdapters) {
    Tests::CountingGraphics2D sink2d;
    Engine::VertexDimensionConvertWorldGraphics<3, 2, double> projector(
        sink2d,
        [](const Engine::WorldVertex3D<double>& in, Engine::WorldVertex2D<double>& out) {
            out.position.x() = in.position.x() + 10;
            out.position.y() = in.position.y() - 5;
            out.color = in.color + 1;
        }
    );

    projector.RenderTriangle(
        Engine::WorldVertex3D<double>({1, 2, 3}, 4),
        Engine::WorldVertex3D<double>({2, 3, 4}, 5),
        Engine::WorldVertex3D<double>({3, 4, 5}, 6)
    );
    EXPECT_EQ(1, sink2d.triangles);
    EXPECT_NEAR(11.0, sink2d.lastA.position.x(), 1e-12);
    EXPECT_NEAR(-3.0, sink2d.lastA.position.y(), 1e-12);
    EXPECT_NEAR(5.0, sink2d.lastA.color, 1e-12);

    Tests::CountingGraphics4D sink4d;
    Engine::VertexDimensionConvertWorldGraphics<3, 4, double> embedder(
        sink4d,
        [](const Engine::WorldVertex3D<double>& in, Engine::WorldVertex4D<double>& out) {
            out.position.x() = in.position.x();
            out.position.y() = in.position.y();
            out.position.z() = in.position.z();
            out.position.w() = 1.0;
            out.color = in.color;
        }
    );

    embedder.RenderTriangle(
        Engine::WorldVertex3D<double>({ 1,  2,  3}, 7),
        Engine::WorldVertex3D<double>({ 0,  0,  1}, 8),
        Engine::WorldVertex3D<double>({-1, -2, -3}, 9)
    );
    EXPECT_EQ(1, sink4d.triangles);
    EXPECT_NEAR(1.0, sink4d.lastA.position.w(), 1e-12);
    EXPECT_NEAR(7.0, sink4d.lastA.color, 1e-12);
}

TEST_CASE(World_3DAnd4DPlaneClippingRejectsOutsideTriangle) {
    Engine::Interpolator<double> interp;

    Tests::CountingGraphics3D sink3d;
    Engine::PlaneClipWorldGraphics3D<double> clip3d(sink3d, interp, {Engine::WorldPlane3D(1, 0, 0, 0)}); // x >= 0
    clip3d.RenderTriangle(
        Engine::WorldVertex3D<double>({-2,  0, 0}, 1),
        Engine::WorldVertex3D<double>({-1,  1, 0}, 1),
        Engine::WorldVertex3D<double>({-1, -1, 0}, 1)
    );
    EXPECT_EQ(0, sink3d.triangles);

    Tests::CountingGraphics4D sink4d;
    Engine::PlaneClipWorldGraphics4D<double> clip4d(sink4d, interp, {Engine::WorldPlane4D(1, 0, 0, 0, 0)}); // x >= 0
    clip4d.RenderTriangle(
        Engine::WorldVertex4D<double>({-2,  0, 0, 1}, 1),
        Engine::WorldVertex4D<double>({-1,  1, 0, 1}, 1),
        Engine::WorldVertex4D<double>({-1, -1, 0, 1}, 1)
    );
    EXPECT_EQ(0, sink4d.triangles);
}
