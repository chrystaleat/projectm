/**
 * @file ParticleSystem.hpp
 * @brief Manages a collection of Verlet particles with force fields
 */

#pragma once

#include "VerletParticle.hpp"
#include "ForceField.hpp"

#include <projectM-4/projectM_export.h>

#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace libprojectM {
namespace Renderer {

/**
 * @brief Boundary behavior when particles leave bounding box
 */
enum class BoundaryMode
{
    None,           //!< No boundary enforcement
    Kill,           //!< Deactivate particles outside bounds
    Wrap,           //!< Wrap to opposite side (Pac-Man style)
    Bounce,         //!< Reflect velocity (elastic collision)
    Clamp           //!< Clamp position to bounds
};

/**
 * @brief High-performance particle system with Verlet physics
 *
 * Manages thousands of particles with:
 * - Verlet integration physics
 * - Multiple force fields
 * - Spatial constraints (bounding boxes)
 * - Efficient update and rendering
 *
 * Usage:
 * @code
 * ParticleSystem system(10000);
 * system.SetGravity(0, -9.8, 0);
 * system.AddForceField(std::make_shared<AttractorForceField>(0, 0, 0, 100));
 *
 * // Each frame:
 * system.EmitParticle(x, y, z, vx, vy, vz);
 * system.Update(deltaTime);
 * @endcode
 */
class PROJECTM_EXPORT ParticleSystem
{
public:
    /**
     * @brief Construct particle system with maximum capacity
     */
    explicit ParticleSystem(size_t maxParticles = 10000);

    /**
     * @brief Update all particles
     *
     * Applies force fields, integrates physics, handles constraints.
     */
    void Update(float deltaTime);

    /**
     * @brief Emit a new particle
     *
     * @param x,y,z Initial position
     * @param vx,vy,vz Initial velocity
     * @param lifetime Particle lifetime in seconds (-1 = infinite)
     * @param mass Particle mass
     * @return Pointer to created particle, or nullptr if at capacity
     */
    VerletParticle* EmitParticle(float x, float y, float z,
                                 float vx = 0.0f, float vy = 0.0f, float vz = 0.0f,
                                 float lifetime = -1.0f,
                                 float mass = 1.0f);

    /**
     * @brief Clear all particles
     */
    void Clear();

    /**
     * @brief Get active particle count
     */
    size_t GetParticleCount() const { return m_particles.size(); }

    /**
     * @brief Get maximum particle capacity
     */
    size_t GetMaxParticles() const { return m_maxParticles; }

    /**
     * @brief Get particle array for rendering
     */
    const std::vector<VerletParticle>& GetParticles() const { return m_particles; }
    std::vector<VerletParticle>& GetParticles() { return m_particles; }

    // ====== Force Fields ======

    /**
     * @brief Set global gravity
     */
    void SetGravity(float gx, float gy, float gz);

    /**
     * @brief Add a force field
     */
    void AddForceField(std::shared_ptr<ForceField> field);

    /**
     * @brief Clear all force fields
     */
    void ClearForceFields();

    /**
     * @brief Get all force fields
     */
    const std::vector<std::shared_ptr<ForceField>>& GetForceFields() const { return m_forceFields; }

    // ====== Boundaries ======

    /**
     * @brief Set bounding box
     */
    void SetBoundingBox(float minX, float minY, float minZ,
                       float maxX, float maxY, float maxZ);

    /**
     * @brief Set boundary behavior
     */
    void SetBoundaryMode(BoundaryMode mode) { m_boundaryMode = mode; }

    BoundaryMode GetBoundaryMode() const { return m_boundaryMode; }

    // ====== Collision ======

    /**
     * @brief Enable/disable inter-particle collision
     *
     * Warning: This is O(n^2) and very expensive for many particles!
     * Use spatial hashing for large systems.
     */
    void SetEnableCollision(bool enable) { m_enableCollision = enable; }

    /**
     * @brief Set collision elasticity (0 = inelastic, 1 = perfectly elastic)
     */
    void SetCollisionElasticity(float elasticity) { m_collisionElasticity = elasticity; }

private:
    /**
     * @brief Apply all force fields to all particles
     */
    void ApplyForces();

    /**
     * @brief Enforce boundary constraints
     */
    void EnforceBoundaries();

    /**
     * @brief Handle inter-particle collisions (expensive!)
     */
    void HandleCollisions();

    /**
     * @brief Remove dead particles
     */
    void RemoveDeadParticles();

    std::vector<VerletParticle> m_particles;
    size_t m_maxParticles;

    std::vector<std::shared_ptr<ForceField>> m_forceFields;
    std::shared_ptr<GravityForceField> m_gravity;

    // Boundary
    glm::vec3 m_boundMin{-100.0f, -100.0f, -100.0f};
    glm::vec3 m_boundMax{100.0f, 100.0f, 100.0f};
    BoundaryMode m_boundaryMode{BoundaryMode::None};

    // Collision
    bool m_enableCollision{false};
    float m_collisionElasticity{0.8f};
};

} // namespace Renderer
} // namespace libprojectM
