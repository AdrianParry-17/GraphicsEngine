#include "Engine/Engine.h"
#include "common/TerminalBufferContext.h"

#include <cmath>

namespace {
    void RenderMeshAt(
        Engine::Graphics3DPipeline<double>& pipeline,
        const Engine::Mesh3D<double>& mesh,
        double x,
        const Engine::Vector3& rotation
    ) {
        pipeline.PushMatrix();
        pipeline.Transform(
            Engine::Vector3(x, 0.0, -5.0),
            rotation,
            Engine::Vector3(1.3, 1.3, 1.3)
        );
        pipeline.RenderMesh(mesh);
        pipeline.PopMatrix();
    }
}

int main() {
    const int width = 120;
    const int height = 30;
    const double degree_to_radian = std::acos(-1.0) / 180.0;

    TerminalBufferContext context(width, height);
    Engine::ContextGraphics<double> graphics(context);
    Engine::ScalableInterpolator<double> color_interpolator;
    Engine::Graphics3DPipeline<double> pipeline(
        graphics,
        color_interpolator,
        Engine::Rectangle(0, 0, width, height)
    );

    const Engine::Mesh3D<double> tetrahedron = Engine::MeshPrimitive::GenerateTetrahedron3D(3.0);
    const Engine::Mesh3D<double> octahedron = Engine::MeshPrimitive::GenerateOctahedron3D(5.0);
    const Engine::Mesh3D<double> cylinder = Engine::MeshPrimitive::GenerateCylinder3D(7.0, 16);
    const Engine::Mesh3D<double> cone = Engine::MeshPrimitive::GenerateCone3D(8.0, 16);
    const Engine::Mesh3D<double> sphere = Engine::MeshPrimitive::GenerateSphere3D(9.0, 16, 8);

    pipeline.LoadIdentity();
    pipeline.Perspective(
        50.0 * degree_to_radian,
        static_cast<double>(width) / height,
        0.01,
        100.0
    );
    pipeline.EnableDepthBuffer();
    pipeline.ClearDepthBuffer();

    RenderMeshAt(pipeline, tetrahedron, -3.0, Engine::Vector3(15.0, -25.0, 0.0) * degree_to_radian);
    RenderMeshAt(pipeline, octahedron, -1.5, Engine::Vector3(15.0, 25.0, 0.0) * degree_to_radian);
    RenderMeshAt(pipeline, cylinder, 0.0, Engine::Vector3(15.0, -20.0, 0.0) * degree_to_radian);
    RenderMeshAt(pipeline, cone, 1.5, Engine::Vector3(10.0, 20.0, 0.0) * degree_to_radian);
    RenderMeshAt(pipeline, sphere, 3.0, Engine::Vector3(15.0, -20.0, 0.0) * degree_to_radian);

    context.Write();
    context.Report();
    return 0;
}
