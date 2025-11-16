/**
 * @file BoidsSystem.hpp
 * @brief Boids flocking algorithm for organic swarm behavior
 *
 * Implements Craig Reynolds' Boids algorithm with three behavioral rules:
 * 1. Separation: Avoid crowding neighbors
 * 2. Alignment: Steer toward average heading of neighbors
 * 3. Cohesion: Steer toward average position of neighbors
 *
 * This creates emergent flocking behavior similar to birds, fish, or insects.
 */

#pragma once

#include "VerletParticle.hpp"

#include <projectM-4/projectM_export.h>

#include <glm/glm.hpp>

#include <memory>
#include <vector>

namespace libprojectM {
namespace Renderer {

/**
 * @brief Boundary behavior for boids
 */
enum class BoundaryMode
{
    Kill,    //!< Remove boids that leave bounds
    Wrap,    //!< Wrap to opposite side (toroidal space)
    Bounce,  //!< Reflect velocity at boundaries
    Clamp    //!< Stop at boundary edge
};

/**
 * @brief Manages a flock of boids with emergent swarm behavior
 *
 * The Boids algorithm creates realistic flocking by applying three simple rules
 * to each boid based on its neighbors within a perception radius:
 *
 * - **Separation**: Avoid getting too close to neighbors
 * - **Alignment**: Match velocity with nearby boids
 * - **Cohesion**: Move toward the center of nearby boids
 *
 * Each behavior can be weighted independently to create different effects:
 * - High separation + low cohesion = scattered, chaotic
 * - High cohesion + high alignment = tight, coordinated swarms
 * - Balanced weights = natural-looking flocks
 *
 * Audio reactivity features:
 * - Speed modulation based on audio energy
 * - Force modulation based on beat detection
 * - Color/size mapping to frequency bands
 *
 * Usage:
 * @code
 * BoidsSystem flock(1000);
 * flock.SetSeparationWeight(1.5f);
 * flock.SetAlignmentWeight(1.0f);
 * flock.SetCohesionWeight(1.0f);
 * flock.SetPerceptionRadius(3.0f);
 * flock.SetMaxSpeed(5.0f);
 *
 * // Spawn boids
 * for (int i = 0; i < 500; ++i)
 * {
 *     flock.AddBoid(randomPos, randomVel);
 * }
 *
 * // Each frame
 * flock.SetAudioEnergyMultiplier(audioEnergy);
 * flock.Update(deltaTime);
 * @endcode
 */
class PROJECTM_EXPORT BoidsSystem
{
public:
    /**
     * @brief Construct a boids system
     *
     * @param maxBoids Maximum number of boids to manage
     */
    explicit BoidsSystem(size_t maxBoids = 1000);

    /**
     * @brief Add a new boid to the system
     *
     * @param position Initial position
     * @param velocity Initial velocity
     * @return Pointer to created boid, or nullptr if at capacity
     */
    VerletParticle* AddBoid(const glm::vec3& position, const glm::vec3& velocity);

    /**
     * @brief Update all boids
     *
     * Applies flocking behaviors and physics.
     *
     * @param deltaTime Time step in seconds
     */
    void Update(float deltaTime);

    /**
     * @brief Get all active boids
     *
     * @return Vector of boid pointers (may contain nullptrs for inactive boids)
     */
    const std::vector<VerletParticle*>& GetBoids() const { return m_boids; }

    /**
     * @brief Get current boid count
     */
    size_t GetBoidCount() const { return m_activeBoidCount; }

    /**
     * @brief Get maximum boid capacity
     */
    size_t GetMaxBoids() const { return m_maxBoids; }

    /**
     * @brief Clear all boids
     */
    void Clear();

    // ====== Behavior Weights ======

    /**
     * @brief Set separation force weight
     *
     * Higher values = stronger avoidance of neighbors
     *
     * @param weight Separation weight (default 1.5)
     */
    void SetSeparationWeight(float weight) { m_separationWeight = weight; }

    /**
     * @brief Set alignment force weight
     *
     * Higher values = stronger velocity matching
     *
     * @param weight Alignment weight (default 1.0)
     */
    void SetAlignmentWeight(float weight) { m_alignmentWeight = weight; }

    /**
     * @brief Set cohesion force weight
     *
     * Higher values = stronger attraction to group center
     *
     * @param weight Cohesion weight (default 1.0)
     */
    void SetCohesionWeight(float weight) { m_cohesionWeight = weight; }

    // ====== Perception and Limits ======

    /**
     * @brief Set perception radius
     *
     * Boids only react to neighbors within this distance.
     *
     * @param radius Perception radius (default 3.0)
     */
    void SetPerceptionRadius(float radius) { m_perceptionRadius = radius; }

    /**
     * @brief Set maximum speed
     *
     * @param speed Max speed (default 5.0)
     */
    void SetMaxSpeed(float speed) { m_maxSpeed = speed; }

    /**
     * @brief Set maximum steering force
     *
     * Limits how quickly boids can change direction.
     *
     * @param force Max force (default 0.5)
     */
    void SetMaxForce(float force) { m_maxForce = force; }

    // ====== Boundaries ======

    /**
     * @brief Set bounding box
     *
     * @param minX, minY, minZ Minimum bounds
     * @param maxX, maxY, maxZ Maximum bounds
     */
    void SetBoundingBox(float minX, float minY, float minZ,
                       float maxX, float maxY, float maxZ);

    /**
     * @brief Set boundary behavior
     *
     * @param mode Boundary mode (Kill, Wrap, Bounce, Clamp)
     */
    void SetBoundaryMode(BoundaryMode mode) { m_boundaryMode = mode; }

    // ====== Audio Reactivity ======

    /**
     * @brief Set audio energy multiplier
     *
     * Modulates max speed based on audio energy.
     * 1.0 = no effect, 2.0 = double speed at high energy
     *
     * @param multiplier Energy multiplier (1.0 - 3.0)
     */
    void SetAudioEnergyMultiplier(float multiplier) { m_audioEnergyMultiplier = multiplier; }

    /**
     * @brief Set beat intensity
     *
     * Temporarily boosts separation on beats for pulsing effect.
     *
     * @param intensity Beat intensity (0.0 - 1.0)
     */
    void SetBeatIntensity(float intensity) { m_beatIntensity = intensity; }

private:
    /**
     * @brief Calculate separation steering force
     */
    glm::vec3 CalculateSeparation(size_t boidIndex);

    /**
     * @brief Calculate alignment steering force
     */
    glm::vec3 CalculateAlignment(size_t boidIndex);

    /**
     * @brief Calculate cohesion steering force
     */
    glm::vec3 CalculateCohesion(size_t boidIndex);

    /**
     * @brief Apply steering force with max force limit
     */
    glm::vec3 LimitForce(const glm::vec3& force);

    /**
     * @brief Apply speed limit to boid
     */
    void ApplySpeedLimit(VerletParticle* boid);

    /**
     * @brief Handle boundary conditions
     */
    void HandleBoundaries(VerletParticle* boid);

    /**
     * @brief Find neighbors within perception radius
     */
    std::vector<size_t> FindNeighbors(size_t boidIndex);

    // ====== Boid Storage ======

    std::vector<VerletParticle*> m_boids;        //!< All boids (active and inactive)
    size_t m_maxBoids;                           //!< Maximum capacity
    size_t m_activeBoidCount{0};                 //!< Current active count

    // ====== Behavior Parameters ======

    float m_separationWeight{1.5f};              //!< Separation force multiplier
    float m_alignmentWeight{1.0f};               //!< Alignment force multiplier
    float m_cohesionWeight{1.0f};                //!< Cohesion force multiplier

    float m_perceptionRadius{3.0f};              //!< Neighbor detection radius
    float m_separationRadius{1.0f};              //!< Desired minimum spacing

    float m_maxSpeed{5.0f};                      //!< Maximum velocity magnitude
    float m_maxForce{0.5f};                      //!< Maximum steering force

    // ====== Boundaries ======

    glm::vec3 m_boundMin{-100.0f, -100.0f, -100.0f};  //!< Bounding box minimum
    glm::vec3 m_boundMax{100.0f, 100.0f, 100.0f};     //!< Bounding box maximum
    BoundaryMode m_boundaryMode{BoundaryMode::Wrap};   //!< Boundary behavior

    // ====== Audio Reactivity ======

    float m_audioEnergyMultiplier{1.0f};         //!< Audio energy speed multiplier
    float m_beatIntensity{0.0f};                 //!< Current beat intensity
};

} // namespace Renderer
} // namespace libprojectM
