#include "Engine/Engine.h"
#include "common/TerminalBufferContext.h"
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <thread>

void RenderCube(Engine::IWorldGraphics3D<double> &graphics) {
    // Shared primitive defining an unscaled cube bounding box.
    // Demonstrates compiling a centralized vertex buffer down through Custom Pipelines.
    
    std::vector<Engine::WorldVertex3D<double>> vertices = {
        Engine::WorldVertex3D<double>({-1, -1, -1}, 3), // 0: bottom-left-back
        Engine::WorldVertex3D<double>({ 1, -1, -1}, 4), // 1: bottom-right-back
        Engine::WorldVertex3D<double>({ 1,  1, -1}, 5), // 2: top-right-back
        Engine::WorldVertex3D<double>({-1,  1, -1}, 6), // 3: top-left-back
        Engine::WorldVertex3D<double>({-1, -1,  1}, 4), // 4: bottom-left-front
        Engine::WorldVertex3D<double>({ 1, -1,  1}, 5), // 5: bottom-right-front
        Engine::WorldVertex3D<double>({ 1,  1,  1}, 9), // 6: top-right-front
        Engine::WorldVertex3D<double>({-1,  1,  1}, 8)  // 7: top-left-front
    };

    std::vector<int> indices = {
        4, 5, 6, 4, 6, 7, // Front
        1, 0, 3, 1, 3, 2, // Back
        0, 4, 7, 0, 7, 3, // Left
        5, 1, 2, 5, 2, 6, // Right
        3, 7, 6, 3, 6, 2, // Top
        0, 1, 5, 0, 5, 4  // Bottom
    };

    graphics.RenderGeometry(vertices, indices);
}

int main() {
    TerminalBufferContext context(120, 30);
    Engine::Interpolator<double> color_interpolator;

    // Below maps an entire custom manual 3D raster pipeline utilizing low-level interface links.

    // 1. Initial 2D primitive render linkage
    Engine::ContextGraphics<double> context_g(context);
    Engine::RendererGraphics<double> renderer_g(context_g, color_interpolator);
    Engine::RendererGraphics2D<double> renderer_g2d(renderer_g);
    
    // Transform coordinates logic onto raster scale
    Engine::ViewportTransformWorldGraphics2D<double> viewport_g2d(
        renderer_g2d, {Engine::RangeMapper(-1, 1, 0, 119), Engine::RangeMapper(1, -1, 0, 29)}
    );

    // Provide 2D safe guard viewport constraint clipping planes
    Engine::PlaneClipWorldGraphics2D<double> clipped_g2d(
        viewport_g2d, color_interpolator,
        {
            Engine::WorldPlane2D( 1,  0, 1),  // x >= -1
            Engine::WorldPlane2D(-1,  0, 1),  // x <= 1
            Engine::WorldPlane2D( 0,  1, 1),  // y >= -1
            Engine::WorldPlane2D( 0, -1, 1),  // y <= 1
        }
    );

    // 2. 3D layer mapping links
    Engine::VertexDimensionConvertWorldGraphics<3, 2, double> project2d_g3d(
        clipped_g2d,
        [](const Engine::WorldVertex3D<double> &in, Engine::WorldVertex2D<double> &out) {
            out.position = in.position;
            out.color = in.color;
        }
    );

    // Setup W-Component processing in temporary 4-Dimensional phase for depth math 
    Engine::VertexDimensionConvertWorldGraphics<4, 3, double> project3d_g4d(
        project2d_g3d,
        [](const Engine::WorldVertex4D<double> &in, Engine::WorldVertex3D<double> &out) {
            double w_factor = in.position.w();
            if (Engine::NumericConstants::IsNearZero(w_factor))
                w_factor = Engine::NumericConstants::NearZero;
            w_factor = 1.0 / w_factor; // homogenization

            out.position = in.position; out.position *= w_factor;
            out.color = in.color;
        }
    );

    // Execute comprehensive frustum cutoff on mapped coordinates
    Engine::PlaneClipWorldGraphics4D<double> clipped_g4d(
        project3d_g4d, color_interpolator,
        {
            Engine::WorldPlane4D(-1,  0,  0, 1, 0), Engine::WorldPlane4D(1, 0, 0, 1, 0),  // X limits 
            Engine::WorldPlane4D( 0, -1,  0, 1, 0), Engine::WorldPlane4D(0, 1, 0, 1, 0),  // Y limits
            Engine::WorldPlane4D( 0,  0, -1, 1, 0), Engine::WorldPlane4D(0, 0, 1, 1, 0)   // Z limits
        }
    );

    Engine::MatrixWorldGraphics4D<double> matrix_g4d(clipped_g4d);

    // Initial hook embedding logical 3-space coordinates up to 4D to trigger pipeline backwards
    Engine::VertexDimensionConvertWorldGraphics<3, 4, double> embed4d_g3d(
        matrix_g4d,
        [](const Engine::WorldVertex3D<double> &in, Engine::WorldVertex4D<double> &out) {
            out.position = in.position; out.position.w() = 1;
            out.color = in.color;
        }
    );

    const double _pi = std::acos(-1);
    
    // Core Logic execution
    matrix_g4d.LoadIdentity();
    matrix_g4d.MultiplyMatrix(Engine::Transform3D::GetPerspectiveProjection(45 * _pi / 180, 120.0 / 30.0, 0.01, 100));

    for (int i = -100; i <= 100; ++i) {
        
        // Cube 1
        matrix_g4d.PushMatrix();
        matrix_g4d.MultiplyMatrix(
            Engine::Transform3D::GetTransformation(
                Engine::Vector3(-2, 0, -12 + std::abs(i) * 0.05),
                Engine::Vector3(0, 3.14 * 0.05 * i, 0),
                Engine::Vector3(1, 0.5, 1)
            )
        );
        RenderCube(embed4d_g3d);
        matrix_g4d.PopMatrix();

        // Cube 2
        matrix_g4d.PushMatrix();
        matrix_g4d.MultiplyMatrix(
            Engine::Transform3D::GetTransformation(
                Engine::Vector3(0.08 * i, 0, -5),
                Engine::Vector3(1, 1, 0) * (3.14 * 0.025 * i),
                Engine::Vector3(1, 0.5, 1)
            )
        );
        
        RenderCube(embed4d_g3d);
        matrix_g4d.PopMatrix();

        std::system("clear");
        context.Write();

        context.Clear();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    context.Report();
    return 0;
}
