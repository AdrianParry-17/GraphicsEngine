#include "Engine/Engine.h"
#include "common/TerminalBufferContext.h"

#include <cmath>
#include <cstddef>

int main() {
    const int width = 80;
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

    Engine::Mesh3D<double> cube;
    Engine::MeshPrimitive::GenerateCube3D(1.0, cube);

    const double face_colors[6] = {9.0, 2.0, 4.0, 6.0, 8.0, 3.0};
    for (std::size_t face = 0; face < 6; ++face) {
        for (std::size_t vertex = 0; vertex < 4; ++vertex)
            cube.vertices[face * 4 + vertex].color = face_colors[face];
    }

    pipeline.LoadIdentity();
    pipeline.Perspective(
        50.0 * degree_to_radian,
        static_cast<double>(width) / height,
        0.01,
        100.0
    );
    pipeline.Transform(
        Engine::Vector3(0.0, 0.0, -5.0),
        Engine::Vector3(20.0, -30.0, 0.0) * degree_to_radian,
        Engine::Vector3(1.4, 1.4, 1.4)
    );
    pipeline.EnableDepthBuffer();
    pipeline.ClearDepthBuffer();

    pipeline.RenderMesh(cube);

    context.Write();
    context.Report();
    return 0;
}
