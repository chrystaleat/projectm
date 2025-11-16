/**
 * @file Hyperdimensional4DTest.cpp
 * @brief Test suite for 4D hyperdimensional math and visualization
 */

#include <gtest/gtest.h>

#include <Renderer/Vec4D.hpp>
#include <Renderer/Tesseract.hpp>
#include <Renderer/HyperdimensionalProjection.hpp>

#include <cmath>
#include <numbers>

using namespace libprojectM::Renderer;

class Hyperdimensional4DTest : public ::testing::Test
{
protected:
    static constexpr float EPSILON = 0.001f;
};

// ====== Vec4D Tests ======

TEST_F(Hyperdimensional4DTest, Vec4D_Construction)
{
    Vec4D v(1.0f, 2.0f, 3.0f, 4.0f);

    EXPECT_FLOAT_EQ(v.x, 1.0f);
    EXPECT_FLOAT_EQ(v.y, 2.0f);
    EXPECT_FLOAT_EQ(v.z, 3.0f);
    EXPECT_FLOAT_EQ(v.w, 4.0f);
}

TEST_F(Hyperdimensional4DTest, Vec4D_Addition)
{
    Vec4D v1(1.0f, 2.0f, 3.0f, 4.0f);
    Vec4D v2(5.0f, 6.0f, 7.0f, 8.0f);

    Vec4D result = v1 + v2;

    EXPECT_FLOAT_EQ(result.x, 6.0f);
    EXPECT_FLOAT_EQ(result.y, 8.0f);
    EXPECT_FLOAT_EQ(result.z, 10.0f);
    EXPECT_FLOAT_EQ(result.w, 12.0f);
}

TEST_F(Hyperdimensional4DTest, Vec4D_DotProduct)
{
    Vec4D v1(1.0f, 0.0f, 0.0f, 0.0f);
    Vec4D v2(1.0f, 0.0f, 0.0f, 0.0f);

    float dot = v1.Dot(v2);

    EXPECT_FLOAT_EQ(dot, 1.0f);

    Vec4D v3(1.0f, 2.0f, 3.0f, 4.0f);
    Vec4D v4(5.0f, 6.0f, 7.0f, 8.0f);

    // Dot = 1*5 + 2*6 + 3*7 + 4*8 = 5 + 12 + 21 + 32 = 70
    float dot2 = v3.Dot(v4);
    EXPECT_FLOAT_EQ(dot2, 70.0f);
}

TEST_F(Hyperdimensional4DTest, Vec4D_Length)
{
    Vec4D v(3.0f, 4.0f, 0.0f, 0.0f);

    // Length = sqrt(3^2 + 4^2) = sqrt(9 + 16) = sqrt(25) = 5
    EXPECT_FLOAT_EQ(v.Length(), 5.0f);

    Vec4D v2(1.0f, 1.0f, 1.0f, 1.0f);
    // Length = sqrt(1 + 1 + 1 + 1) = 2
    EXPECT_FLOAT_EQ(v2.Length(), 2.0f);
}

TEST_F(Hyperdimensional4DTest, Vec4D_Normalize)
{
    Vec4D v(3.0f, 4.0f, 0.0f, 0.0f);
    Vec4D normalized = v.Normalized();

    EXPECT_NEAR(normalized.Length(), 1.0f, EPSILON);
    EXPECT_NEAR(normalized.x, 0.6f, EPSILON);  // 3/5
    EXPECT_NEAR(normalized.y, 0.8f, EPSILON);  // 4/5
}

// ====== 4D Rotation Tests ======

TEST_F(Hyperdimensional4DTest, Rotation4D_XY_Plane)
{
    Vec4D v(1.0f, 0.0f, 0.0f, 0.0f);

    // Rotate 90 degrees in XY plane
    Vec4D rotated = Rotate4D_XY(v, std::numbers::pi_v<float> / 2.0f);

    // Should rotate (1,0,0,0) to (0,1,0,0)
    EXPECT_NEAR(rotated.x, 0.0f, EPSILON);
    EXPECT_NEAR(rotated.y, 1.0f, EPSILON);
    EXPECT_NEAR(rotated.z, 0.0f, EPSILON);
    EXPECT_NEAR(rotated.w, 0.0f, EPSILON);
}

TEST_F(Hyperdimensional4DTest, Rotation4D_ZW_Plane)
{
    Vec4D v(0.0f, 0.0f, 1.0f, 0.0f);

    // Rotate 90 degrees in ZW plane
    Vec4D rotated = Rotate4D_ZW(v, std::numbers::pi_v<float> / 2.0f);

    // Should rotate (0,0,1,0) to (0,0,0,1)
    EXPECT_NEAR(rotated.x, 0.0f, EPSILON);
    EXPECT_NEAR(rotated.y, 0.0f, EPSILON);
    EXPECT_NEAR(rotated.z, 0.0f, EPSILON);
    EXPECT_NEAR(rotated.w, 1.0f, EPSILON);
}

TEST_F(Hyperdimensional4DTest, Rotation4D_Double)
{
    Vec4D v(1.0f, 0.0f, 1.0f, 0.0f);

    // Double rotation: XY and ZW simultaneously
    Vec4D rotated = Rotate4D_Double(v,
                                    std::numbers::pi_v<float> / 2.0f,  // XY
                                    std::numbers::pi_v<float> / 2.0f); // ZW

    // XY: (1,0,_,_) -> (0,1,_,_)
    // ZW: (_,_,1,0) -> (_,_,0,1)
    // Result: (0,1,0,1)

    EXPECT_NEAR(rotated.x, 0.0f, EPSILON);
    EXPECT_NEAR(rotated.y, 1.0f, EPSILON);
    EXPECT_NEAR(rotated.z, 0.0f, EPSILON);
    EXPECT_NEAR(rotated.w, 1.0f, EPSILON);
}

TEST_F(Hyperdimensional4DTest, Rotation4D_PreservesLength)
{
    Vec4D v(3.0f, 4.0f, 5.0f, 6.0f);
    float originalLength = v.Length();

    // Rotate by arbitrary angle
    Vec4D rotated = Rotate4D_Double(v, 0.7f, 1.3f);

    // Length should be preserved
    EXPECT_NEAR(rotated.Length(), originalLength, EPSILON);
}

// ====== Tesseract Tests ======

TEST_F(Hyperdimensional4DTest, Tesseract_Construction)
{
    Tesseract tesseract(2.0f); // Side length 2

    // Tesseract has 16 vertices
    EXPECT_EQ(tesseract.GetVertices().size(), 16);

    // And 32 edges
    EXPECT_EQ(tesseract.GetEdges().size(), 32);
}

TEST_F(Hyperdimensional4DTest, Tesseract_VertexPositions)
{
    Tesseract tesseract(1.0f);

    auto& vertices = tesseract.GetVertices();

    // Each vertex should be at distance sqrt(4) = 2 from origin
    // (since all coordinates are ±1, and sqrt(1² + 1² + 1² + 1²) = 2)
    for (const auto& vertex : vertices)
    {
        float distance = vertex.Length();
        EXPECT_NEAR(distance, 2.0f, EPSILON);
    }

    // Vertices should all have coordinates ±1
    for (const auto& vertex : vertices)
    {
        EXPECT_TRUE(std::abs(std::abs(vertex.x) - 1.0f) < EPSILON);
        EXPECT_TRUE(std::abs(std::abs(vertex.y) - 1.0f) < EPSILON);
        EXPECT_TRUE(std::abs(std::abs(vertex.z) - 1.0f) < EPSILON);
        EXPECT_TRUE(std::abs(std::abs(vertex.w) - 1.0f) < EPSILON);
    }
}

TEST_F(Hyperdimensional4DTest, Tesseract_Rotation)
{
    Tesseract tesseract(1.0f);

    auto verticesBefore = tesseract.GetVertices();

    // Rotate tesseract
    tesseract.Rotate(0.1f, 0.2f);

    auto verticesAfter = tesseract.GetVertices();

    // Vertices should have changed
    bool changed = false;
    for (size_t i = 0; i < verticesBefore.size(); ++i)
    {
        if ((verticesBefore[i] - verticesAfter[i]).Length() > EPSILON)
        {
            changed = true;
            break;
        }
    }

    EXPECT_TRUE(changed);

    // But distance from origin should be preserved
    for (size_t i = 0; i < verticesBefore.size(); ++i)
    {
        EXPECT_NEAR(verticesBefore[i].Length(), verticesAfter[i].Length(), EPSILON);
    }
}

// ====== Projection Tests ======

TEST_F(Hyperdimensional4DTest, Projection_4Dto3D)
{
    Vec4D v4d(1.0f, 2.0f, 3.0f, 4.0f);

    // Project to 3D with camera distance 5
    glm::vec3 v3d = Project4Dto3D(v4d, 5.0f);

    // Perspective projection: (x,y,z,w) -> (x/(1-w/d), y/(1-w/d), z/(1-w/d))
    // With d=5, w=4: scale = 1/(1-4/5) = 1/0.2 = 5
    EXPECT_NEAR(v3d.x, 5.0f, EPSILON);   // 1 * 5
    EXPECT_NEAR(v3d.y, 10.0f, EPSILON);  // 2 * 5
    EXPECT_NEAR(v3d.z, 15.0f, EPSILON);  // 3 * 5
}

TEST_F(Hyperdimensional4DTest, Projection_FullPipeline)
{
    // Create a 4D point
    Vec4D v4d(1.0f, 1.0f, 1.0f, 0.0f);

    // Project to 3D
    glm::vec3 v3d = Project4Dto3D(v4d, 5.0f);

    // Project to 2D (screen space)
    glm::vec2 v2d = Project3Dto2D(v3d, glm::perspective(
        std::numbers::pi_v<float> / 4.0f,  // FOV
        16.0f / 9.0f,                       // Aspect
        0.1f,                               // Near
        100.0f),                            // Far
        1920, 1080);

    // Should produce valid screen coordinates
    EXPECT_FALSE(std::isnan(v2d.x));
    EXPECT_FALSE(std::isnan(v2d.y));
}

TEST_F(Hyperdimensional4DTest, Projection_TesseractToScreen)
{
    Tesseract tesseract(1.0f);

    // Rotate it
    tesseract.Rotate(0.5f, 0.3f);

    // Project all vertices to screen
    auto projected = ProjectTesseractTo2D(tesseract,
                                         5.0f,  // 4D->3D distance
                                         glm::perspective(std::numbers::pi_v<float> / 4.0f, 16.0f/9.0f, 0.1f, 100.0f),
                                         1920, 1080);

    // Should have 16 projected vertices
    EXPECT_EQ(projected.size(), 16);

    // All should be valid (not NaN)
    for (const auto& point : projected)
    {
        EXPECT_FALSE(std::isnan(point.x));
        EXPECT_FALSE(std::isnan(point.y));
    }
}

// ====== Audio-Reactive 4D Rotation Tests ======

TEST_F(Hyperdimensional4DTest, AudioReactive_BPMRotation)
{
    Tesseract tesseract(1.0f);

    float bpm = 120.0f;
    float beatPhase = 0.5f; // Halfway through beat

    // Rotate based on BPM and beat phase
    float angleXY = beatPhase * 2.0f * std::numbers::pi_v<float>;
    float angleZW = (bpm / 60.0f) * 0.1f; // Slower ZW rotation

    auto verticesBefore = tesseract.GetVertices();

    tesseract.Rotate(angleXY, angleZW);

    auto verticesAfter = tesseract.GetVertices();

    // Should have rotated
    EXPECT_NE(verticesBefore[0].x, verticesAfter[0].x);
}

TEST_F(Hyperdimensional4DTest, AudioReactive_EnergyScale)
{
    Tesseract tesseract(1.0f);

    float energy = 0.8f; // 80% energy

    // Scale tesseract based on energy
    tesseract.SetScale(1.0f + energy * 0.5f); // Scale 1.0 to 1.5

    auto vertices = tesseract.GetVertices();

    // Vertices should be farther from origin
    for (const auto& vertex : vertices)
    {
        float distance = vertex.Length();
        EXPECT_GT(distance, 2.0f); // Scaled up from base distance of 2.0
    }
}
