/**
 * @file ReactionDiffusionTest.cpp
 * @brief Test suite for Reaction-Diffusion pattern generation
 */

#include <gtest/gtest.h>

#include <Renderer/ReactionDiffusion.hpp>

#include <cmath>

using namespace libprojectM::Renderer;

class ReactionDiffusionTest : public ::testing::Test
{
protected:
    static constexpr float EPSILON = 0.001f;

    void SetUp() override
    {
        // Create 64x64 grid
        reactionDiffusion = std::make_unique<ReactionDiffusion>(64, 64);
    }

    std::unique_ptr<ReactionDiffusion> reactionDiffusion;
};

// ====== Construction Tests ======

TEST_F(ReactionDiffusionTest, Construction)
{
    EXPECT_EQ(reactionDiffusion->GetWidth(), 64);
    EXPECT_EQ(reactionDiffusion->GetHeight(), 64);
}

TEST_F(ReactionDiffusionTest, InitialState_AllU)
{
    // Initial state should be mostly U (chemical A)
    const auto& grid = reactionDiffusion->GetGrid();

    // Check a few cells
    EXPECT_NEAR(grid[0].u, 1.0f, 0.1f); // U starts high
    EXPECT_NEAR(grid[0].v, 0.0f, 0.1f); // V starts low
}

// ====== Grid Access Tests ======

TEST_F(ReactionDiffusionTest, GetCell)
{
    const auto& cell = reactionDiffusion->GetCell(10, 10);

    EXPECT_GE(cell.u, 0.0f);
    EXPECT_LE(cell.u, 1.0f);
    EXPECT_GE(cell.v, 0.0f);
    EXPECT_LE(cell.v, 1.0f);
}

TEST_F(ReactionDiffusionTest, SetCell)
{
    reactionDiffusion->SetCell(10, 10, 0.5f, 0.7f);

    const auto& cell = reactionDiffusion->GetCell(10, 10);

    EXPECT_NEAR(cell.u, 0.5f, EPSILON);
    EXPECT_NEAR(cell.v, 0.7f, EPSILON);
}

// ====== Seeding Tests ======

TEST_F(ReactionDiffusionTest, SeedCenter_CreatesVSpot)
{
    reactionDiffusion->SeedCenter(5); // 5-pixel radius

    // Center should have V (chemical B)
    const auto& centerCell = reactionDiffusion->GetCell(32, 32);
    EXPECT_GT(centerCell.v, 0.5f);

    // Edge should still be mostly U
    const auto& edgeCell = reactionDiffusion->GetCell(0, 0);
    EXPECT_LT(edgeCell.v, 0.1f);
}

TEST_F(ReactionDiffusionTest, SeedRandom_CreatesMultipleSpots)
{
    reactionDiffusion->SeedRandom(10, 2); // 10 spots, radius 2

    // Count cells with significant V
    int vCells = 0;
    const auto& grid = reactionDiffusion->GetGrid();

    for (const auto& cell : grid)
    {
        if (cell.v > 0.5f)
        {
            vCells++;
        }
    }

    // Should have some V spots (at least a few cells)
    EXPECT_GT(vCells, 5);
}

// ====== Parameter Tests ======

TEST_F(ReactionDiffusionTest, SetParameters_Spots)
{
    reactionDiffusion->SetPreset(ReactionDiffusionPreset::Spots);

    // Spot parameters: F=0.055, k=0.062
    EXPECT_NEAR(reactionDiffusion->GetFeedRate(), 0.055f, 0.001f);
    EXPECT_NEAR(reactionDiffusion->GetKillRate(), 0.062f, 0.001f);
}

TEST_F(ReactionDiffusionTest, SetParameters_Stripes)
{
    reactionDiffusion->SetPreset(ReactionDiffusionPreset::Stripes);

    EXPECT_NEAR(reactionDiffusion->GetFeedRate(), 0.035f, 0.001f);
    EXPECT_NEAR(reactionDiffusion->GetKillRate(), 0.065f, 0.001f);
}

TEST_F(ReactionDiffusionTest, SetParameters_Spirals)
{
    reactionDiffusion->SetPreset(ReactionDiffusionPreset::Spirals);

    EXPECT_NEAR(reactionDiffusion->GetFeedRate(), 0.014f, 0.001f);
    EXPECT_NEAR(reactionDiffusion->GetKillRate(), 0.054f, 0.001f);
}

TEST_F(ReactionDiffusionTest, SetCustomParameters)
{
    reactionDiffusion->SetFeedRate(0.123f);
    reactionDiffusion->SetKillRate(0.456f);

    EXPECT_NEAR(reactionDiffusion->GetFeedRate(), 0.123f, EPSILON);
    EXPECT_NEAR(reactionDiffusion->GetKillRate(), 0.456f, EPSILON);
}

// ====== Update/Simulation Tests ======

TEST_F(ReactionDiffusionTest, Update_ChangesConcentrations)
{
    reactionDiffusion->SeedCenter(5);

    const auto& beforeCell = reactionDiffusion->GetCell(32, 32);
    float beforeV = beforeCell.v;

    // Run several iterations
    for (int i = 0; i < 100; ++i)
    {
        reactionDiffusion->Update(0.016f);
    }

    const auto& afterCell = reactionDiffusion->GetCell(32, 32);
    float afterV = afterCell.v;

    // V concentration should have changed
    EXPECT_NE(beforeV, afterV);
}

TEST_F(ReactionDiffusionTest, Update_MaintainsBounds)
{
    reactionDiffusion->SeedRandom(20, 3);

    // Run many iterations
    for (int i = 0; i < 1000; ++i)
    {
        reactionDiffusion->Update(0.016f);
    }

    // All cells should maintain valid concentrations
    const auto& grid = reactionDiffusion->GetGrid();
    for (const auto& cell : grid)
    {
        EXPECT_GE(cell.u, 0.0f);
        EXPECT_LE(cell.u, 1.0f);
        EXPECT_GE(cell.v, 0.0f);
        EXPECT_LE(cell.v, 1.0f);
    }
}

TEST_F(ReactionDiffusionTest, Update_PatternFormation)
{
    reactionDiffusion->SetPreset(ReactionDiffusionPreset::Spots);
    reactionDiffusion->SeedRandom(5, 3);

    // Count initial V cells
    int initialVCells = CountVCells(0.3f);

    // Run simulation
    for (int i = 0; i < 500; ++i)
    {
        reactionDiffusion->Update(0.016f);
    }

    // V should have spread (pattern formed)
    int finalVCells = CountVCells(0.3f);

    EXPECT_NE(initialVCells, finalVCells); // Pattern should change
}

// ====== Diffusion Rate Tests ======

TEST_F(ReactionDiffusionTest, DiffusionRates_AffectSpread)
{
    reactionDiffusion->SetDiffusionRateU(0.2f);
    reactionDiffusion->SetDiffusionRateV(0.1f);

    EXPECT_NEAR(reactionDiffusion->GetDiffusionRateU(), 0.2f, EPSILON);
    EXPECT_NEAR(reactionDiffusion->GetDiffusionRateV(), 0.1f, EPSILON);
}

// ====== Reset Tests ======

TEST_F(ReactionDiffusionTest, Reset_ClearsPattern)
{
    reactionDiffusion->SeedRandom(10, 3);

    // Run simulation
    for (int i = 0; i < 100; ++i)
    {
        reactionDiffusion->Update(0.016f);
    }

    int beforeVCells = CountVCells(0.3f);
    EXPECT_GT(beforeVCells, 10);

    // Reset
    reactionDiffusion->Reset();

    int afterVCells = CountVCells(0.3f);

    // Should be mostly reset
    EXPECT_LT(afterVCells, beforeVCells);
}

// ====== Audio Reactivity Tests ======

TEST_F(ReactionDiffusionTest, AudioModulation_FeedRate)
{
    float baseFeedRate = 0.055f;
    reactionDiffusion->SetFeedRate(baseFeedRate);

    // Apply audio modulation
    float audioEnergy = 0.8f; // High energy
    reactionDiffusion->SetAudioModulation(audioEnergy, 0.5f, 0.0f);

    // Feed rate should be modulated
    // (exact value depends on implementation)
    EXPECT_GE(reactionDiffusion->GetFeedRate(), baseFeedRate * 0.5f);
    EXPECT_LE(reactionDiffusion->GetFeedRate(), baseFeedRate * 1.5f);
}

// ====== Boundary Condition Tests ======

TEST_F(ReactionDiffusionTest, Boundaries_WrapAround)
{
    // Set wrap mode
    reactionDiffusion->SetBoundaryMode(RDBoundaryMode::Wrap);

    // Set edge cells
    reactionDiffusion->SetCell(0, 0, 0.2f, 0.8f);
    reactionDiffusion->SetCell(63, 63, 0.8f, 0.2f);

    // Update should handle boundaries correctly (no crashes)
    for (int i = 0; i < 10; ++i)
    {
        reactionDiffusion->Update(0.016f);
    }

    // Test passed if no crash
    EXPECT_TRUE(true);
}

TEST_F(ReactionDiffusionTest, Boundaries_Fixed)
{
    reactionDiffusion->SetBoundaryMode(RDBoundaryMode::Fixed);

    // Edge cells should remain relatively stable
    const auto& edgeCell = reactionDiffusion->GetCell(0, 0);
    float initialU = edgeCell.u;

    for (int i = 0; i < 100; ++i)
    {
        reactionDiffusion->Update(0.016f);
    }

    const auto& finalCell = reactionDiffusion->GetCell(0, 0);

    // Fixed boundary should prevent much change at edge
    EXPECT_NEAR(finalCell.u, initialU, 0.3f);
}

// ====== Texture Generation Tests ======

TEST_F(ReactionDiffusionTest, GetTextureData_ValidFormat)
{
    auto textureData = reactionDiffusion->GetTextureData();

    // Should have width * height * 3 (RGB) values
    size_t expectedSize = 64 * 64 * 3;
    EXPECT_EQ(textureData.size(), expectedSize);

    // All values should be 0-255
    for (float value : textureData)
    {
        EXPECT_GE(value, 0.0f);
        EXPECT_LE(value, 255.0f);
    }
}

TEST_F(ReactionDiffusionTest, GetTextureData_ReflectsPattern)
{
    reactionDiffusion->SeedCenter(10);

    // Run simulation
    for (int i = 0; i < 100; ++i)
    {
        reactionDiffusion->Update(0.016f);
    }

    auto textureData = reactionDiffusion->GetTextureData();

    // Texture data should vary (not all same color)
    float minValue = 255.0f;
    float maxValue = 0.0f;

    for (float value : textureData)
    {
        minValue = std::min(minValue, value);
        maxValue = std::max(maxValue, value);
    }

    // Should have variation
    EXPECT_GT(maxValue - minValue, 50.0f);
}

// ====== Performance Test ======

TEST_F(ReactionDiffusionTest, LargeGrid_Performance)
{
    // Create larger grid
    auto largeRD = std::make_unique<ReactionDiffusion>(256, 256);
    largeRD->SetPreset(ReactionDiffusionPreset::Spots);
    largeRD->SeedRandom(20, 5);

    // Should be able to update reasonably quickly
    // (This is just a smoke test, not a real benchmark)
    for (int i = 0; i < 10; ++i)
    {
        largeRD->Update(0.016f);
    }

    EXPECT_TRUE(true); // If we got here, performance is acceptable
}

// ====== Helper Methods ======

protected:
    int CountVCells(float threshold)
    {
        int count = 0;
        const auto& grid = reactionDiffusion->GetGrid();

        for (const auto& cell : grid)
        {
            if (cell.v > threshold)
            {
                count++;
            }
        }

        return count;
    }
};
