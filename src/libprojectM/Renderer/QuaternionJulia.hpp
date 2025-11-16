/**
 * @file QuaternionJulia.hpp
 * @brief 4D Julia Set fractals using quaternion mathematics
 *
 * Implements quaternion-based Julia sets - true 4D fractals that can be
 * projected to 3D and 2D for visualization. Creates mind-bending fractal
 * structures that rotate and morph in 4D space!
 *
 * Quaternions are 4D extensions of complex numbers: q = a + bi + cj + dk
 * where i² = j² = k² = ijk = -1
 *
 * The Julia set iteration: q_new = q_old² + c
 * Points that don't escape to infinity form the Julia set.
 */

#pragma once

#include "Vec4D.hpp"

#include <projectM-4/projectM_export.h>

#include <vector>

namespace libprojectM {
namespace Renderer {

/**
 * @brief Quaternion (4D complex number)
 *
 * A quaternion has four components: q = a + bi + cj + dk
 * - a: Real part
 * - b: i component (imaginary)
 * - c: j component (imaginary)
 * - d: k component (imaginary)
 *
 * Quaternion multiplication rules:
 * - i² = j² = k² = -1
 * - ij = k, jk = i, ki = j
 * - ji = -k, kj = -i, ik = -j
 */
struct PROJECTM_EXPORT Quaternion
{
    float a{0.0f};  //!< Real part
    float b{0.0f};  //!< i component
    float c{0.0f};  //!< j component
    float d{0.0f};  //!< k component

    /**
     * @brief Construct quaternion
     */
    Quaternion(float a = 0.0f, float b = 0.0f, float c = 0.0f, float d = 0.0f)
        : a(a), b(b), c(c), d(d) {}

    /**
     * @brief Add quaternions
     */
    Quaternion operator+(const Quaternion& other) const
    {
        return Quaternion(a + other.a, b + other.b, c + other.c, d + other.d);
    }

    /**
     * @brief Multiply quaternions
     *
     * Uses Hamilton's quaternion multiplication rules.
     */
    Quaternion operator*(const Quaternion& other) const
    {
        return Quaternion(
            a * other.a - b * other.b - c * other.c - d * other.d,  // Real
            a * other.b + b * other.a + c * other.d - d * other.c,  // i
            a * other.c - b * other.d + c * other.a + d * other.b,  // j
            a * other.d + b * other.c - c * other.b + d * other.a   // k
        );
    }

    /**
     * @brief Square quaternion
     */
    Quaternion Square() const
    {
        return (*this) * (*this);
    }

    /**
     * @brief Calculate magnitude
     */
    float Magnitude() const
    {
        return std::sqrt(a * a + b * b + c * c + d * d);
    }

    /**
     * @brief Convert to Vec4D
     */
    Vec4D ToVec4D() const
    {
        return Vec4D(a, b, c, d);
    }

    /**
     * @brief Create from Vec4D
     */
    static Quaternion FromVec4D(const Vec4D& v)
    {
        return Quaternion(v.x, v.y, v.z, v.w);
    }
};

/**
 * @brief Julia set presets
 */
enum class JuliaPreset
{
    Classic,        //!< Classic Julia set (-0.8, 0.156, 0, 0)
    Spiral,         //!< Spiral structure (-0.4, -0.59, -0.59, 0)
    Dragon,         //!< Dragon-like shape (-0.123, 0.745, 0, 0)
    Bouquet,        //!< Flower-like (0.285, 0.0, 0.0, 0.53)
    Chaotic,        //!< Chaotic structure (-0.2, 0.8, 0.0, 0.0)
    Symmetrical,    //!< 4-fold symmetry (-0.213, -0.0410, -0.563, 0.0)
    Custom          //!< User-defined c parameter
};

/**
 * @brief Quaternion Julia Set fractal generator
 *
 * Generates 4D Julia set fractals using quaternion iteration.
 * The algorithm:
 *
 * ```
 * for each point q in 4D space:
 *     iterate: q = q² + c
 *     if magnitude(q) > escape_radius:
 *         point escapes (not in set)
 *     if iterations reach max:
 *         point is in Julia set
 * ```
 *
 * Points in the Julia set form beautiful 4D fractal structures that can be:
 * - Projected to 3D using 4D->3D projection
 * - Rotated in 4D space
 * - Morphed by changing c parameter
 * - Made audio-reactive
 *
 * **Audio Reactivity:**
 * - Beat: Rotates c parameter in 4D space
 * - Energy: Scales the fractal
 * - Bass: Modulates c.a (real part)
 * - Treble: Modulates c.b-d (imaginary parts)
 *
 * Usage:
 * @code
 * QuaternionJulia julia;
 * julia.SetPreset(JuliaPreset::Spiral);
 * julia.SetAudioModulation(energy, beat, bass);
 * julia.GeneratePointCloud(5000, 2.0f);
 *
 * const auto& points = julia.GetPoints();
 * // Project points using HyperdimensionalProjection
 * @endcode
 */
class PROJECTM_EXPORT QuaternionJulia
{
public:
    /**
     * @brief Construct Julia set generator
     */
    QuaternionJulia();

    /**
     * @brief Iterate a point to test Julia set membership
     *
     * @param point Starting quaternion
     * @param c Julia set parameter
     * @param maxIter Maximum iterations
     * @return Number of iterations before escape (maxIter if in set)
     */
    int IteratePoint(const Quaternion& point, const Quaternion& c, int maxIter);

    /**
     * @brief Generate point cloud of Julia set
     *
     * Samples 4D space and keeps points that are in the Julia set.
     *
     * @param numSamples Number of random samples to test
     * @param bounds Sampling bounds (±bounds in each dimension)
     */
    void GeneratePointCloud(int numSamples, float bounds = 2.0f);

    /**
     * @brief Get generated points
     *
     * @return Vector of 4D points in the Julia set
     */
    const std::vector<Vec4D>& GetPoints() const { return m_points; }

    // ====== Parameters ======

    /**
     * @brief Set Julia set preset
     */
    void SetPreset(JuliaPreset preset);

    /**
     * @brief Set custom c parameter
     */
    void SetCParameter(const Quaternion& c);

    /**
     * @brief Get current c parameter
     */
    Quaternion GetCParameter() const { return m_c; }

    /**
     * @brief Set escape radius
     *
     * Points with magnitude > radius are considered escaped.
     *
     * @param radius Escape threshold (default 2.0)
     */
    void SetEscapeRadius(float radius) { m_escapeRadius = radius; }

    /**
     * @brief Set maximum iterations
     *
     * @param maxIter Max iterations per point (default 100)
     */
    void SetMaxIterations(int maxIter) { m_maxIterations = maxIter; }

    // ====== Audio Reactivity ======

    /**
     * @brief Set audio modulation
     *
     * Modulates the c parameter and render size based on audio.
     *
     * @param energy Overall audio energy (0-1)
     * @param beat Beat intensity (0-1)
     * @param bass Bass level (0-1)
     */
    void SetAudioModulation(float energy, float beat, float bass);

    /**
     * @brief Get current render size multiplier
     *
     * Affected by audio energy.
     *
     * @return Size multiplier (1.0 = normal)
     */
    float GetRenderSize() const { return m_renderSize; }

    // ====== Clear ======

    /**
     * @brief Clear generated points
     */
    void Clear() { m_points.clear(); }

private:
    /**
     * @brief Generate random point in bounds
     */
    Quaternion RandomPoint(float bounds);

    /**
     * @brief Rotate c parameter for audio reactivity
     */
    void RotateC(float angle);

    // ====== Julia Set Parameters ======

    Quaternion m_c{-0.8f, 0.156f, 0.0f, 0.0f};    //!< Julia set parameter
    Quaternion m_baseC{-0.8f, 0.156f, 0.0f, 0.0f}; //!< Base c (before modulation)

    float m_escapeRadius{2.0f};                    //!< Escape threshold
    int m_maxIterations{100};                      //!< Max iterations per point

    // ====== Generated Data ======

    std::vector<Vec4D> m_points;                   //!< Points in Julia set

    // ====== Audio Modulation ======

    float m_audioEnergy{0.5f};                     //!< Current audio energy
    float m_beatIntensity{0.0f};                   //!< Current beat intensity
    float m_bassLevel{0.0f};                       //!< Current bass level

    float m_renderSize{1.0f};                      //!< Render size multiplier
};

} // namespace Renderer
} // namespace libprojectM
