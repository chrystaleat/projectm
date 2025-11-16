/**
 * @file ParticleSystem.cpp
 * @brief Implementation of particle system
 */

#include "ParticleSystem.hpp"

#include <algorithm>

namespace libprojectM {
namespace Renderer {

ParticleSystem::ParticleSystem(size_t maxParticles)
    : m_maxParticles(maxParticles)
{
    m_particles.reserve(maxParticles);

    // Create default gravity (disabled by default)
    m_gravity = std::make_shared<GravityForceField>(0.0f, 0.0f, 0.0f);
    m_gravity->SetEnabled(false);
}

void ParticleSystem::Update(float deltaTime)
{
    // Apply force fields to all particles
    ApplyForces();

    // Update particle physics
    for (auto& particle : m_particles)
    {
        if (particle.active)
        {
            particle.Update(deltaTime);
        }
    }

    // Handle collisions if enabled
    if (m_enableCollision)
    {
        HandleCollisions();
    }

    // Enforce boundaries
    if (m_boundaryMode != BoundaryMode::None)
    {
        EnforceBoundaries();
    }

    // Remove dead particles
    RemoveDeadParticles();
}

VerletParticle* ParticleSystem::EmitParticle(float x, float y, float z,
                                             float vx, float vy, float vz,
                                             float lifetime,
                                             float mass)
{
    // Check capacity
    if (m_particles.size() >= m_maxParticles)
    {
        // Try to find an inactive particle to reuse
        for (auto& particle : m_particles)
        {
            if (!particle.active)
            {
                // Reuse this particle
                particle = VerletParticle(x, y, z);
                particle.SetVelocity(vx, vy, vz);
                particle.lifetime = lifetime;
                particle.mass = mass;
                particle.age = 0.0f;
                particle.active = true;
                return &particle;
            }
        }

        // No inactive particles, can't emit
        return nullptr;
    }

    // Create new particle
    VerletParticle particle(x, y, z);
    particle.SetVelocity(vx, vy, vz);
    particle.lifetime = lifetime;
    particle.mass = mass;

    m_particles.push_back(particle);
    return &m_particles.back();
}

void ParticleSystem::Clear()
{
    m_particles.clear();
}

void ParticleSystem::SetGravity(float gx, float gy, float gz)
{
    m_gravity->gravity = glm::vec3(gx, gy, gz);
    m_gravity->SetEnabled(true);
}

void ParticleSystem::AddForceField(std::shared_ptr<ForceField> field)
{
    m_forceFields.push_back(field);
}

void ParticleSystem::ClearForceFields()
{
    m_forceFields.clear();
}

void ParticleSystem::SetBoundingBox(float minX, float minY, float minZ,
                                   float maxX, float maxY, float maxZ)
{
    m_boundMin = glm::vec3(minX, minY, minZ);
    m_boundMax = glm::vec3(maxX, maxY, maxZ);
}

void ParticleSystem::ApplyForces()
{
    // Apply gravity if enabled
    if (m_gravity && m_gravity->IsEnabled())
    {
        for (auto& particle : m_particles)
        {
            if (particle.active)
            {
                m_gravity->ApplyForce(particle);
            }
        }
    }

    // Apply all other force fields
    for (auto& field : m_forceFields)
    {
        if (field && field->IsEnabled())
        {
            for (auto& particle : m_particles)
            {
                if (particle.active)
                {
                    field->ApplyForce(particle);
                }
            }
        }
    }
}

void ParticleSystem::EnforceBoundaries()
{
    for (auto& particle : m_particles)
    {
        if (!particle.active) continue;

        bool outsideBounds = false;

        // Check each axis
        for (int axis = 0; axis < 3; ++axis)
        {
            float& pos = particle.position[axis];
            float& prevPos = particle.previousPosition[axis];
            float minBound = m_boundMin[axis];
            float maxBound = m_boundMax[axis];

            if (pos < minBound || pos > maxBound)
            {
                outsideBounds = true;

                switch (m_boundaryMode)
                {
                case BoundaryMode::Kill:
                    particle.active = false;
                    break;

                case BoundaryMode::Wrap:
                    if (pos < minBound)
                    {
                        float overflow = minBound - pos;
                        pos = maxBound - overflow;
                        prevPos = maxBound - overflow;
                    }
                    else if (pos > maxBound)
                    {
                        float overflow = pos - maxBound;
                        pos = minBound + overflow;
                        prevPos = minBound + overflow;
                    }
                    break;

                case BoundaryMode::Bounce:
                    if (pos < minBound)
                    {
                        pos = minBound;
                        // Reflect velocity
                        float velocity = pos - prevPos;
                        prevPos = pos - velocity; // Reverse velocity
                    }
                    else if (pos > maxBound)
                    {
                        pos = maxBound;
                        float velocity = pos - prevPos;
                        prevPos = pos - velocity;
                    }
                    break;

                case BoundaryMode::Clamp:
                    if (pos < minBound)
                    {
                        pos = minBound;
                        prevPos = minBound; // Stop motion
                    }
                    else if (pos > maxBound)
                    {
                        pos = maxBound;
                        prevPos = maxBound;
                    }
                    break;

                case BoundaryMode::None:
                default:
                    break;
                }
            }
        }
    }
}

void ParticleSystem::HandleCollisions()
{
    // Simple O(n^2) collision detection
    // For production, use spatial hashing or BVH

    size_t count = m_particles.size();

    for (size_t i = 0; i < count; ++i)
    {
        if (!m_particles[i].active) continue;

        for (size_t j = i + 1; j < count; ++j)
        {
            if (!m_particles[j].active) continue;

            auto& p1 = m_particles[i];
            auto& p2 = m_particles[j];

            // Check collision
            glm::vec3 delta = p1.position - p2.position;
            float distance = glm::length(delta);
            float minDist = p1.radius + p2.radius;

            if (distance < minDist && distance > 0.0001f)
            {
                // Collision detected!
                glm::vec3 normal = delta / distance;

                // Separate particles
                float overlap = minDist - distance;
                float totalMass = p1.mass + p2.mass;

                p1.position += normal * (overlap * p2.mass / totalMass);
                p2.position -= normal * (overlap * p1.mass / totalMass);

                // Apply collision response (elastic collision)
                glm::vec3 v1 = p1.GetVelocity();
                glm::vec3 v2 = p2.GetVelocity();

                float v1n = glm::dot(v1, normal);
                float v2n = glm::dot(v2, normal);

                // Only if approaching
                if (v1n - v2n < 0.0f) continue;

                // New normal velocities after elastic collision
                float m1 = p1.mass;
                float m2 = p2.mass;

                float v1nNew = (v1n * (m1 - m2) + 2.0f * m2 * v2n) / totalMass;
                float v2nNew = (v2n * (m2 - m1) + 2.0f * m1 * v1n) / totalMass;

                // Apply elasticity
                v1nNew = v1n + (v1nNew - v1n) * m_collisionElasticity;
                v2nNew = v2n + (v2nNew - v2n) * m_collisionElasticity;

                // Update velocities
                glm::vec3 v1New = v1 + normal * (v1nNew - v1n);
                glm::vec3 v2New = v2 + normal * (v2nNew - v2n);

                p1.SetVelocity(v1New.x, v1New.y, v1New.z);
                p2.SetVelocity(v2New.x, v2New.y, v2New.z);
            }
        }
    }
}

void ParticleSystem::RemoveDeadParticles()
{
    // Remove inactive particles
    m_particles.erase(
        std::remove_if(m_particles.begin(), m_particles.end(),
                      [](const VerletParticle& p) { return !p.active; }),
        m_particles.end()
    );
}

} // namespace Renderer
} // namespace libprojectM
