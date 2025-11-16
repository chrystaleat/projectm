/**
 * @file ForceField.hpp
 * @brief Force fields that affect particles (gravity, attraction, vortex, etc.)
 */

#pragma once

#include "VerletParticle.hpp"

#include <projectM-4/projectM_export.h>

#include <glm/glm.hpp>
#include <memory>

namespace libprojectM {
namespace Renderer {

/**
 * @brief Base class for force fields
 *
 * Force fields apply forces to particles. Different types create
 * different effects: gravity, attraction, repulsion, vortex, turbulence, etc.
 */
class PROJECTM_EXPORT ForceField
{
public:
    virtual ~ForceField() = default;

    /**
     * @brief Apply this force field to a particle
     *
     * Modifies particle's acceleration based on force field characteristics.
     */
    virtual void ApplyForce(VerletParticle& particle) = 0;

    /**
     * @brief Enable/disable this force field
     */
    void SetEnabled(bool enabled) { m_enabled = enabled; }
    bool IsEnabled() const { return m_enabled; }

    /**
     * @brief Set strength multiplier
     */
    void SetStrength(float strength) { m_strength = strength; }
    float GetStrength() const { return m_strength; }

protected:
    bool m_enabled{true};
    float m_strength{1.0f};
};

/**
 * @brief Constant gravity force field
 *
 * Applies uniform acceleration in specified direction.
 * Like Earth's gravity: F = m * g
 */
class PROJECTM_EXPORT GravityForceField : public ForceField
{
public:
    GravityForceField(float gx = 0.0f, float gy = -9.8f, float gz = 0.0f)
        : gravity(gx, gy, gz)
    {}

    void ApplyForce(VerletParticle& particle) override;

    glm::vec3 gravity;
};

/**
 * @brief Point attractor force field
 *
 * Attracts particles toward a point using inverse-square law (like gravity).
 * F = strength / distance^2
 */
class PROJECTM_EXPORT AttractorForceField : public ForceField
{
public:
    AttractorForceField(float x, float y, float z, float strength = 100.0f)
        : position(x, y, z)
    {
        m_strength = strength;
    }

    void ApplyForce(VerletParticle& particle) override;

    glm::vec3 position;
    float minDistance{0.1f};  //!< Minimum distance to avoid singularity
};

/**
 * @brief Point repulsor force field
 *
 * Repels particles away from a point.
 * F = -strength / distance^2
 */
class PROJECTM_EXPORT RepulsorForceField : public ForceField
{
public:
    RepulsorForceField(float x, float y, float z, float strength = 100.0f)
        : position(x, y, z)
    {
        m_strength = strength;
    }

    void ApplyForce(VerletParticle& particle) override;

    glm::vec3 position;
    float minDistance{0.1f};
};

/**
 * @brief Vortex (tornado) force field
 *
 * Creates spiraling motion around an axis.
 * Particles orbit around the axis while being pulled toward it.
 */
class PROJECTM_EXPORT VortexForceField : public ForceField
{
public:
    VortexForceField(float x, float y, float z,
                    float strength,
                    float axisX, float axisY, float axisZ)
        : position(x, y, z)
        , axis(axisX, axisY, axisZ)
    {
        m_strength = strength;
        axis = glm::normalize(axis);
    }

    void ApplyForce(VerletParticle& particle) override;

    glm::vec3 position;     //!< Center of vortex
    glm::vec3 axis;         //!< Rotation axis
    float pullStrength{0.5f}; //!< How much to pull toward axis
};

/**
 * @brief Turbulence force field
 *
 * Adds random noise-based forces for chaotic motion.
 * Uses Perlin-like noise based on particle position.
 */
class PROJECTM_EXPORT TurbulenceForceField : public ForceField
{
public:
    TurbulenceForceField(float strength = 10.0f, float frequency = 0.1f)
        : m_frequency(frequency)
    {
        m_strength = strength;
    }

    void ApplyForce(VerletParticle& particle) override;

private:
    /**
     * @brief Simple noise function for turbulence
     */
    float Noise(float x, float y, float z) const;

    float m_frequency;
};

/**
 * @brief Directional wind force field
 *
 * Applies force in a constant direction, like wind.
 * Can vary with position and time for more realistic effects.
 */
class PROJECTM_EXPORT WindForceField : public ForceField
{
public:
    WindForceField(float dx, float dy, float dz, float strength = 5.0f)
        : direction(dx, dy, dz)
    {
        m_strength = strength;
        direction = glm::normalize(direction);
    }

    void ApplyForce(VerletParticle& particle) override;

    glm::vec3 direction;
    float gustiness{0.1f};  //!< Random variation (0-1)
};

/**
 * @brief Drag force field (air resistance)
 *
 * Opposes motion proportional to velocity.
 * F = -k * v
 */
class PROJECTM_EXPORT DragForceField : public ForceField
{
public:
    explicit DragForceField(float coefficient = 0.1f)
        : dragCoefficient(coefficient)
    {}

    void ApplyForce(VerletParticle& particle) override;

    float dragCoefficient;
};

} // namespace Renderer
} // namespace libprojectM
