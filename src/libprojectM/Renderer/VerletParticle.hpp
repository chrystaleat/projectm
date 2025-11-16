/**
 * @file VerletParticle.hpp
 * @brief Verlet integration-based particle for stable physics simulation
 *
 * Verlet integration is numerically stable and energy-conserving, perfect for
 * real-time particle systems. Unlike Euler integration, it doesn't accumulate
 * energy errors over time.
 */

#pragma once

#include <projectM-4/projectM_export.h>

#include <glm/glm.hpp>

namespace libprojectM {
namespace Renderer {

/**
 * @brief 3D particle using Verlet integration for physics
 *
 * Verlet integration stores position and previous position (not velocity).
 * Velocity is implicit in the difference between positions.
 *
 * Benefits:
 * - Energy conserving (no velocity drift)
 * - Simple constraint satisfaction
 * - Perfect for cloth, particles, soft bodies
 * - Automatically handles collision response
 */
class PROJECTM_EXPORT VerletParticle
{
public:
    /**
     * @brief Construct particle at position
     */
    VerletParticle(float x = 0.0f, float y = 0.0f, float z = 0.0f);

    /**
     * @brief Update particle physics using Verlet integration
     *
     * Verlet equation:
     * new_pos = 2*pos - old_pos + acceleration*dt^2
     *
     * @param deltaTime Time step in seconds
     */
    void Update(float deltaTime);

    /**
     * @brief Add force to be applied this frame
     *
     * Forces accumulate until Update() is called.
     * Force is converted to acceleration using F = ma
     */
    void AddForce(float fx, float fy, float fz);

    /**
     * @brief Set velocity directly
     *
     * Sets previous position to achieve desired velocity.
     * velocity = (position - previousPosition) / dt
     */
    void SetVelocity(float vx, float vy, float vz, float dt = 0.016f);

    /**
     * @brief Get current velocity
     *
     * Velocity is computed from position difference:
     * v = (current - previous) / dt
     */
    glm::vec3 GetVelocity(float dt = 0.016f) const;

    /**
     * @brief Reset forces to zero
     *
     * Called automatically after Update()
     */
    void ClearForces();

    /**
     * @brief Apply damping (air resistance)
     *
     * Reduces velocity by damping factor each frame.
     * damping = 1.0 means no damping
     * damping = 0.95 means 5% velocity loss per frame
     */
    void ApplyDamping();

    // ====== State ======

    glm::vec3 position{0.0f};           //!< Current position
    glm::vec3 previousPosition{0.0f};   //!< Previous position (for Verlet)
    glm::vec3 acceleration{0.0f};       //!< Accumulated acceleration this frame

    // ====== Properties ======

    float mass{1.0f};                   //!< Particle mass (affects force response)
    float damping{0.99f};               //!< Velocity damping (0-1, 1=no damping)
    float radius{0.1f};                 //!< Particle radius (for collisions)

    glm::vec4 color{1.0f, 1.0f, 1.0f, 1.0f}; //!< RGBA color

    bool active{true};                  //!< Is particle active?
    float lifetime{-1.0f};              //!< Remaining lifetime (-1 = infinite)
    float age{0.0f};                    //!< Time since creation

    // ====== User Data ======

    int userData{0};                    //!< Generic int for user data
};

} // namespace Renderer
} // namespace libprojectM
