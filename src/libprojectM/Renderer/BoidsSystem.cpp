/**
 * @file BoidsSystem.cpp
 * @brief Implementation of Boids flocking algorithm
 */

#include "BoidsSystem.hpp"

#include <algorithm>
#include <cmath>

namespace libprojectM {
namespace Renderer {

BoidsSystem::BoidsSystem(size_t maxBoids)
    : m_maxBoids(maxBoids)
{
    m_boids.reserve(maxBoids);
}

VerletParticle* BoidsSystem::AddBoid(const glm::vec3& position, const glm::vec3& velocity)
{
    // Check capacity
    if (m_activeBoidCount >= m_maxBoids)
    {
        // Try to find inactive boid to recycle
        for (auto* boid : m_boids)
        {
            if (!boid->active)
            {
                boid->position = position;
                boid->previousPosition = position - velocity * 0.016f;
                boid->acceleration = glm::vec3(0.0f);
                boid->active = true;
                m_activeBoidCount++;
                return boid;
            }
        }

        // No inactive boids, at capacity
        return nullptr;
    }

    // Create new boid
    auto* boid = new VerletParticle(position.x, position.y, position.z);
    boid->SetVelocity(velocity.x, velocity.y, velocity.z);
    m_boids.push_back(boid);
    m_activeBoidCount++;

    return boid;
}

void BoidsSystem::Update(float deltaTime)
{
    // Apply flocking forces to all boids
    for (size_t i = 0; i < m_boids.size(); ++i)
    {
        auto* boid = m_boids[i];
        if (!boid || !boid->active)
            continue;

        // Calculate flocking forces
        glm::vec3 separation = CalculateSeparation(i) * m_separationWeight;
        glm::vec3 alignment = CalculateAlignment(i) * m_alignmentWeight;
        glm::vec3 cohesion = CalculateCohesion(i) * m_cohesionWeight;

        // Add beat intensity to separation for pulsing effect
        if (m_beatIntensity > 0.0f)
        {
            separation *= (1.0f + m_beatIntensity);
        }

        // Combine forces
        glm::vec3 totalForce = separation + alignment + cohesion;

        // Limit force magnitude
        totalForce = LimitForce(totalForce);

        // Apply force
        boid->AddForce(totalForce.x, totalForce.y, totalForce.z);
    }

    // Update physics
    for (auto* boid : m_boids)
    {
        if (!boid || !boid->active)
            continue;

        boid->Update(deltaTime);

        // Apply speed limit
        ApplySpeedLimit(boid);

        // Handle boundaries
        HandleBoundaries(boid);
    }

    // Decay beat intensity
    m_beatIntensity *= 0.9f;
}

void BoidsSystem::Clear()
{
    for (auto* boid : m_boids)
    {
        delete boid;
    }
    m_boids.clear();
    m_activeBoidCount = 0;
}

void BoidsSystem::SetBoundingBox(float minX, float minY, float minZ,
                                 float maxX, float maxY, float maxZ)
{
    m_boundMin = glm::vec3(minX, minY, minZ);
    m_boundMax = glm::vec3(maxX, maxY, maxZ);
}

glm::vec3 BoidsSystem::CalculateSeparation(size_t boidIndex)
{
    auto* boid = m_boids[boidIndex];
    glm::vec3 steer(0.0f);
    int count = 0;

    // Find nearby boids and steer away
    for (size_t i = 0; i < m_boids.size(); ++i)
    {
        if (i == boidIndex)
            continue;

        auto* other = m_boids[i];
        if (!other || !other->active)
            continue;

        float distance = glm::distance(boid->position, other->position);

        // Only separate from very close boids
        if (distance > 0.0f && distance < m_separationRadius)
        {
            // Calculate repulsion vector (away from other boid)
            glm::vec3 diff = boid->position - other->position;
            diff = glm::normalize(diff);

            // Weight by distance (closer = stronger repulsion)
            diff /= distance;

            steer += diff;
            count++;
        }
    }

    if (count > 0)
    {
        steer /= static_cast<float>(count);
    }

    if (glm::length(steer) > 0.0f)
    {
        // Steering = desired - current velocity
        steer = glm::normalize(steer) * m_maxSpeed;
        steer -= boid->GetVelocity();
    }

    return steer;
}

glm::vec3 BoidsSystem::CalculateAlignment(size_t boidIndex)
{
    auto* boid = m_boids[boidIndex];
    glm::vec3 averageVelocity(0.0f);
    int count = 0;

    // Find neighbors and average their velocities
    auto neighbors = FindNeighbors(boidIndex);

    for (size_t neighborIdx : neighbors)
    {
        averageVelocity += m_boids[neighborIdx]->GetVelocity();
        count++;
    }

    if (count > 0)
    {
        averageVelocity /= static_cast<float>(count);

        // Steering = desired - current velocity
        averageVelocity = glm::normalize(averageVelocity) * m_maxSpeed;
        glm::vec3 steer = averageVelocity - boid->GetVelocity();

        return steer;
    }

    return glm::vec3(0.0f);
}

glm::vec3 BoidsSystem::CalculateCohesion(size_t boidIndex)
{
    auto* boid = m_boids[boidIndex];
    glm::vec3 centerOfMass(0.0f);
    int count = 0;

    // Find neighbors and calculate their center of mass
    auto neighbors = FindNeighbors(boidIndex);

    for (size_t neighborIdx : neighbors)
    {
        centerOfMass += m_boids[neighborIdx]->position;
        count++;
    }

    if (count > 0)
    {
        centerOfMass /= static_cast<float>(count);

        // Steering toward center of mass
        glm::vec3 desired = centerOfMass - boid->position;

        if (glm::length(desired) > 0.0f)
        {
            desired = glm::normalize(desired) * m_maxSpeed;
            glm::vec3 steer = desired - boid->GetVelocity();
            return steer;
        }
    }

    return glm::vec3(0.0f);
}

glm::vec3 BoidsSystem::LimitForce(const glm::vec3& force)
{
    float magnitude = glm::length(force);

    if (magnitude > m_maxForce)
    {
        return glm::normalize(force) * m_maxForce;
    }

    return force;
}

void BoidsSystem::ApplySpeedLimit(VerletParticle* boid)
{
    glm::vec3 velocity = boid->GetVelocity();
    float speed = glm::length(velocity);

    // Apply audio energy multiplier to max speed
    float effectiveMaxSpeed = m_maxSpeed * m_audioEnergyMultiplier;

    if (speed > effectiveMaxSpeed)
    {
        velocity = glm::normalize(velocity) * effectiveMaxSpeed;
        boid->SetVelocity(velocity.x, velocity.y, velocity.z);
    }
}

void BoidsSystem::HandleBoundaries(VerletParticle* boid)
{
    switch (m_boundaryMode)
    {
        case BoundaryMode::Kill:
        {
            // Deactivate if outside bounds
            if (boid->position.x < m_boundMin.x || boid->position.x > m_boundMax.x ||
                boid->position.y < m_boundMin.y || boid->position.y > m_boundMax.y ||
                boid->position.z < m_boundMin.z || boid->position.z > m_boundMax.z)
            {
                boid->active = false;
                m_activeBoidCount--;
            }
            break;
        }

        case BoundaryMode::Wrap:
        {
            // Wrap to opposite side (toroidal space)
            if (boid->position.x < m_boundMin.x)
            {
                float offset = m_boundMin.x - boid->position.x;
                boid->position.x = m_boundMax.x - offset;
                boid->previousPosition.x = m_boundMax.x - offset;
            }
            else if (boid->position.x > m_boundMax.x)
            {
                float offset = boid->position.x - m_boundMax.x;
                boid->position.x = m_boundMin.x + offset;
                boid->previousPosition.x = m_boundMin.x + offset;
            }

            if (boid->position.y < m_boundMin.y)
            {
                float offset = m_boundMin.y - boid->position.y;
                boid->position.y = m_boundMax.y - offset;
                boid->previousPosition.y = m_boundMax.y - offset;
            }
            else if (boid->position.y > m_boundMax.y)
            {
                float offset = boid->position.y - m_boundMax.y;
                boid->position.y = m_boundMin.y + offset;
                boid->previousPosition.y = m_boundMin.y + offset;
            }

            if (boid->position.z < m_boundMin.z)
            {
                float offset = m_boundMin.z - boid->position.z;
                boid->position.z = m_boundMax.z - offset;
                boid->previousPosition.z = m_boundMax.z - offset;
            }
            else if (boid->position.z > m_boundMax.z)
            {
                float offset = boid->position.z - m_boundMax.z;
                boid->position.z = m_boundMin.z + offset;
                boid->previousPosition.z = m_boundMin.z + offset;
            }
            break;
        }

        case BoundaryMode::Bounce:
        {
            // Reflect velocity at boundaries
            glm::vec3 velocity = boid->GetVelocity();
            bool bounced = false;

            if (boid->position.x < m_boundMin.x || boid->position.x > m_boundMax.x)
            {
                velocity.x *= -0.8f; // Dampen bounce
                boid->position.x = glm::clamp(boid->position.x, m_boundMin.x, m_boundMax.x);
                bounced = true;
            }

            if (boid->position.y < m_boundMin.y || boid->position.y > m_boundMax.y)
            {
                velocity.y *= -0.8f;
                boid->position.y = glm::clamp(boid->position.y, m_boundMin.y, m_boundMax.y);
                bounced = true;
            }

            if (boid->position.z < m_boundMin.z || boid->position.z > m_boundMax.z)
            {
                velocity.z *= -0.8f;
                boid->position.z = glm::clamp(boid->position.z, m_boundMin.z, m_boundMax.z);
                bounced = true;
            }

            if (bounced)
            {
                boid->SetVelocity(velocity.x, velocity.y, velocity.z);
            }
            break;
        }

        case BoundaryMode::Clamp:
        {
            // Stop at boundary
            boid->position.x = glm::clamp(boid->position.x, m_boundMin.x, m_boundMax.x);
            boid->position.y = glm::clamp(boid->position.y, m_boundMin.y, m_boundMax.y);
            boid->position.z = glm::clamp(boid->position.z, m_boundMin.z, m_boundMax.z);
            break;
        }
    }
}

std::vector<size_t> BoidsSystem::FindNeighbors(size_t boidIndex)
{
    std::vector<size_t> neighbors;
    auto* boid = m_boids[boidIndex];

    for (size_t i = 0; i < m_boids.size(); ++i)
    {
        if (i == boidIndex)
            continue;

        auto* other = m_boids[i];
        if (!other || !other->active)
            continue;

        float distance = glm::distance(boid->position, other->position);

        if (distance < m_perceptionRadius)
        {
            neighbors.push_back(i);
        }
    }

    return neighbors;
}

} // namespace Renderer
} // namespace libprojectM
