/**
 * @file ReactionDiffusion.cpp
 * @brief Implementation of Gray-Scott reaction-diffusion
 */

#include "ReactionDiffusion.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>

namespace libprojectM {
namespace Renderer {

ReactionDiffusion::ReactionDiffusion(size_t width, size_t height)
    : m_width(width)
    , m_height(height)
{
    m_grid.resize(width * height);
    m_gridNext.resize(width * height);

    Reset();

    // Seed random number generator
    static bool seeded = false;
    if (!seeded)
    {
        std::srand(static_cast<unsigned>(std::time(nullptr)));
        seeded = true;
    }
}

void ReactionDiffusion::Reset()
{
    // Initialize grid: full U, no V
    for (auto& cell : m_grid)
    {
        cell.u = 1.0f;
        cell.v = 0.0f;
    }

    m_gridNext = m_grid;
}

void ReactionDiffusion::Update(float deltaTime)
{
    // Gray-Scott reaction-diffusion equations:
    // dU/dt = Du * ∇²U - UV² + F(1-U)
    // dV/dt = Dv * ∇²V + UV² - (F+k)V

    float dt = deltaTime * TimeScale;

    for (size_t y = 0; y < m_height; ++y)
    {
        for (size_t x = 0; x < m_width; ++x)
        {
            size_t idx = GetIndex(x, y);
            const RDCell& cell = m_grid[idx];

            // Calculate Laplacians (diffusion)
            float laplaceU = CalculateLaplacian(m_grid, x, y, true);
            float laplaceV = CalculateLaplacian(m_grid, x, y, false);

            // Reaction term: UV²
            float uvv = cell.u * cell.v * cell.v;

            // Update U: Du * ∇²U - UV² + F(1-U)
            float dudt = m_diffusionU * laplaceU - uvv + m_feedRate * (1.0f - cell.u);

            // Update V: Dv * ∇²V + UV² - (F+k)V
            float dvdt = m_diffusionV * laplaceV + uvv - (m_feedRate + m_killRate) * cell.v;

            // Euler integration
            m_gridNext[idx].u = Clamp01(cell.u + dudt * dt);
            m_gridNext[idx].v = Clamp01(cell.v + dvdt * dt);
        }
    }

    // Swap grids (double buffering)
    std::swap(m_grid, m_gridNext);
}

const RDCell& ReactionDiffusion::GetCell(size_t x, size_t y) const
{
    return m_grid[GetIndex(x, y)];
}

void ReactionDiffusion::SetCell(size_t x, size_t y, float u, float v)
{
    size_t idx = GetIndex(x, y);
    m_grid[idx].u = Clamp01(u);
    m_grid[idx].v = Clamp01(v);
}

void ReactionDiffusion::SeedCenter(int radius)
{
    int centerX = static_cast<int>(m_width) / 2;
    int centerY = static_cast<int>(m_height) / 2;

    SeedAt(centerX, centerY, radius);
}

void ReactionDiffusion::SeedRandom(int count, int radius)
{
    for (int i = 0; i < count; ++i)
    {
        int x = std::rand() % static_cast<int>(m_width);
        int y = std::rand() % static_cast<int>(m_height);

        SeedAt(x, y, radius);
    }
}

void ReactionDiffusion::SeedAt(int x, int y, int radius)
{
    int radiusSquared = radius * radius;

    for (int dy = -radius; dy <= radius; ++dy)
    {
        for (int dx = -radius; dx <= radius; ++dx)
        {
            if (dx * dx + dy * dy <= radiusSquared)
            {
                int px = x + dx;
                int py = y + dy;

                // Boundary check
                if (px >= 0 && px < static_cast<int>(m_width) &&
                    py >= 0 && py < static_cast<int>(m_height))
                {
                    size_t idx = GetIndex(static_cast<size_t>(px), static_cast<size_t>(py));
                    m_grid[idx].v = 1.0f;
                    m_grid[idx].u = 0.0f;
                }
            }
        }
    }
}

void ReactionDiffusion::SetPreset(ReactionDiffusionPreset preset)
{
    switch (preset)
    {
        case ReactionDiffusionPreset::Spots:
            m_feedRate = 0.055f;
            m_killRate = 0.062f;
            m_diffusionU = 0.16f;
            m_diffusionV = 0.08f;
            break;

        case ReactionDiffusionPreset::Stripes:
            m_feedRate = 0.035f;
            m_killRate = 0.065f;
            m_diffusionU = 0.16f;
            m_diffusionV = 0.08f;
            break;

        case ReactionDiffusionPreset::Spirals:
            m_feedRate = 0.014f;
            m_killRate = 0.054f;
            m_diffusionU = 0.16f;
            m_diffusionV = 0.08f;
            break;

        case ReactionDiffusionPreset::Chaos:
            m_feedRate = 0.062f;
            m_killRate = 0.061f;
            m_diffusionU = 0.16f;
            m_diffusionV = 0.08f;
            break;

        case ReactionDiffusionPreset::Waves:
            m_feedRate = 0.020f;
            m_killRate = 0.050f;
            m_diffusionU = 0.14f;
            m_diffusionV = 0.06f;
            break;

        case ReactionDiffusionPreset::Custom:
            // Use current values
            break;
    }

    // Store base values for audio modulation
    m_baseFeedRate = m_feedRate;
    m_baseKillRate = m_killRate;
}

void ReactionDiffusion::SetAudioModulation(float energy, float beatIntensity, float bassLevel)
{
    m_audioEnergy = energy;
    m_beatIntensity = beatIntensity;
    m_bassLevel = bassLevel;

    // Modulate feed rate with energy (higher energy = faster growth)
    m_feedRate = m_baseFeedRate * (1.0f + (energy - 0.5f) * 0.3f);

    // Modulate kill rate with beat (beats = pattern decay)
    m_killRate = m_baseKillRate * (1.0f + beatIntensity * 0.2f);

    // Clamp to reasonable ranges
    m_feedRate = std::max(0.01f, std::min(0.1f, m_feedRate));
    m_killRate = std::max(0.03f, std::min(0.08f, m_killRate));
}

std::vector<float> ReactionDiffusion::GetTextureData() const
{
    std::vector<float> textureData;
    textureData.reserve(m_width * m_height * 3);

    for (const auto& cell : m_grid)
    {
        // Map V concentration to grayscale
        // V = 0: dark, V = 1: bright
        float intensity = cell.v * 255.0f;

        // RGB (grayscale for now, could add color mapping later)
        textureData.push_back(intensity);
        textureData.push_back(intensity);
        textureData.push_back(intensity);
    }

    return textureData;
}

float ReactionDiffusion::CalculateLaplacian(const std::vector<RDCell>& grid, size_t x, size_t y, bool isU)
{
    // 9-point stencil Laplacian approximation
    // Weights:
    //   0.05  0.2  0.05
    //   0.2   -1   0.2
    //   0.05  0.2  0.05

    float center = isU ? grid[GetIndex(x, y)].u : grid[GetIndex(x, y)].v;
    float sum = 0.0f;

    // Helper lambda to get neighbor with boundary handling
    auto getNeighbor = [&](int dx, int dy) -> float {
        int nx = static_cast<int>(x) + dx;
        int ny = static_cast<int>(y) + dy;

        if (m_boundaryMode == RDBoundaryMode::Wrap)
        {
            // Wrap around (toroidal)
            if (nx < 0) nx += static_cast<int>(m_width);
            if (nx >= static_cast<int>(m_width)) nx -= static_cast<int>(m_width);
            if (ny < 0) ny += static_cast<int>(m_height);
            if (ny >= static_cast<int>(m_height)) ny -= static_cast<int>(m_height);
        }
        else
        {
            // Fixed boundary (clamp)
            nx = std::max(0, std::min(static_cast<int>(m_width) - 1, nx));
            ny = std::max(0, std::min(static_cast<int>(m_height) - 1, ny));
        }

        const RDCell& cell = grid[GetIndex(static_cast<size_t>(nx), static_cast<size_t>(ny))];
        return isU ? cell.u : cell.v;
    };

    // Apply 9-point stencil
    sum += getNeighbor(-1, -1) * 0.05f;  // Top-left
    sum += getNeighbor(0, -1) * 0.2f;    // Top
    sum += getNeighbor(1, -1) * 0.05f;   // Top-right

    sum += getNeighbor(-1, 0) * 0.2f;    // Left
    sum += center * (-1.0f);             // Center
    sum += getNeighbor(1, 0) * 0.2f;     // Right

    sum += getNeighbor(-1, 1) * 0.05f;   // Bottom-left
    sum += getNeighbor(0, 1) * 0.2f;     // Bottom
    sum += getNeighbor(1, 1) * 0.05f;    // Bottom-right

    return sum;
}

float ReactionDiffusion::Clamp01(float value) const
{
    return std::max(0.0f, std::min(1.0f, value));
}

} // namespace Renderer
} // namespace libprojectM
