/**
 * @file VerletPhysicsTest.cpp
 * @brief Test suite for Verlet integration physics engine
 */

#include <gtest/gtest.h>

#include <Renderer/VerletParticle.hpp>
#include <Renderer/ParticleSystem.hpp>
#include <Renderer/ForceField.hpp>

#include <cmath>
#include <numbers>

using namespace libprojectM::Renderer;

class VerletPhysicsTest : public ::testing::Test
{
protected:
    static constexpr float EPSILON = 0.001f;
};

// ====== VerletParticle Tests ======

TEST_F(VerletPhysicsTest, ParticleInitialization)
{
    VerletParticle particle(1.0f, 2.0f, 3.0f);

    EXPECT_FLOAT_EQ(particle.position.x, 1.0f);
    EXPECT_FLOAT_EQ(particle.position.y, 2.0f);
    EXPECT_FLOAT_EQ(particle.position.z, 3.0f);

    // Previous position should match current (zero initial velocity)
    EXPECT_FLOAT_EQ(particle.previousPosition.x, 1.0f);
    EXPECT_FLOAT_EQ(particle.previousPosition.y, 2.0f);
    EXPECT_FLOAT_EQ(particle.previousPosition.z, 3.0f);

    EXPECT_FLOAT_EQ(particle.mass, 1.0f);
    EXPECT_TRUE(particle.active);
}

TEST_F(VerletPhysicsTest, ParticleVelocity)
{
    VerletParticle particle(0.0f, 0.0f, 0.0f);

    // Set initial velocity
    particle.SetVelocity(1.0f, 0.0f, 0.0f);

    // Previous position should reflect velocity
    EXPECT_FLOAT_EQ(particle.previousPosition.x, -1.0f);
    EXPECT_FLOAT_EQ(particle.previousPosition.y, 0.0f);
    EXPECT_FLOAT_EQ(particle.previousPosition.z, 0.0f);

    // Get velocity should return what we set
    auto vel = particle.GetVelocity();
    EXPECT_NEAR(vel.x, 1.0f, EPSILON);
    EXPECT_NEAR(vel.y, 0.0f, EPSILON);
    EXPECT_NEAR(vel.z, 0.0f, EPSILON);
}

TEST_F(VerletPhysicsTest, VerletIntegration_FreeFall)
{
    VerletParticle particle(0.0f, 10.0f, 0.0f);
    particle.SetVelocity(0.0f, 0.0f, 0.0f);

    float dt = 0.016f; // ~60 FPS
    float gravity = -9.8f;

    // Apply gravity force for several frames
    for (int i = 0; i < 60; ++i)
    {
        particle.AddForce(0.0f, gravity * particle.mass, 0.0f);
        particle.Update(dt);
    }

    // After 1 second, particle should have fallen
    // s = 0.5 * g * t^2 = 0.5 * 9.8 * 1^2 = 4.9 meters
    EXPECT_LT(particle.position.y, 10.0f);
    EXPECT_GT(particle.position.y, 0.0f);

    // Velocity should be downward
    auto vel = particle.GetVelocity();
    EXPECT_LT(vel.y, -5.0f); // Should be falling fast
}

TEST_F(VerletPhysicsTest, VerletIntegration_ProjectileMotion)
{
    VerletParticle particle(0.0f, 0.0f, 0.0f);

    // Launch at 45 degrees
    float speed = 10.0f;
    float angle = std::numbers::pi_v<float> / 4.0f;
    particle.SetVelocity(speed * std::cos(angle), speed * std::sin(angle), 0.0f);

    float dt = 0.016f;
    float gravity = -9.8f;

    float maxHeight = 0.0f;

    // Simulate projectile motion
    for (int i = 0; i < 120; ++i) // 2 seconds
    {
        particle.AddForce(0.0f, gravity * particle.mass, 0.0f);
        particle.Update(dt);

        maxHeight = std::max(maxHeight, particle.position.y);
    }

    // Should have reached some height
    EXPECT_GT(maxHeight, 1.0f);

    // Should have traveled horizontally
    EXPECT_GT(particle.position.x, 5.0f);
}

TEST_F(VerletPhysicsTest, ParticleDamping)
{
    VerletParticle particle(0.0f, 0.0f, 0.0f);
    particle.SetVelocity(10.0f, 0.0f, 0.0f);
    particle.damping = 0.95f; // 5% energy loss per frame

    float dt = 0.016f;

    // Simulate with damping
    for (int i = 0; i < 60; ++i)
    {
        particle.Update(dt);
    }

    // Velocity should have decreased significantly
    auto vel = particle.GetVelocity();
    EXPECT_LT(std::abs(vel.x), 5.0f); // Should have slowed down
}

// ====== ParticleSystem Tests ======

TEST_F(VerletPhysicsTest, ParticleSystemCreation)
{
    ParticleSystem system(100);

    EXPECT_EQ(system.GetParticleCount(), 0);
    EXPECT_EQ(system.GetMaxParticles(), 100);
}

TEST_F(VerletPhysicsTest, ParticleSystemEmit)
{
    ParticleSystem system(100);

    system.EmitParticle(0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);

    EXPECT_EQ(system.GetParticleCount(), 1);
}

TEST_F(VerletPhysicsTest, ParticleSystemMaxCapacity)
{
    ParticleSystem system(10);

    // Emit more than capacity
    for (int i = 0; i < 20; ++i)
    {
        system.EmitParticle(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    }

    // Should not exceed max
    EXPECT_LE(system.GetParticleCount(), 10);
}

TEST_F(VerletPhysicsTest, ParticleSystemUpdate)
{
    ParticleSystem system(100);
    system.SetGravity(0.0f, -9.8f, 0.0f);

    // Emit particle at rest
    system.EmitParticle(0.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f);

    float dt = 0.016f;

    // Update for 1 second
    for (int i = 0; i < 60; ++i)
    {
        system.Update(dt);
    }

    // Particle should have fallen
    auto& particles = system.GetParticles();
    ASSERT_GT(particles.size(), 0);
    EXPECT_LT(particles[0].position.y, 10.0f);
}

// ====== Force Field Tests ======

TEST_F(VerletPhysicsTest, GravityForceField)
{
    GravityForceField gravity(0.0f, -9.8f, 0.0f);

    VerletParticle particle(0.0f, 0.0f, 0.0f);
    particle.mass = 2.0f;

    gravity.ApplyForce(particle);

    // Force should be mass * gravity
    EXPECT_FLOAT_EQ(particle.acceleration.x, 0.0f);
    EXPECT_FLOAT_EQ(particle.acceleration.y, -9.8f);
    EXPECT_FLOAT_EQ(particle.acceleration.z, 0.0f);
}

TEST_F(VerletPhysicsTest, AttractorForceField)
{
    AttractorForceField attractor(0.0f, 0.0f, 0.0f, 100.0f);

    VerletParticle particle(10.0f, 0.0f, 0.0f);

    attractor.ApplyForce(particle);

    // Force should point toward attractor (negative x direction)
    EXPECT_LT(particle.acceleration.x, 0.0f);
    EXPECT_NEAR(particle.acceleration.y, 0.0f, EPSILON);
    EXPECT_NEAR(particle.acceleration.z, 0.0f, EPSILON);
}

TEST_F(VerletPhysicsTest, VortexForceField)
{
    VortexForceField vortex(0.0f, 0.0f, 0.0f, 50.0f, 0.0f, 0.0f, 1.0f);

    VerletParticle particle(5.0f, 0.0f, 0.0f);

    vortex.ApplyForce(particle);

    // Should have tangential force (perpendicular to radius)
    // At (5,0,0), tangent should be in +Y direction
    EXPECT_NEAR(particle.acceleration.x, 0.0f, EPSILON);
    EXPECT_GT(std::abs(particle.acceleration.y), 0.0f);
}

TEST_F(VerletPhysicsTest, TurbulenceForceField)
{
    TurbulenceForceField turbulence(10.0f, 0.5f);

    VerletParticle particle(1.0f, 2.0f, 3.0f);

    turbulence.ApplyForce(particle);

    // Should add some random force
    EXPECT_NE(particle.acceleration.x, 0.0f);
    EXPECT_NE(particle.acceleration.y, 0.0f);
    EXPECT_NE(particle.acceleration.z, 0.0f);
}

// ====== Integration Tests ======

TEST_F(VerletPhysicsTest, ParticleSystemWithMultipleForces)
{
    ParticleSystem system(100);

    // Add force fields
    system.AddForceField(std::make_shared<GravityForceField>(0.0f, -5.0f, 0.0f));
    system.AddForceField(std::make_shared<AttractorForceField>(0.0f, 0.0f, 0.0f, 20.0f));

    // Emit particles
    for (int i = 0; i < 10; ++i)
    {
        float angle = 2.0f * std::numbers::pi_v<float> * i / 10.0f;
        float radius = 10.0f;
        system.EmitParticle(radius * std::cos(angle),
                          radius * std::sin(angle),
                          0.0f,
                          0.0f, 0.0f, 0.0f);
    }

    float dt = 0.016f;

    // Update system
    for (int i = 0; i < 60; ++i)
    {
        system.Update(dt);
    }

    // Particles should be attracted toward center and fall
    auto& particles = system.GetParticles();
    for (const auto& p : particles)
    {
        // Should have moved from initial position
        float dist = std::sqrt(p.position.x * p.position.x + p.position.y * p.position.y);
        EXPECT_LT(dist, 10.0f); // Moved inward due to attractor
    }
}

TEST_F(VerletPhysicsTest, ParticleLifetime)
{
    ParticleSystem system(100);
    system.EmitParticle(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f); // 1 second lifetime

    EXPECT_EQ(system.GetParticleCount(), 1);

    float dt = 0.5f;
    system.Update(dt); // 0.5 seconds

    EXPECT_EQ(system.GetParticleCount(), 1); // Still alive

    system.Update(dt); // 1.0 seconds total

    EXPECT_EQ(system.GetParticleCount(), 1); // Just expired

    system.Update(dt); // 1.5 seconds

    EXPECT_EQ(system.GetParticleCount(), 0); // Should be removed
}

TEST_F(VerletPhysicsTest, BoundingBoxConstraint)
{
    ParticleSystem system(100);
    system.SetBoundingBox(-10.0f, -10.0f, -10.0f, 10.0f, 10.0f, 10.0f);
    system.SetBoundaryMode(BoundaryMode::Bounce);

    // Emit particle moving fast in +X
    system.EmitParticle(0.0f, 0.0f, 0.0f, 20.0f, 0.0f, 0.0f);

    float dt = 0.016f;

    // Update for a while
    for (int i = 0; i < 120; ++i)
    {
        system.Update(dt);
    }

    // Particle should stay within bounds
    auto& particles = system.GetParticles();
    ASSERT_GT(particles.size(), 0);
    EXPECT_GE(particles[0].position.x, -10.0f);
    EXPECT_LE(particles[0].position.x, 10.0f);
}
