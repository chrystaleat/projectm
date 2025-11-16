/**
 * @file ForceField.cpp
 * @brief Implementation of various force fields
 */

#include "ForceField.hpp"

#include <glm/gtc/constants.hpp>
#include <cmath>

namespace libprojectM {
namespace Renderer {

// ====== GravityForceField ======

void GravityForceField::ApplyForce(VerletParticle& particle)
{
    if (!m_enabled) return;

    // F = m * g, so acceleration = g (independent of mass!)
    particle.acceleration += gravity * m_strength;
}

// ====== AttractorForceField ======

void AttractorForceField::ApplyForce(VerletParticle& particle)
{
    if (!m_enabled) return;

    // Vector from particle to attractor
    glm::vec3 toAttractor = position - particle.position;
    float distance = glm::length(toAttractor);

    // Avoid singularity at center
    if (distance < minDistance)
    {
        distance = minDistance;
    }

    // Normalize direction
    glm::vec3 direction = toAttractor / distance;

    // Inverse square law: F = k / r^2
    float forceMagnitude = m_strength / (distance * distance);

    particle.acceleration += direction * forceMagnitude;
}

// ====== RepulsorForceField ======

void RepulsorForceField::ApplyForce(VerletParticle& particle)
{
    if (!m_enabled) return;

    // Vector from repulsor to particle
    glm::vec3 fromRepulsor = particle.position - position;
    float distance = glm::length(fromRepulsor);

    if (distance < minDistance)
    {
        distance = minDistance;
    }

    glm::vec3 direction = fromRepulsor / distance;

    // Inverse square repulsion
    float forceMagnitude = m_strength / (distance * distance);

    particle.acceleration += direction * forceMagnitude;
}

// ====== VortexForceField ======

void VortexForceField::ApplyForce(VerletParticle& particle)
{
    if (!m_enabled) return;

    // Vector from vortex center to particle
    glm::vec3 toParticle = particle.position - position;

    // Project onto plane perpendicular to axis
    float alongAxis = glm::dot(toParticle, axis);
    glm::vec3 radial = toParticle - (axis * alongAxis);

    float radius = glm::length(radial);

    if (radius < 0.01f)
    {
        return; // Too close to axis
    }

    // Tangential direction (perpendicular to radius, in rotation plane)
    glm::vec3 tangent = glm::cross(axis, radial);
    tangent = glm::normalize(tangent);

    // Tangential force (rotation)
    float tangentForce = m_strength / std::max(radius, 0.1f);

    // Radial force (pull toward axis)
    glm::vec3 radialDirection = radial / radius;
    float radialForce = -pullStrength * m_strength / std::max(radius, 0.1f);

    particle.acceleration += tangent * tangentForce + radialDirection * radialForce;
}

// ====== TurbulenceForceField ======

float TurbulenceForceField::Noise(float x, float y, float z) const
{
    // Simple pseudo-random noise based on position
    // This is a simplified version - real Perlin noise would be better
    float n = std::sin(x * 12.9898f + y * 78.233f + z * 37.719f) * 43758.5453f;
    return n - std::floor(n); // Fractional part
}

void TurbulenceForceField::ApplyForce(VerletParticle& particle)
{
    if (!m_enabled) return;

    // Sample noise at particle position (with frequency scaling)
    float px = particle.position.x * m_frequency;
    float py = particle.position.y * m_frequency;
    float pz = particle.position.z * m_frequency;

    // Get noise values for each axis
    float nx = Noise(px, py, pz) * 2.0f - 1.0f;           // -1 to 1
    float ny = Noise(py, pz, px) * 2.0f - 1.0f;
    float nz = Noise(pz, px, py) * 2.0f - 1.0f;

    glm::vec3 turbulence(nx, ny, nz);

    particle.acceleration += turbulence * m_strength;
}

// ====== WindForceField ======

void WindForceField::ApplyForce(VerletParticle& particle)
{
    if (!m_enabled) return;

    // Base wind force
    glm::vec3 force = direction * m_strength;

    // Add gustiness (random variation)
    if (gustiness > 0.0f)
    {
        float gust = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 2.0f; // -1 to 1
        force *= (1.0f + gust * gustiness);
    }

    particle.acceleration += force;
}

// ====== DragForceField ======

void DragForceField::ApplyForce(VerletParticle& particle)
{
    if (!m_enabled) return;

    // Get particle velocity
    glm::vec3 velocity = particle.GetVelocity();

    // Drag force opposes velocity: F = -k * v
    glm::vec3 drag = -velocity * dragCoefficient * m_strength;

    particle.acceleration += drag;
}

} // namespace Renderer
} // namespace libprojectM
