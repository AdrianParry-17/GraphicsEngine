#include "Engine/Engine.h"
#include "common/TerminalBufferContext.h"
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <thread>

void RenderCube(Engine::Graphics3DPipeline<Engine::TexturedColor<double>>& pipeline) {
    // Helper function mapping 12 triangles forming a simple local cube at zero-point.
    
    std::vector<Engine::WorldVertex3D<Engine::TexturedColor<double>>> vertices = {
        // Front face (Z = 1), Color: 9
        Engine::WorldVertex3D<Engine::TexturedColor<double>>({-1, -1,  1}, {9, 0.0, 0.0}),
        Engine::WorldVertex3D<Engine::TexturedColor<double>>({ 1, -1,  1}, {9, 1.0, 0.0}),
        Engine::WorldVertex3D<Engine::TexturedColor<double>>({ 1,  1,  1}, {9, 1.0, 1.0}),
        Engine::WorldVertex3D<Engine::TexturedColor<double>>({-1,  1,  1}, {9, 0.0, 1.0}),
        // Back face (Z = -1), Color: 4
        Engine::WorldVertex3D<Engine::TexturedColor<double>>({-1, -1, -1}, {4, 0.0, 0.0}),
        Engine::WorldVertex3D<Engine::TexturedColor<double>>({ 1, -1, -1}, {4, 1.0, 0.0}),
        Engine::WorldVertex3D<Engine::TexturedColor<double>>({ 1,  1, -1}, {4, 1.0, 1.0}),
        Engine::WorldVertex3D<Engine::TexturedColor<double>>({-1,  1, -1}, {4, 0.0, 1.0}),
        // Left face (X = -1), Color: 5
        Engine::WorldVertex3D<Engine::TexturedColor<double>>({-1, -1, -1}, {5, 0.0, 0.0}),
        Engine::WorldVertex3D<Engine::TexturedColor<double>>({-1, -1,  1}, {5, 1.0, 0.0}),
        Engine::WorldVertex3D<Engine::TexturedColor<double>>({-1,  1,  1}, {5, 1.0, 1.0}),
        Engine::WorldVertex3D<Engine::TexturedColor<double>>({-1,  1, -1}, {5, 0.0, 1.0}),
        // Right face (X = 1), Color: 3
        Engine::WorldVertex3D<Engine::TexturedColor<double>>({ 1, -1, -1}, {3, 0.0, 0.0}),
        Engine::WorldVertex3D<Engine::TexturedColor<double>>({ 1, -1,  1}, {3, 1.0, 0.0}),
        Engine::WorldVertex3D<Engine::TexturedColor<double>>({ 1,  1,  1}, {3, 1.0, 1.0}),
        Engine::WorldVertex3D<Engine::TexturedColor<double>>({ 1,  1, -1}, {3, 0.0, 1.0}),
        // Top face (Y = 1), Color: 7
        Engine::WorldVertex3D<Engine::TexturedColor<double>>({-1,  1, -1}, {7, 0.0, 0.0}),
        Engine::WorldVertex3D<Engine::TexturedColor<double>>({ 1,  1, -1}, {7, 1.0, 0.0}),
        Engine::WorldVertex3D<Engine::TexturedColor<double>>({ 1,  1,  1}, {7, 1.0, 1.0}),
        Engine::WorldVertex3D<Engine::TexturedColor<double>>({-1,  1,  1}, {7, 0.0, 1.0}),
        // Bottom face (Y = -1), Color: 8
        Engine::WorldVertex3D<Engine::TexturedColor<double>>({-1, -1, -1}, {8, 0.0, 0.0}),
        Engine::WorldVertex3D<Engine::TexturedColor<double>>({ 1, -1, -1}, {8, 1.0, 0.0}),
        Engine::WorldVertex3D<Engine::TexturedColor<double>>({ 1, -1,  1}, {8, 1.0, 1.0}),
        Engine::WorldVertex3D<Engine::TexturedColor<double>>({-1, -1,  1}, {8, 0.0, 1.0})
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
    Engine::TexturedColorScalableInterpolator<double> tex_color_interpolator(color_interpolator);

    // - Texture Initialize
    //  + A simple 2x2 checker board pattern texture.
    Engine::FunctionTexture<double> func_tex([](int x, int y, double& res) {
        if (x < 0 || x > 1 || y < 0 || y > 1) return false;
        res = ((x + y) % 2 == 0) ? 9 : 3;
        return true;
    });
    //  + Making the texture repeated
    Engine::RepeatedTexture<double> rep_tex(func_tex, Engine::Rectangle(0, 0, 2, 2));
    //  + Adapting to 2D texture mapping, using Nearest Neighbor mapping algorithm
    Engine::NativeTexture2D<double> native_tex2d(rep_tex, color_interpolator, Engine::NativeTextureAdaptMethod::NearestNeighbor);
    //  + UV mapping (trimmed with NearZero to prevent hitting the exclusive upper bound 'b'):
    //    - Input (0, 0) -> (1, 1)
    //    - Output (0, 0) -> (2 - epsilon, 2 - epsilon)
    Engine::ViewportTransformTexture2D<double> viewport_tex2d(
        native_tex2d, 
        Engine::RangeMapper(0, 1, 0, 2), 
        Engine::RangeMapper(0, 1, 0, 2)
    );

    // - Graphics Initialize
    TerminalBufferContext context(120, 30);
    Engine::ContextGraphics<double> context_g(context);
    //   + Add texture sampling layer
    Engine::TextureSamplingGraphics<double> tex_sampling_g(context_g);

    // Setup 3D graphics pipeline inside bounding rect
    Engine::Graphics3DPipeline<Engine::TexturedColor<double>> pipeline3d(
        tex_sampling_g, tex_color_interpolator, Engine::Rectangle(0, 0, 120, 30)
    );

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
        
        // Cube #1 Generation (no texture)
        tex_sampling_g.SetTargetTexture(nullptr);
        
        pipeline3d.PushMatrix();
        pipeline3d.Transform(
            Engine::Vector3(-2, 0, -8),
            Engine::Vector3(0, (5 * i) * _deg_to_rad, 0), // Rotate continuously on Y axis
            Engine::Vector3(1, 0.5, 1)                      // Scale Y to flat block shape
        );
        RenderCube(pipeline3d);
        pipeline3d.PopMatrix();

        // Cube #2 Generation (with texture)
        tex_sampling_g.SetTargetTexture(&viewport_tex2d);
        
        pipeline3d.PushMatrix();
        pipeline3d.Transform(
            Engine::Vector3(0.08 * i, 0, -5),             // Move dynamically horizontally
            Engine::Vector3(1, 1, 0) * (3.14 * 0.025 * i), // Complex combined rotation
            Engine::Vector3(1, 0.5, 1)
        );
        RenderCube(pipeline3d);
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
