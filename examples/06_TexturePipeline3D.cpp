#include "Engine/Engine.h"
#include "common/TerminalBufferContext.h"
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <thread>

struct TexturedColor { double uv_x, uv_y, color; };

void RenderCube_pipe(Engine::Graphics3DPipeline<TexturedColor> &pipeline) {
    // Helper function mapping 12 triangles forming a simple local cube at zero-point.
    
    std::vector<Engine::WorldVertex3D<TexturedColor>> vertices = {
        // Front face (Z = 1), Color: 9
        Engine::WorldVertex3D<TexturedColor>({-1, -1,  1}, {0.0, 0.0, 9}),
        Engine::WorldVertex3D<TexturedColor>({ 1, -1,  1}, {1.0, 0.0, 9}),
        Engine::WorldVertex3D<TexturedColor>({ 1,  1,  1}, {1.0, 1.0, 9}),
        Engine::WorldVertex3D<TexturedColor>({-1,  1,  1}, {0.0, 1.0, 9}),
        // Back face (Z = -1), Color: 4
        Engine::WorldVertex3D<TexturedColor>({-1, -1, -1}, {0.0, 0.0, 4}),
        Engine::WorldVertex3D<TexturedColor>({ 1, -1, -1}, {1.0, 0.0, 4}),
        Engine::WorldVertex3D<TexturedColor>({ 1,  1, -1}, {1.0, 1.0, 4}),
        Engine::WorldVertex3D<TexturedColor>({-1,  1, -1}, {0.0, 1.0, 4}),
        // Left face (X = -1), Color: 5
        Engine::WorldVertex3D<TexturedColor>({-1, -1, -1}, {0.0, 0.0, 5}),
        Engine::WorldVertex3D<TexturedColor>({-1, -1,  1}, {1.0, 0.0, 5}),
        Engine::WorldVertex3D<TexturedColor>({-1,  1,  1}, {1.0, 1.0, 5}),
        Engine::WorldVertex3D<TexturedColor>({-1,  1, -1}, {0.0, 1.0, 5}),
        // Right face (X = 1), Color: 6
        Engine::WorldVertex3D<TexturedColor>({ 1, -1, -1}, {0.0, 0.0, 3}),
        Engine::WorldVertex3D<TexturedColor>({ 1, -1,  1}, {1.0, 0.0, 3}),
        Engine::WorldVertex3D<TexturedColor>({ 1,  1,  1}, {1.0, 1.0, 3}),
        Engine::WorldVertex3D<TexturedColor>({ 1,  1, -1}, {0.0, 1.0, 3}),
        // Top face (Y = 1), Color: 7
        Engine::WorldVertex3D<TexturedColor>({-1,  1, -1}, {0.0, 0.0, 7}),
        Engine::WorldVertex3D<TexturedColor>({ 1,  1, -1}, {1.0, 0.0, 7}),
        Engine::WorldVertex3D<TexturedColor>({ 1,  1,  1}, {1.0, 1.0, 7}),
        Engine::WorldVertex3D<TexturedColor>({-1,  1,  1}, {0.0, 1.0, 7}),
        // Bottom face (Y = -1), Color: 8
        Engine::WorldVertex3D<TexturedColor>({-1, -1, -1}, {0.0, 0.0, 8}),
        Engine::WorldVertex3D<TexturedColor>({ 1, -1, -1}, {1.0, 0.0, 8}),
        Engine::WorldVertex3D<TexturedColor>({ 1, -1,  1}, {1.0, 1.0, 8}),
        Engine::WorldVertex3D<TexturedColor>({-1, -1,  1}, {0.0, 1.0, 8})
    };

    std::vector<int> indices = {
        0,  1,  2,  0,  2,  3,  // Front
        5,  4,  7,  5,  7,  6,  // Back
        8,  9, 10,  8, 10, 11,  // Left
        13, 12, 15, 13, 15, 14, // Right
        16, 19, 18, 16, 18, 17, // Top
        20, 21, 22, 20, 22, 23  // Bottom
    };

    pipeline.RenderGeometry(vertices, indices);
}

int main() {
    Engine::ScalableInterpolator<double> color_interpolator;
    Engine::FunctionScalableInterpolator<TexturedColor> tex_color_interpolator(
        [&](const TexturedColor& in, double scalar, TexturedColor& res) {
            res.uv_x = in.uv_x * scalar;
            res.uv_y = in.uv_y * scalar;
            color_interpolator.Scale(in.color, scalar, res.color);
        },
        [&](const TexturedColor& a, const TexturedColor& b, double t, TexturedColor& res) {
            res.uv_x = a.uv_x + (b.uv_x - a.uv_x) * t;
            res.uv_y = a.uv_y + (b.uv_y - a.uv_y) * t;
            color_interpolator.Linear(a.color, b.color, t, res.color);
        },
        [&](const TexturedColor& a, const TexturedColor& b, const TexturedColor& c, double wa, double wb, double wc, TexturedColor& res) {
            res.uv_x = a.uv_x * wa + b.uv_x * wb + c.uv_x * wc;
            res.uv_y = a.uv_y * wa + b.uv_y * wb + c.uv_y * wc;
            color_interpolator.Triangle(a.color, b.color, c.color, wa, wb, wc, res.color);
        }
    );

    // - Texture Initialize
    //  + A infinite checker board pattern texture.
    Engine::FunctionTexture<double> func_tex([](int x, int y, double& res) { res = ((x + y) % 2 == 0) ? 9 : 3; return true; });
    Engine::NativeTexture2D<double> native_tex2d(func_tex, color_interpolator, Engine::NativeTextureAdaptMethod::NearestNeighbor);
    Engine::ViewportTransformTexture2D<double> viewport_tex2d(
        native_tex2d, Engine::RangeMapper(0, 1, 0, 2), Engine::RangeMapper(0, 1, 0, 2)
    );

    // - Graphics Initialize
    TerminalBufferContext context(120, 30);
    Engine::ContextGraphics<double> context_g(context);
    Engine::FunctionGraphics<TexturedColor> func_g([&](int x, int y, const TexturedColor& c) {
        double color;
        if (viewport_tex2d.GetColor(c.uv_x, c.uv_y, color))
            return context_g.DrawPoint(x, y, color);
        return context_g.DrawPoint(x, y, c.color);
    });


    // Setup 3D graphics pipeline inside bounding rect
    Engine::Graphics3DPipeline<TexturedColor> pipeline3d(
        func_g, tex_color_interpolator, Engine::Rectangle(0, 0, 120, 30));

    // Prepare global logic bindings
    const double _deg_to_rad = std::acos(-1) / 180.0;
    
    pipeline3d.EnablePerspectiveCorrection();
    
    // Define viewpoint base metrics
    pipeline3d.LoadIdentity();
    pipeline3d.Perspective(45 * _deg_to_rad, 120.0 / 30.0, 0.01, 1000);
    pipeline3d.EnableDepthBuffer();

    // Main interaction simulation
    for (int i = -100; i <= 100; ++i) {
        pipeline3d.ClearDepthBuffer();
        
        // Cube #1 Generation
        pipeline3d.PushMatrix();
        pipeline3d.Transform(
            Engine::Vector3(-2, 0, -8),
            Engine::Vector3(0, (5 * i) * _deg_to_rad, 0), // Rotate continuously on Y axis
            Engine::Vector3(1, 0.5, 1)                      // Scale Y to flat block shape
        );
        RenderCube_pipe(pipeline3d);
        pipeline3d.PopMatrix();

        // Cube #2 Generation
        pipeline3d.PushMatrix();
        pipeline3d.Transform(
            Engine::Vector3(0.08 * i, 0, -5),             // Move dynamically horizontally
            Engine::Vector3(1, 1, 0) * (3.14 * 0.025 * i), // Complex combined rotation
            Engine::Vector3(1, 0.5, 1)
        );
        RenderCube_pipe(pipeline3d);
        pipeline3d.PopMatrix();

        // Flush text display sequence to user
        std::system("clear");
        context.Write();
        context.Clear(); // reset backend buffer
        
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    context.Report();
    return 0;
}
