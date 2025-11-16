/**
 * @file QuaternionJuliaTest.cpp
 * @brief Test suite for 4D Quaternion Julia Set fractals
 */

#include <gtest/gtest.h>

#include <Renderer/QuaternionJulia.hpp>
#include <Renderer/Vec4D.hpp>

#include <cmath>

using namespace libprojectM::Renderer;

class QuaternionJuliaTest : public ::testing::Test
{
protected:
    static constexpr float EPSILON = 0.001f;

    void SetUp() override
    {
        juliaSet = std::make_unique<QuaternionJulia>();
    }

    std::unique_ptr<QuaternionJulia> juliaSet;
};

// ====== Quaternion Math Tests ======

TEST_F(QuaternionJuliaTest, Quaternion_Construction)
{
    Quaternion q(1.0f, 2.0f, 3.0f, 4.0f);

    EXPECT_FLOAT_EQ(q.a, 1.0f);
    EXPECT_FLOAT_EQ(q.b, 2.0f);
    EXPECT_FLOAT_EQ(q.c, 3.0f);
    EXPECT_FLOAT_EQ(q.d, 4.0f);
}

TEST_F(QuaternionJuliaTest, Quaternion_Addition)
{
    Quaternion q1(1.0f, 2.0f, 3.0f, 4.0f);
    Quaternion q2(5.0f, 6.0f, 7.0f, 8.0f);

    Quaternion result = q1 + q2;

    EXPECT_FLOAT_EQ(result.a, 6.0f);
    EXPECT_FLOAT_EQ(result.b, 8.0f);
    EXPECT_FLOAT_EQ(result.c, 10.0f);
    EXPECT_FLOAT_EQ(result.d, 12.0f);
}

TEST_F(QuaternionJuliaTest, Quaternion_Multiplication)
{
    // i * i = -1, j * j = -1, k * k = -1
    // i * j = k, j * k = i, k * i = j

    Quaternion i(0.0f, 1.0f, 0.0f, 0.0f);  // Pure i
    Quaternion j(0.0f, 0.0f, 1.0f, 0.0f);  // Pure j

    Quaternion result = i * i;
    EXPECT_NEAR(result.a, -1.0f, EPSILON); // i² = -1

    Quaternion result2 = i * j;
    EXPECT_NEAR(result2.c, 0.0f, EPSILON);
    EXPECT_NEAR(result2.d, 1.0f, EPSILON); // i*j = k
}

TEST_F(QuaternionJuliaTest, Quaternion_Magnitude)
{
    Quaternion q(1.0f, 2.0f, 3.0f, 4.0f);

    // Magnitude = sqrt(1² + 2² + 3² + 4²) = sqrt(30)
    float mag = q.Magnitude();
    EXPECT_NEAR(mag, std::sqrt(30.0f), EPSILON);
}

TEST_F(QuaternionJuliaTest, Quaternion_Square)
{
    Quaternion q(1.0f, 1.0f, 0.0f, 0.0f);
    Quaternion squared = q.Square();

    // (1 + i)² = 1 + 2i + i² = 1 + 2i - 1 = 2i
    EXPECT_NEAR(squared.a, 0.0f, EPSILON);
    EXPECT_NEAR(squared.b, 2.0f, EPSILON);
}

// ====== Julia Set Iteration Tests ======

TEST_F(QuaternionJuliaTest, Iterate_PointInSet)
{
    // Origin with small c should stay bounded
    Quaternion point(0.0f, 0.0f, 0.0f, 0.0f);
    Quaternion c(0.1f, 0.1f, 0.1f, 0.1f);

    int iterations = juliaSet->IteratePoint(point, c, 100);

    // Should reach max iterations (point stays bounded)
    EXPECT_EQ(iterations, 100);
}

TEST_F(QuaternionJuliaTest, Iterate_PointEscapes)
{
    // Large starting point should escape quickly
    Quaternion point(10.0f, 10.0f, 10.0f, 10.0f);
    Quaternion c(0.3f, 0.2f, 0.1f, 0.0f);

    int iterations = juliaSet->IteratePoint(point, c, 100);

    // Should escape in very few iterations
    EXPECT_LT(iterations, 10);
}

TEST_F(QuaternionJuliaTest, Iterate_DifferentC_DifferentResults)
{
    Quaternion point(0.5f, 0.5f, 0.5f, 0.5f);
    Quaternion c1(0.3f, 0.3f, 0.3f, 0.3f);
    Quaternion c2(-0.2f, 0.8f, 0.0f, 0.0f);

    int iter1 = juliaSet->IteratePoint(point, c1, 100);
    int iter2 = juliaSet->IteratePoint(point, c2, 100);

    // Different c values should give different results
    EXPECT_NE(iter1, iter2);
}

// ====== Preset Tests ======

TEST_F(QuaternionJuliaTest, SetPreset_Classic)
{
    juliaSet->SetPreset(JuliaPreset::Classic);

    auto c = juliaSet->GetCParameter();

    // Classic Julia set parameters
    EXPECT_NEAR(c.a, -0.8f, 0.1f);
}

TEST_F(QuaternionJuliaTest, SetPreset_Spiral)
{
    juliaSet->SetPreset(JuliaPreset::Spiral);

    auto c = juliaSet->GetCParameter();

    // Should have some rotation components
    EXPECT_TRUE(std::abs(c.b) > 0.01f || std::abs(c.c) > 0.01f || std::abs(c.d) > 0.01f);
}

// ====== Point Cloud Generation Tests ======

TEST_F(QuaternionJuliaTest, GeneratePointCloud_CreatesPoints)
{
    juliaSet->SetPreset(JuliaPreset::Classic);
    juliaSet->GeneratePointCloud(1000, 2.0f);

    const auto& points = juliaSet->GetPoints();

    EXPECT_GT(points.size(), 0);
    EXPECT_LE(points.size(), 1000);
}

TEST_F(QuaternionJuliaTest, GeneratePointCloud_PointsInBounds)
{
    juliaSet->SetPreset(JuliaPreset::Classic);
    juliaSet->GeneratePointCloud(500, 2.0f);

    const auto& points = juliaSet->GetPoints();

    // All points should be within specified bounds
    for (const auto& point : points)
    {
        float magnitude = point.Length();
        EXPECT_LE(magnitude, 3.0f); // Some margin
    }
}

TEST_F(QuaternionJuliaTest, GeneratePointCloud_DensityAffectsCount)
{
    juliaSet->SetPreset(JuliaPreset::Classic);

    juliaSet->GeneratePointCloud(100, 2.0f);
    size_t count1 = juliaSet->GetPoints().size();

    juliaSet->GeneratePointCloud(1000, 2.0f);
    size_t count2 = juliaSet->GetPoints().size();

    // More samples should generally yield more points in set
    EXPECT_GT(count2, count1);
}

// ====== Audio Reactivity Tests ======

TEST_F(QuaternionJuliaTest, AudioModulation_RotatesC)
{
    juliaSet->SetPreset(JuliaPreset::Classic);
    auto cBefore = juliaSet->GetCParameter();

    // Apply audio modulation
    juliaSet->SetAudioModulation(0.5f, 0.8f, 1.0f);

    auto cAfter = juliaSet->GetCParameter();

    // C parameter should have changed
    EXPECT_NE(cBefore.b, cAfter.b); // Rotation components changed
}

TEST_F(QuaternionJuliaTest, AudioModulation_EnergyAffectsSize)
{
    juliaSet->SetPreset(JuliaPreset::Classic);

    // Low energy
    juliaSet->SetAudioModulation(0.1f, 0.0f, 0.0f);
    float size1 = juliaSet->GetRenderSize();

    // High energy
    juliaSet->SetAudioModulation(0.9f, 0.0f, 0.0f);
    float size2 = juliaSet->GetRenderSize();

    // Higher energy should increase size
    EXPECT_GT(size2, size1);
}

// ====== Escape Threshold Tests ======

TEST_F(QuaternionJuliaTest, EscapeThreshold_AffectsIteration)
{
    Quaternion point(1.5f, 1.5f, 0.0f, 0.0f);
    Quaternion c(0.3f, 0.2f, 0.1f, 0.0f);

    juliaSet->SetEscapeRadius(2.0f);
    int iter1 = juliaSet->IteratePoint(point, c, 100);

    juliaSet->SetEscapeRadius(10.0f);
    int iter2 = juliaSet->IteratePoint(point, c, 100);

    // Larger escape radius should allow more iterations
    EXPECT_GE(iter2, iter1);
}

// ====== Custom C Parameter Tests ======

TEST_F(QuaternionJuliaTest, SetCustomC)
{
    Quaternion customC(0.123f, 0.456f, 0.789f, 0.012f);

    juliaSet->SetCParameter(customC);

    auto retrieved = juliaSet->GetCParameter();

    EXPECT_NEAR(retrieved.a, 0.123f, EPSILON);
    EXPECT_NEAR(retrieved.b, 0.456f, EPSILON);
    EXPECT_NEAR(retrieved.c, 0.789f, EPSILON);
    EXPECT_NEAR(retrieved.d, 0.012f, EPSILON);
}

// ====== Conversion Tests ======

TEST_F(QuaternionJuliaTest, QuaternionToVec4D)
{
    Quaternion q(1.0f, 2.0f, 3.0f, 4.0f);
    Vec4D v = q.ToVec4D();

    EXPECT_FLOAT_EQ(v.x, 1.0f);
    EXPECT_FLOAT_EQ(v.y, 2.0f);
    EXPECT_FLOAT_EQ(v.z, 3.0f);
    EXPECT_FLOAT_EQ(v.w, 4.0f);
}

TEST_F(QuaternionJuliaTest, Vec4DToQuaternion)
{
    Vec4D v(5.0f, 6.0f, 7.0f, 8.0f);
    Quaternion q = Quaternion::FromVec4D(v);

    EXPECT_FLOAT_EQ(q.a, 5.0f);
    EXPECT_FLOAT_EQ(q.b, 6.0f);
    EXPECT_FLOAT_EQ(q.c, 7.0f);
    EXPECT_FLOAT_EQ(q.d, 8.0f);
}

// ====== Preset Variety Tests ======

TEST_F(QuaternionJuliaTest, AllPresets_Unique)
{
    juliaSet->SetPreset(JuliaPreset::Classic);
    auto c1 = juliaSet->GetCParameter();

    juliaSet->SetPreset(JuliaPreset::Spiral);
    auto c2 = juliaSet->GetCParameter();

    juliaSet->SetPreset(JuliaPreset::Dragon);
    auto c3 = juliaSet->GetCParameter();

    // Each preset should have unique C parameters
    EXPECT_TRUE(c1.a != c2.a || c1.b != c2.b);
    EXPECT_TRUE(c2.a != c3.a || c2.b != c3.b);
}

// ====== Performance Test ======

TEST_F(QuaternionJuliaTest, LargePointCloud_Performance)
{
    juliaSet->SetPreset(JuliaPreset::Classic);

    // Should be able to generate many points reasonably quickly
    juliaSet->GeneratePointCloud(10000, 2.5f);

    const auto& points = juliaSet->GetPoints();

    // Should have generated some points
    EXPECT_GT(points.size(), 100);
}

// ====== Iteration Limit Tests ======

TEST_F(QuaternionJuliaTest, MaxIterations_LimitsComputation)
{
    Quaternion point(0.0f, 0.0f, 0.0f, 0.0f);
    Quaternion c(0.01f, 0.01f, 0.01f, 0.01f);

    juliaSet->SetMaxIterations(50);
    int iter1 = juliaSet->IteratePoint(point, c, 50);

    juliaSet->SetMaxIterations(200);
    int iter2 = juliaSet->IteratePoint(point, c, 200);

    // Second should allow more iterations
    EXPECT_GE(iter2, iter1);
}
