/**
 * @file BoidsFlockingTest.cpp
 * @brief Test suite for Boids flocking algorithm
 */

#include <gtest/gtest.h>

#include <Renderer/BoidsSystem.hpp>
#include <Renderer/VerletParticle.hpp>

#include <cmath>
#include <numbers>

using namespace libprojectM::Renderer;

class BoidsFlockingTest : public ::testing::Test
{
protected:
    static constexpr float EPSILON = 0.001f;

    void SetUp() override
    {
        boidsSystem = std::make_unique<BoidsSystem>(100);
    }

    std::unique_ptr<BoidsSystem> boidsSystem;
};

// ====== Boid Construction Tests ======

TEST_F(BoidsFlockingTest, Construction)
{
    EXPECT_EQ(boidsSystem->GetBoidCount(), 0);
    EXPECT_EQ(boidsSystem->GetMaxBoids(), 100);
}

TEST_F(BoidsFlockingTest, AddBoid)
{
    auto* boid = boidsSystem->AddBoid(glm::vec3(0.0f, 0.0f, 0.0f),
                                      glm::vec3(1.0f, 0.0f, 0.0f));

    EXPECT_NE(boid, nullptr);
    EXPECT_EQ(boidsSystem->GetBoidCount(), 1);
    EXPECT_NEAR(boid->position.x, 0.0f, EPSILON);
}

TEST_F(BoidsFlockingTest, MaxBoidLimit)
{
    // Fill to capacity
    for (size_t i = 0; i < 100; ++i)
    {
        boidsSystem->AddBoid(glm::vec3(0.0f), glm::vec3(0.0f));
    }

    EXPECT_EQ(boidsSystem->GetBoidCount(), 100);

    // Try to add one more (should fail or recycle)
    auto* extraBoid = boidsSystem->AddBoid(glm::vec3(0.0f), glm::vec3(0.0f));

    // Either null (can't add) or count stays at 100 (recycled)
    EXPECT_LE(boidsSystem->GetBoidCount(), 100);
}

// ====== Separation Behavior Tests ======

TEST_F(BoidsFlockingTest, Separation_AvoidsCrowding)
{
    // Create two boids very close together
    auto* boid1 = boidsSystem->AddBoid(glm::vec3(0.0f, 0.0f, 0.0f),
                                       glm::vec3(0.0f, 0.0f, 0.0f));
    auto* boid2 = boidsSystem->AddBoid(glm::vec3(0.1f, 0.0f, 0.0f),
                                       glm::vec3(0.0f, 0.0f, 0.0f));

    // Enable only separation
    boidsSystem->SetSeparationWeight(1.0f);
    boidsSystem->SetAlignmentWeight(0.0f);
    boidsSystem->SetCohesionWeight(0.0f);

    // Update several frames
    for (int i = 0; i < 30; ++i)
    {
        boidsSystem->Update(0.016f);
    }

    // Boids should have moved apart
    float distance = glm::distance(boid1->position, boid2->position);
    EXPECT_GT(distance, 0.1f);
}

TEST_F(BoidsFlockingTest, Separation_NoEffectWhenFarApart)
{
    // Create two boids far apart
    auto* boid1 = boidsSystem->AddBoid(glm::vec3(0.0f, 0.0f, 0.0f),
                                       glm::vec3(0.0f, 0.0f, 0.0f));
    auto* boid2 = boidsSystem->AddBoid(glm::vec3(10.0f, 0.0f, 0.0f),
                                       glm::vec3(0.0f, 0.0f, 0.0f));

    glm::vec3 initialPos1 = boid1->position;

    // Enable only separation
    boidsSystem->SetSeparationWeight(1.0f);
    boidsSystem->SetAlignmentWeight(0.0f);
    boidsSystem->SetCohesionWeight(0.0f);

    boidsSystem->Update(0.016f);

    // Boid1 should not have moved much (no nearby boids)
    float movement = glm::distance(boid1->position, initialPos1);
    EXPECT_LT(movement, 0.1f);
}

// ====== Alignment Behavior Tests ======

TEST_F(BoidsFlockingTest, Alignment_MatchesNeighborHeading)
{
    // Create boid moving in +X direction
    auto* boid1 = boidsSystem->AddBoid(glm::vec3(0.0f, 0.0f, 0.0f),
                                       glm::vec3(1.0f, 0.0f, 0.0f));

    // Create nearby boid moving in +Y direction
    auto* boid2 = boidsSystem->AddBoid(glm::vec3(1.0f, 0.0f, 0.0f),
                                       glm::vec3(0.0f, 1.0f, 0.0f));

    // Enable only alignment
    boidsSystem->SetSeparationWeight(0.0f);
    boidsSystem->SetAlignmentWeight(1.0f);
    boidsSystem->SetCohesionWeight(0.0f);

    glm::vec3 initialVel1 = boid1->GetVelocity();

    // Update several frames
    for (int i = 0; i < 30; ++i)
    {
        boidsSystem->Update(0.016f);
    }

    glm::vec3 finalVel1 = boid1->GetVelocity();

    // Velocity should have changed (influenced by neighbor)
    float velChange = glm::distance(initialVel1, finalVel1);
    EXPECT_GT(velChange, 0.1f);

    // Should have some Y component now
    EXPECT_GT(std::abs(finalVel1.y), 0.01f);
}

// ====== Cohesion Behavior Tests ======

TEST_F(BoidsFlockingTest, Cohesion_MovesTowardCenter)
{
    // Create boid away from group center
    auto* boid1 = boidsSystem->AddBoid(glm::vec3(5.0f, 0.0f, 0.0f),
                                       glm::vec3(0.0f, 0.0f, 0.0f));

    // Create group of boids at origin
    for (int i = 0; i < 5; ++i)
    {
        boidsSystem->AddBoid(glm::vec3(0.0f, 0.0f, 0.0f),
                            glm::vec3(0.0f, 0.0f, 0.0f));
    }

    // Enable only cohesion
    boidsSystem->SetSeparationWeight(0.0f);
    boidsSystem->SetAlignmentWeight(0.0f);
    boidsSystem->SetCohesionWeight(1.0f);

    float initialDistance = glm::length(boid1->position);

    // Update several frames
    for (int i = 0; i < 50; ++i)
    {
        boidsSystem->Update(0.016f);
    }

    float finalDistance = glm::length(boid1->position);

    // Boid should have moved closer to group center (origin)
    EXPECT_LT(finalDistance, initialDistance);
}

// ====== Combined Behaviors Tests ======

TEST_F(BoidsFlockingTest, CombinedBehaviors_CreatesFlocking)
{
    // Create a scattered group of boids
    for (int i = 0; i < 20; ++i)
    {
        float x = (i % 5) * 2.0f;
        float y = (i / 5) * 2.0f;
        float z = 0.0f;

        // Random velocities
        float vx = (i % 3) - 1.0f;
        float vy = ((i + 1) % 3) - 1.0f;

        boidsSystem->AddBoid(glm::vec3(x, y, z), glm::vec3(vx, vy, 0.0f));
    }

    // Enable all behaviors
    boidsSystem->SetSeparationWeight(1.5f);
    boidsSystem->SetAlignmentWeight(1.0f);
    boidsSystem->SetCohesionWeight(1.0f);

    // Calculate initial spread
    float initialSpread = CalculateGroupSpread();

    // Update for many frames
    for (int i = 0; i < 100; ++i)
    {
        boidsSystem->Update(0.016f);
    }

    // Group should eventually form a cohesive flock
    // (This is hard to test precisely, but we can check they're still somewhat together)
    float finalSpread = CalculateGroupSpread();

    // Spread might increase or decrease, but boids should still be generally together
    // Check that at least some boids are close to each other
    int closeNeighborCount = CountBoidsWithCloseNeighbors(2.0f);
    EXPECT_GT(closeNeighborCount, 10); // More than half should have close neighbors
}

// ====== Perception Radius Tests ======

TEST_F(BoidsFlockingTest, PerceptionRadius_LimitsInfluence)
{
    // Create two boids outside perception radius
    auto* boid1 = boidsSystem->AddBoid(glm::vec3(0.0f, 0.0f, 0.0f),
                                       glm::vec3(1.0f, 0.0f, 0.0f));
    auto* boid2 = boidsSystem->AddBoid(glm::vec3(0.0f, 20.0f, 0.0f),
                                       glm::vec3(0.0f, 1.0f, 0.0f));

    // Set small perception radius
    boidsSystem->SetPerceptionRadius(5.0f);

    // Enable alignment
    boidsSystem->SetSeparationWeight(0.0f);
    boidsSystem->SetAlignmentWeight(1.0f);
    boidsSystem->SetCohesionWeight(0.0f);

    glm::vec3 initialVel1 = boid1->GetVelocity();

    // Update
    for (int i = 0; i < 30; ++i)
    {
        boidsSystem->Update(0.016f);
    }

    glm::vec3 finalVel1 = boid1->GetVelocity();

    // Velocity should not have changed much (boid2 is outside perception)
    float velChange = glm::distance(initialVel1, finalVel1);
    EXPECT_LT(velChange, 0.5f);
}

// ====== Speed Limits Tests ======

TEST_F(BoidsFlockingTest, MaxSpeed_CapsVelocity)
{
    auto* boid = boidsSystem->AddBoid(glm::vec3(0.0f, 0.0f, 0.0f),
                                      glm::vec3(0.0f, 0.0f, 0.0f));

    // Add large force
    boid->AddForce(100.0f, 100.0f, 100.0f);

    // Set max speed
    boidsSystem->SetMaxSpeed(5.0f);

    // Update
    for (int i = 0; i < 30; ++i)
    {
        boidsSystem->Update(0.016f);
    }

    float speed = glm::length(boid->GetVelocity());

    // Speed should be capped
    EXPECT_LE(speed, 5.1f); // Small tolerance
}

TEST_F(BoidsFlockingTest, MaxForce_LimitsSteering)
{
    // Create two very close boids for strong separation force
    auto* boid1 = boidsSystem->AddBoid(glm::vec3(0.0f, 0.0f, 0.0f),
                                       glm::vec3(0.0f, 0.0f, 0.0f));
    boidsSystem->AddBoid(glm::vec3(0.01f, 0.0f, 0.0f),
                        glm::vec3(0.0f, 0.0f, 0.0f));

    // Set low max force
    boidsSystem->SetMaxForce(0.5f);

    // Enable strong separation
    boidsSystem->SetSeparationWeight(10.0f);
    boidsSystem->SetAlignmentWeight(0.0f);
    boidsSystem->SetCohesionWeight(0.0f);

    boidsSystem->Update(0.016f);

    // Check that acceleration is limited
    float accelMag = glm::length(boid1->acceleration);
    EXPECT_LE(accelMag, 0.6f); // Should be close to maxForce
}

// ====== Boundary Handling Tests ======

TEST_F(BoidsFlockingTest, Boundaries_WrapMode)
{
    boidsSystem->SetBoundingBox(-10.0f, -10.0f, -10.0f, 10.0f, 10.0f, 10.0f);
    boidsSystem->SetBoundaryMode(BoundaryMode::Wrap);

    auto* boid = boidsSystem->AddBoid(glm::vec3(9.9f, 0.0f, 0.0f),
                                      glm::vec3(1.0f, 0.0f, 0.0f));

    // Update until boid goes past boundary
    for (int i = 0; i < 10; ++i)
    {
        boidsSystem->Update(0.016f);
    }

    // Should wrap to other side
    EXPECT_LT(boid->position.x, -5.0f); // Wrapped to negative side
}

// ====== Audio Reactivity Tests ======

TEST_F(BoidsFlockingTest, AudioReactive_SpeedModulation)
{
    auto* boid = boidsSystem->AddBoid(glm::vec3(0.0f, 0.0f, 0.0f),
                                      glm::vec3(1.0f, 0.0f, 0.0f));

    // Set base max speed
    boidsSystem->SetMaxSpeed(2.0f);

    // Apply audio energy multiplier (2x speed during high energy)
    boidsSystem->SetAudioEnergyMultiplier(2.0f);

    // Add some force
    boid->AddForce(5.0f, 0.0f, 0.0f);

    for (int i = 0; i < 30; ++i)
    {
        boidsSystem->Update(0.016f);
    }

    float speed = glm::length(boid->GetVelocity());

    // Speed should be able to reach up to 2x base (4.0)
    EXPECT_GT(speed, 2.0f); // Exceeds base max due to audio multiplier
}

// ====== Helper Methods ======

protected:
    float CalculateGroupSpread()
    {
        // Calculate average distance from center of mass
        if (boidsSystem->GetBoidCount() == 0)
            return 0.0f;

        glm::vec3 center(0.0f);
        const auto& boids = boidsSystem->GetBoids();

        for (const auto* boid : boids)
        {
            if (boid && boid->active)
            {
                center += boid->position;
            }
        }
        center /= static_cast<float>(boidsSystem->GetBoidCount());

        float totalDistance = 0.0f;
        for (const auto* boid : boids)
        {
            if (boid && boid->active)
            {
                totalDistance += glm::distance(boid->position, center);
            }
        }

        return totalDistance / static_cast<float>(boidsSystem->GetBoidCount());
    }

    int CountBoidsWithCloseNeighbors(float radius)
    {
        int count = 0;
        const auto& boids = boidsSystem->GetBoids();

        for (size_t i = 0; i < boids.size(); ++i)
        {
            if (!boids[i] || !boids[i]->active)
                continue;

            for (size_t j = i + 1; j < boids.size(); ++j)
            {
                if (!boids[j] || !boids[j]->active)
                    continue;

                float dist = glm::distance(boids[i]->position, boids[j]->position);
                if (dist < radius)
                {
                    count++;
                    break; // Found at least one close neighbor
                }
            }
        }

        return count;
    }
};
