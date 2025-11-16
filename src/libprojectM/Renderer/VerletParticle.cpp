/**
 * @file VerletParticle.cpp
 * @brief Implementation of Verlet particle physics
 */

#include "VerletParticle.hpp"

namespace libprojectM {
namespace Renderer {

VerletParticle::VerletParticle(float x, float y, float z)
    : position(x, y, z)
    , previousPosition(x, y, z) // Start at rest
{
}

void VerletParticle::Update(float deltaTime)
{
    if (!active)
    {
        return;
    }

    // Update age and check lifetime
    age += deltaTime;
    if (lifetime > 0.0f && age >= lifetime)
    {
        active = false;
        return;
    }

    // Verlet integration: x(t+dt) = 2*x(t) - x(t-dt) + a*dt^2
    glm::vec3 temp = position;

    float dt2 = deltaTime * deltaTime;
    position = 2.0f * position - previousPosition + acceleration * dt2;

    previousPosition = temp;

    // Apply damping
    ApplyDamping();

    // Clear forces for next frame
    ClearForces();
}

void VerletParticle::AddForce(float fx, float fy, float fz)
{
    // F = ma, so a = F/m
    if (mass > 0.0f)
    {
        acceleration.x += fx / mass;
        acceleration.y += fy / mass;
        acceleration.z += fz / mass;
    }
}

void VerletParticle::SetVelocity(float vx, float vy, float vz, float dt)
{
    // velocity = (position - previousPosition) / dt
    // So: previousPosition = position - velocity * dt
    previousPosition.x = position.x - vx * dt;
    previousPosition.y = position.y - vy * dt;
    previousPosition.z = position.z - vz * dt;
}

glm::vec3 VerletParticle::GetVelocity(float dt) const
{
    // v = (current - previous) / dt
    return (position - previousPosition) / dt;
}

void VerletParticle::ClearForces()
{
    acceleration = glm::vec3(0.0f);
}

void VerletParticle::ApplyDamping()
{
    if (damping < 1.0f)
    {
        // Damping is applied to the velocity
        // velocity = position - previousPosition
        // dampedVelocity = velocity * damping
        // previousPosition = position - dampedVelocity

        glm::vec3 velocity = position - previousPosition;
        previousPosition = position - (velocity * damping);
    }
}

} // namespace Renderer
} // namespace libprojectM
