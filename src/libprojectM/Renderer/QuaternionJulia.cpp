/**
 * @file QuaternionJulia.cpp
 * @brief Implementation of Quaternion Julia Set fractals
 */

#include "QuaternionJulia.hpp"

#include <cmath>
#include <cstdlib>
#include <ctime>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace libprojectM {
namespace Renderer {

QuaternionJulia::QuaternionJulia()
{
    // Seed random number generator
    static bool seeded = false;
    if (!seeded)
    {
        std::srand(static_cast<unsigned>(std::time(nullptr)));
        seeded = true;
    }

    SetPreset(JuliaPreset::Classic);
}

int QuaternionJulia::IteratePoint(const Quaternion& point, const Quaternion& c, int maxIter)
{
    Quaternion q = point;
    int iterations = 0;

    float escapeRadiusSquared = m_escapeRadius * m_escapeRadius;

    for (iterations = 0; iterations < maxIter; ++iterations)
    {
        // Check for escape
        float magnitudeSquared = q.a * q.a + q.b * q.b + q.c * q.c + q.d * q.d;

        if (magnitudeSquared > escapeRadiusSquared)
        {
            break; // Escaped
        }

        // Julia iteration: q = q² + c
        q = q.Square() + c;
    }

    return iterations;
}

void QuaternionJulia::GeneratePointCloud(int numSamples, float bounds)
{
    m_points.clear();
    m_points.reserve(numSamples / 2); // Estimate ~half will be in set

    for (int i = 0; i < numSamples; ++i)
    {
        // Generate random point in 4D space
        Quaternion point = RandomPoint(bounds);

        // Test if point is in Julia set
        int iterations = IteratePoint(point, m_c, m_maxIterations);

        // If point didn't escape (reached max iterations), add to set
        if (iterations >= m_maxIterations)
        {
            m_points.push_back(point.ToVec4D());
        }
    }
}

void QuaternionJulia::SetPreset(JuliaPreset preset)
{
    switch (preset)
    {
        case JuliaPreset::Classic:
            m_c = Quaternion(-0.8f, 0.156f, 0.0f, 0.0f);
            break;

        case JuliaPreset::Spiral:
            m_c = Quaternion(-0.4f, -0.59f, -0.59f, 0.0f);
            break;

        case JuliaPreset::Dragon:
            m_c = Quaternion(-0.123f, 0.745f, 0.0f, 0.0f);
            break;

        case JuliaPreset::Bouquet:
            m_c = Quaternion(0.285f, 0.0f, 0.0f, 0.53f);
            break;

        case JuliaPreset::Chaotic:
            m_c = Quaternion(-0.2f, 0.8f, 0.0f, 0.0f);
            break;

        case JuliaPreset::Symmetrical:
            m_c = Quaternion(-0.213f, -0.0410f, -0.563f, 0.0f);
            break;

        case JuliaPreset::Custom:
            // Keep current c value
            break;
    }

    // Store base for audio modulation
    m_baseC = m_c;
}

void QuaternionJulia::SetCParameter(const Quaternion& c)
{
    m_c = c;
    m_baseC = c;
}

void QuaternionJulia::SetAudioModulation(float energy, float beat, float bass)
{
    m_audioEnergy = energy;
    m_beatIntensity = beat;
    m_bassLevel = bass;

    // Modulate c parameter with beat (rotate in quaternion space)
    float rotationAngle = beat * static_cast<float>(M_PI) * 0.25f;
    m_c = m_baseC;
    RotateC(rotationAngle);

    // Modulate real part with bass
    m_c.a = m_baseC.a * (1.0f + (bass - 0.5f) * 0.3f);

    // Modulate render size with energy
    m_renderSize = 0.5f + energy * 1.5f; // Range: 0.5 to 2.0
}

Quaternion QuaternionJulia::RandomPoint(float bounds)
{
    auto randomFloat = []() -> float {
        return static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
    };

    return Quaternion(
        (randomFloat() * 2.0f - 1.0f) * bounds,  // a: [-bounds, bounds]
        (randomFloat() * 2.0f - 1.0f) * bounds,  // b
        (randomFloat() * 2.0f - 1.0f) * bounds,  // c
        (randomFloat() * 2.0f - 1.0f) * bounds   // d
    );
}

void QuaternionJulia::RotateC(float angle)
{
    // Rotate c in the b-c plane (quaternion space rotation)
    float cosAngle = std::cos(angle);
    float sinAngle = std::sin(angle);

    float newB = m_c.b * cosAngle - m_c.c * sinAngle;
    float newC = m_c.b * sinAngle + m_c.c * cosAngle;

    m_c.b = newB;
    m_c.c = newC;
}

} // namespace Renderer
} // namespace libprojectM
