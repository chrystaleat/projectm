/**
 * @file ReactionDiffusionDemo.cpp
 * @brief Demo showcasing Reaction-Diffusion pattern generation
 *
 * Creates organic patterns like leopard spots, zebra stripes, and
 * spiral waves using the Gray-Scott model!
 *
 * Usage:
 *   - Create ReactionDiffusion grid
 *   - Choose preset pattern
 *   - Seed initial points
 *   - Update simulation
 *   - Export as texture for rendering
 */

#include <Renderer/ReactionDiffusion.hpp>
#include <Audio/FrameAudioData.hpp>

#include <iostream>
#include <iomanip>

using namespace libprojectM::Renderer;
using namespace libprojectM::Audio;

/**
 * @brief Reaction-Diffusion Pattern Demo
 *
 * Creates mesmerizing organic patterns!
 */
class ReactionDiffusionPatternDemo
{
public:
    ReactionDiffusionPatternDemo()
        : rd(128, 128)  // 128x128 grid
    {
        std::cout << "=== REACTION-DIFFUSION DEMO ===" << std::endl;
        std::cout << "Grid size: " << rd.GetWidth() << "x" << rd.GetHeight() << std::endl;
    }

    void DemoSpots()
    {
        std::cout << "\n🐆 PATTERN: Leopard Spots" << std::endl;
        std::cout << "Parameters: F=0.055, k=0.062" << std::endl;

        rd.Reset();
        rd.SetPreset(ReactionDiffusionPreset::Spots);
        rd.SeedRandom(10, 5);

        RunSimulation(500);
        AnalyzePattern("Spots");
    }

    void DemoStripes()
    {
        std::cout << "\n🦓 PATTERN: Zebra Stripes" << std::endl;
        std::cout << "Parameters: F=0.035, k=0.065" << std::endl;

        rd.Reset();
        rd.SetPreset(ReactionDiffusionPreset::Stripes);
        rd.SeedRandom(15, 3);

        RunSimulation(800);
        AnalyzePattern("Stripes");
    }

    void DemoSpirals()
    {
        std::cout << "\n🌀 PATTERN: Spiral Waves" << std::endl;
        std::cout << "Parameters: F=0.014, k=0.054" << std::endl;

        rd.Reset();
        rd.SetPreset(ReactionDiffusionPreset::Spirals);
        rd.SeedCenter(10);

        RunSimulation(1000);
        AnalyzePattern("Spirals");
    }

    void DemoChaos()
    {
        std::cout << "\n⚡ PATTERN: Chaotic" << std::endl;
        std::cout << "Parameters: F=0.062, k=0.061" << std::endl;

        rd.Reset();
        rd.SetPreset(ReactionDiffusionPreset::Chaos);
        rd.SeedRandom(20, 4);

        RunSimulation(300);
        AnalyzePattern("Chaos");
    }

    void DemoAudioReactive()
    {
        std::cout << "\n🎵 AUDIO-REACTIVE PATTERNS" << std::endl;

        rd.Reset();
        rd.SetPreset(ReactionDiffusionPreset::Waves);
        rd.SeedRandom(5, 8);

        std::cout << "Simulating with audio modulation..." << std::endl;

        for (int i = 0; i < 500; ++i)
        {
            // Simulate audio varying over time
            float energy = 0.5f + 0.3f * std::sin(i * 0.05f);
            float beat = (i % 30 == 0) ? 1.0f : 0.0f;

            rd.SetAudioModulation(energy, beat, 0.5f);
            rd.Update(0.016f);

            if (i % 100 == 0)
            {
                std::cout << "  Frame " << i << ": Energy=" << std::fixed
                          << std::setprecision(2) << energy
                          << ", Beat=" << beat << std::endl;
            }
        }

        AnalyzePattern("Audio-Reactive");
    }

    void ExportTexture()
    {
        std::cout << "\n📸 EXPORTING TEXTURE" << std::endl;

        auto textureData = rd.GetTextureData();

        std::cout << "Texture data size: " << textureData.size() << " floats" << std::endl;
        std::cout << "Format: RGB (width * height * 3)" << std::endl;
        std::cout << "Expected: " << (rd.GetWidth() * rd.GetHeight() * 3) << std::endl;

        // Show sample pixels
        std::cout << "\nSample pixels (first 5 RGB values):" << std::endl;
        for (int i = 0; i < 15 && i < textureData.size(); i += 3)
        {
            std::cout << "  Pixel " << (i/3) << ": RGB("
                      << std::fixed << std::setprecision(0)
                      << textureData[i] << ", "
                      << textureData[i+1] << ", "
                      << textureData[i+2] << ")" << std::endl;
        }

        std::cout << "\nIn OpenGL, you would:" << std::endl;
        std::cout << "  1. glGenTextures() to create texture" << std::endl;
        std::cout << "  2. glTexImage2D() to upload texture data" << std::endl;
        std::cout << "  3. Apply to full-screen quad" << std::endl;
        std::cout << "  4. Blend with other visual elements" << std::endl;
    }

private:
    ReactionDiffusion rd;

    void RunSimulation(int iterations)
    {
        std::cout << "Running simulation for " << iterations << " iterations..." << std::endl;

        for (int i = 0; i < iterations; ++i)
        {
            rd.Update(0.016f);

            if (i % 100 == 0)
            {
                std::cout << "  Iteration " << i << "/" << iterations << std::endl;
            }
        }

        std::cout << "✓ Simulation complete!" << std::endl;
    }

    void AnalyzePattern(const std::string& name)
    {
        const auto& grid = rd.GetGrid();

        // Count cells with significant V concentration
        int vCells = 0;
        float maxV = 0.0f;
        float avgV = 0.0f;

        for (const auto& cell : grid)
        {
            if (cell.v > 0.3f)
            {
                vCells++;
            }
            maxV = std::max(maxV, cell.v);
            avgV += cell.v;
        }

        avgV /= grid.size();

        std::cout << "\nPattern Analysis (" << name << "):" << std::endl;
        std::cout << "  Cells with V > 0.3: " << vCells
                  << " (" << (100.0f * vCells / grid.size()) << "%)" << std::endl;
        std::cout << "  Max V concentration: " << std::fixed << std::setprecision(3) << maxV << std::endl;
        std::cout << "  Avg V concentration: " << avgV << std::endl;
    }
};

/**
 * @brief Main demo entry point
 */
int main()
{
    ReactionDiffusionPatternDemo demo;

    // Demo all pattern types
    demo.DemoSpots();
    demo.DemoStripes();
    demo.DemoSpirals();
    demo.DemoChaos();
    demo.DemoAudioReactive();
    demo.ExportTexture();

    std::cout << "\n=== USAGE RECOMMENDATIONS ===" << std::endl;
    std::cout << "\nBest Patterns for Music Genres:" << std::endl;
    std::cout << "  🎹 Ambient/Chill: Spirals or Waves" << std::endl;
    std::cout << "  🎸 Rock/Metal: Chaos" << std::endl;
    std::cout << "  🎧 Electronic: Stripes or Spots" << std::endl;
    std::cout << "  🎺 Jazz: Waves with audio modulation" << std::endl;

    std::cout << "\nPerformance Tips:" << std::endl;
    std::cout << "  - 64x64: Very fast, good for real-time" << std::endl;
    std::cout << "  - 128x128: Balanced detail/performance" << std::endl;
    std::cout << "  - 256x256: High detail, may need GPU" << std::endl;
    std::cout << "  - 512x512: Maximum detail, GPU recommended" << std::endl;

    std::cout << "\n✓ Demo complete!" << std::endl;

    return 0;
}
