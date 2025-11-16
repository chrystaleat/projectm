/**
 * @file QuaternionJuliaDemo.cpp
 * @brief Demo showcasing 4D Quaternion Julia Set fractals
 *
 * Generates TRUE 4D fractals using quaternion mathematics!
 * The most mind-bending feature - fractals from hyperspace! 🤯
 *
 * Usage:
 *   - Create QuaternionJulia generator
 *   - Choose preset or custom c parameter
 *   - Generate point cloud
 *   - Project to 2D and render
 */

#include <Renderer/QuaternionJulia.hpp>
#include <Renderer/Vec4D.hpp>
#include <Renderer/HyperdimensionalProjection.hpp>
#include <Audio/FrameAudioData.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <iomanip>

using namespace libprojectM::Renderer;
using namespace libprojectM::Audio;

/**
 * @brief Quaternion Julia Set Fractal Demo
 *
 * Experience hyperspace fractals!
 */
class QuaternionJuliaFractalDemo
{
public:
    QuaternionJuliaFractalDemo()
    {
        std::cout << "=== QUATERNION JULIA SET DEMO ===" << std::endl;
        std::cout << "Quaternions: 4D extension of complex numbers" << std::endl;
        std::cout << "q = a + bi + cj + dk (where i²=j²=k²=-1)" << std::endl;
    }

    void DemoClassicJuliaSet()
    {
        std::cout << "\n🔮 CLASSIC JULIA SET" << std::endl;
        std::cout << "c = (-0.8, 0.156, 0, 0)" << std::endl;

        julia.SetPreset(JuliaPreset::Classic);
        julia.GeneratePointCloud(5000, 2.0f);

        AnalyzeFractal("Classic");
    }

    void DemoSpiralJuliaSet()
    {
        std::cout << "\n🌀 SPIRAL JULIA SET" << std::endl;
        std::cout << "c = (-0.4, -0.59, -0.59, 0)" << std::endl;
        std::cout << "Uses all 4D components for complex spiraling!" << std::endl;

        julia.SetPreset(JuliaPreset::Spiral);
        julia.GeneratePointCloud(5000, 2.0f);

        AnalyzeFractal("Spiral");
    }

    void DemoDragonJuliaSet()
    {
        std::cout << "\n🐉 DRAGON JULIA SET" << std::endl;
        std::cout << "c = (-0.123, 0.745, 0, 0)" << std::endl;

        julia.SetPreset(JuliaPreset::Dragon);
        julia.GeneratePointCloud(5000, 2.0f);

        AnalyzeFractal("Dragon");
    }

    void DemoBouquetJuliaSet()
    {
        std::cout << "\n🌺 BOUQUET JULIA SET" << std::endl;
        std::cout << "c = (0.285, 0, 0, 0.53)" << std::endl;
        std::cout << "Uses the k component (pure 4D magic!)" << std::endl;

        julia.SetPreset(JuliaPreset::Bouquet);
        julia.GeneratePointCloud(5000, 2.0f);

        AnalyzeFractal("Bouquet");
    }

    void DemoChaoticJuliaSet()
    {
        std::cout << "\n⚡ CHAOTIC JULIA SET" << std::endl;
        std::cout << "c = (-0.2, 0.8, 0, 0)" << std::endl;
        std::cout << "Near the edge of stability - extremely complex!" << std::endl;

        julia.SetPreset(JuliaPreset::Chaotic);
        julia.GeneratePointCloud(5000, 2.0f);

        AnalyzeFractal("Chaotic");
    }

    void DemoSymmetricalJuliaSet()
    {
        std::cout << "\n✨ SYMMETRICAL JULIA SET" << std::endl;
        std::cout << "c = (-0.213, -0.041, -0.563, 0)" << std::endl;

        julia.SetPreset(JuliaPreset::Symmetrical);
        julia.GeneratePointCloud(5000, 2.0f);

        AnalyzeFractal("Symmetrical");
    }

    void DemoCustomParameters()
    {
        std::cout << "\n🎨 CUSTOM JULIA SET" << std::endl;

        // Create custom quaternion
        Quaternion customC(-0.5f, 0.3f, 0.2f, 0.1f);

        std::cout << "Custom c = (" << customC.a << ", "
                  << customC.b << ", " << customC.c << ", "
                  << customC.d << ")" << std::endl;

        julia.SetCParameter(customC);
        julia.GeneratePointCloud(5000, 2.0f);

        AnalyzeFractal("Custom");
    }

    void DemoAudioReactive()
    {
        std::cout << "\n🎵 AUDIO-REACTIVE MORPHING" << std::endl;

        julia.SetPreset(JuliaPreset::Classic);

        std::cout << "Morphing fractal with audio..." << std::endl;

        for (int frame = 0; frame < 60; ++frame)
        {
            // Simulate audio
            float energy = 0.5f + 0.3f * std::sin(frame * 0.1f);
            float beat = (frame % 20 == 0) ? 1.0f : 0.0f;
            float bass = 0.5f + 0.2f * std::cos(frame * 0.08f);

            // Apply audio modulation
            julia.SetAudioModulation(energy, beat, bass);

            // Regenerate (in real app, do this less frequently)
            if (frame % 10 == 0)
            {
                julia.GeneratePointCloud(1000, 2.0f);

                std::cout << "  Frame " << std::setw(2) << frame
                          << ": Energy=" << std::fixed << std::setprecision(2) << energy
                          << ", Beat=" << beat
                          << ", Size=" << julia.GetRenderSize()
                          << ", Points=" << julia.GetPoints().size() << std::endl;
            }
        }

        std::cout << "✓ Audio morphing complete!" << std::endl;
    }

    void DemoProjectionAndRendering()
    {
        std::cout << "\n📐 PROJECTION TO 2D" << std::endl;

        julia.SetPreset(JuliaPreset::Spiral);
        julia.GeneratePointCloud(1000, 2.0f);

        const auto& points4D = julia.GetPoints();

        // Project to 3D first
        std::cout << "Projecting " << points4D.size() << " 4D points to 3D..." << std::endl;

        std::vector<glm::vec3> points3D;
        float cameraDistance4D = 5.0f;

        for (const auto& p4d : points4D)
        {
            glm::vec3 p3d = Project4Dto3D(p4d, cameraDistance4D);
            points3D.push_back(p3d);
        }

        std::cout << "✓ Projected to 3D!" << std::endl;

        // Then project to 2D
        glm::mat4 projectionMatrix = glm::perspective(
            glm::radians(60.0f),
            16.0f / 9.0f,
            0.1f,
            100.0f
        );

        std::vector<glm::vec2> points2D;

        for (const auto& p3d : points3D)
        {
            glm::vec2 p2d = Project3Dto2D(p3d, projectionMatrix, 1920, 1080);
            points2D.push_back(p2d);
        }

        std::cout << "✓ Projected to 2D screen coordinates!" << std::endl;

        // Show sample points
        std::cout << "\nFirst 5 projected points:" << std::endl;
        for (size_t i = 0; i < 5 && i < points2D.size(); ++i)
        {
            std::cout << "  Point " << i << ": ("
                      << std::fixed << std::setprecision(1)
                      << points2D[i].x << ", " << points2D[i].y << ")" << std::endl;
        }

        std::cout << "\nRendering options:" << std::endl;
        std::cout << "  1. Point cloud (GL_POINTS with glow)" << std::endl;
        std::cout << "  2. Connect nearby points (spatial web)" << std::endl;
        std::cout << "  3. Instanced spheres (volumetric look)" << std::endl;
        std::cout << "  4. Density field (raymarching)" << std::endl;
    }

    void DemoIterationAnalysis()
    {
        std::cout << "\n🔬 ITERATION ANALYSIS" << std::endl;

        julia.SetPreset(JuliaPreset::Classic);
        Quaternion c = julia.GetCParameter();

        // Test different points
        std::vector<Quaternion> testPoints = {
            Quaternion(0.0f, 0.0f, 0.0f, 0.0f),  // Origin
            Quaternion(1.0f, 0.0f, 0.0f, 0.0f),  // On axis
            Quaternion(0.5f, 0.5f, 0.5f, 0.5f),  // Diagonal
            Quaternion(2.0f, 2.0f, 2.0f, 2.0f),  // Far out
        };

        std::cout << "Testing points with c = (" << c.a << ", " << c.b << ", "
                  << c.c << ", " << c.d << ")" << std::endl;

        for (size_t i = 0; i < testPoints.size(); ++i)
        {
            const auto& p = testPoints[i];
            int iterations = julia.IteratePoint(p, c, 100);

            std::cout << "  Point (" << p.a << ", " << p.b << ", "
                      << p.c << ", " << p.d << "): ";

            if (iterations >= 100)
            {
                std::cout << "IN SET (bounded)" << std::endl;
            }
            else
            {
                std::cout << "escaped after " << iterations << " iterations" << std::endl;
            }
        }
    }

private:
    QuaternionJulia julia;

    void AnalyzeFractal(const std::string& name)
    {
        const auto& points = julia.GetPoints();

        if (points.empty())
        {
            std::cout << "⚠ No points in Julia set!" << std::endl;
            return;
        }

        // Calculate statistics
        float avgMagnitude = 0.0f;
        float maxMagnitude = 0.0f;
        Vec4D centroid(0.0f, 0.0f, 0.0f, 0.0f);

        for (const auto& p : points)
        {
            float mag = p.Length();
            avgMagnitude += mag;
            maxMagnitude = std::max(maxMagnitude, mag);

            centroid = centroid + p;
        }

        avgMagnitude /= points.size();
        centroid = centroid / static_cast<float>(points.size());

        std::cout << "\n✓ " << name << " Julia Set Generated!" << std::endl;
        std::cout << "  Points in set: " << points.size() << std::endl;
        std::cout << "  Avg magnitude: " << std::fixed << std::setprecision(3) << avgMagnitude << std::endl;
        std::cout << "  Max magnitude: " << maxMagnitude << std::endl;
        std::cout << "  Centroid: (" << centroid.x << ", " << centroid.y << ", "
                  << centroid.z << ", " << centroid.w << ")" << std::endl;
    }
};

/**
 * @brief Main demo entry point
 */
int main()
{
    QuaternionJuliaFractalDemo demo;

    // Demo all presets
    demo.DemoClassicJuliaSet();
    demo.DemoSpiralJuliaSet();
    demo.DemoDragonJuliaSet();
    demo.DemoBouquetJuliaSet();
    demo.DemoChaoticJuliaSet();
    demo.DemoSymmetricalJuliaSet();

    // Advanced demos
    demo.DemoCustomParameters();
    demo.DemoIterationAnalysis();
    demo.DemoAudioReactive();
    demo.DemoProjectionAndRendering();

    std::cout << "\n=== VISUAL PRESET GUIDE ===" << std::endl;
    std::cout << "\nBest Julia Sets for Different Vibes:" << std::endl;
    std::cout << "  🌌 Cosmic/Space: Classic or Symmetrical" << std::endl;
    std::cout << "  🌀 Psychedelic: Spiral or Chaotic" << std::endl;
    std::cout << "  🐉 Organic/Natural: Dragon or Bouquet" << std::endl;
    std::cout << "  ⚡ Intense/Energy: Chaotic with audio modulation" << std::endl;

    std::cout << "\nRender Quality Settings:" << std::endl;
    std::cout << "  Low: 1000 samples, 50 iterations" << std::endl;
    std::cout << "  Medium: 5000 samples, 100 iterations" << std::endl;
    std::cout << "  High: 10000 samples, 150 iterations" << std::endl;
    std::cout << "  Ultra: 50000 samples, 200 iterations (GPU recommended)" << std::endl;

    std::cout << "\nAnimation Ideas:" << std::endl;
    std::cout << "  1. Morph c parameter with music (slow drift)" << std::endl;
    std::cout << "  2. Rotate fractal in 4D space" << std::endl;
    std::cout << "  3. Pulse size with beat" << std::endl;
    std::cout << "  4. Transition between presets on measure" << std::endl;
    std::cout << "  5. Color map by escape time (outside set)" << std::endl;

    std::cout << "\n✓ Demo complete! You've experienced hyperspace fractals! 🌀✨" << std::endl;

    return 0;
}
