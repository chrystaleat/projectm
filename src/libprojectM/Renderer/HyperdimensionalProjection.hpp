/**
 * @file HyperdimensionalProjection.hpp
 * @brief Projection pipeline from 4D to 3D to 2D
 */

#pragma once

#include "Tesseract.hpp"
#include "Vec4D.hpp"

#include <projectM-4/projectM_export.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

namespace libprojectM {
namespace Renderer {

/**
 * @brief Project 4D point to 3D using perspective projection
 *
 * Uses perspective projection based on the w coordinate, similar to how
 * 3D->2D projection uses the z coordinate.
 *
 * The projection formula is:
 *   scale = 1 / (1 - w/distance)
 *   (x, y, z, w) -> (x*scale, y*scale, z*scale)
 *
 * Points with w close to the camera distance will appear larger (closer in 4D),
 * while points with w far from the camera will appear smaller (farther in 4D).
 *
 * @param point4D The 4D point to project
 * @param cameraDistance4D Distance of 4D camera from origin (typically 5-10)
 * @return Projected 3D point
 */
inline glm::vec3 Project4Dto3D(const Vec4D& point4D, float cameraDistance4D)
{
    // Perspective projection: objects closer in 4D (larger w) appear larger
    float scale = 1.0f / (1.0f - point4D.w / cameraDistance4D);

    return glm::vec3(
        point4D.x * scale,
        point4D.y * scale,
        point4D.z * scale
    );
}

/**
 * @brief Project 3D point to 2D screen coordinates
 *
 * Uses standard OpenGL perspective projection and viewport transformation.
 *
 * @param point3D The 3D point to project
 * @param projectionMatrix Perspective projection matrix
 * @param screenWidth Screen width in pixels
 * @param screenHeight Screen height in pixels
 * @return Screen coordinates (x, y)
 */
inline glm::vec2 Project3Dto2D(const glm::vec3& point3D,
                               const glm::mat4& projectionMatrix,
                               int screenWidth,
                               int screenHeight)
{
    // Apply projection matrix
    glm::vec4 projected = projectionMatrix * glm::vec4(point3D, 1.0f);

    // Perspective divide
    if (std::abs(projected.w) > 1e-6f)
    {
        projected /= projected.w;
    }

    // NDC to screen coordinates
    // NDC is [-1, 1], screen is [0, width] and [0, height]
    float screenX = (projected.x + 1.0f) * 0.5f * static_cast<float>(screenWidth);
    float screenY = (1.0f - projected.y) * 0.5f * static_cast<float>(screenHeight); // Flip Y

    return glm::vec2(screenX, screenY);
}

/**
 * @brief Project entire tesseract to 2D screen coordinates
 *
 * Performs the full projection pipeline:
 *   4D (tesseract vertices) -> 3D (perspective) -> 2D (screen)
 *
 * This allows rendering of 4D geometry on a 2D screen.
 *
 * @param tesseract The tesseract to project
 * @param cameraDistance4D Distance of 4D camera
 * @param projectionMatrix 3D perspective projection matrix
 * @param screenWidth Screen width in pixels
 * @param screenHeight Screen height in pixels
 * @return Vector of 2D screen coordinates (one per vertex)
 */
inline std::vector<glm::vec2> ProjectTesseractTo2D(const Tesseract& tesseract,
                                                   float cameraDistance4D,
                                                   const glm::mat4& projectionMatrix,
                                                   int screenWidth,
                                                   int screenHeight)
{
    const auto& vertices4D = tesseract.GetVertices();
    std::vector<glm::vec2> screenPoints;
    screenPoints.reserve(vertices4D.size());

    for (const auto& vertex4D : vertices4D)
    {
        // 4D -> 3D
        glm::vec3 vertex3D = Project4Dto3D(vertex4D, cameraDistance4D);

        // 3D -> 2D
        glm::vec2 screenPoint = Project3Dto2D(vertex3D, projectionMatrix, screenWidth, screenHeight);

        screenPoints.push_back(screenPoint);
    }

    return screenPoints;
}

/**
 * @brief Create a view matrix for 3D camera
 *
 * Helper function to create a view matrix positioned to view the projected tesseract.
 *
 * @param cameraPos Camera position
 * @param lookAt Look-at point
 * @param up Up vector
 * @return View matrix
 */
inline glm::mat4 CreateViewMatrix(const glm::vec3& cameraPos,
                                  const glm::vec3& lookAt,
                                  const glm::vec3& up)
{
    return glm::lookAt(cameraPos, lookAt, up);
}

} // namespace Renderer
} // namespace libprojectM
