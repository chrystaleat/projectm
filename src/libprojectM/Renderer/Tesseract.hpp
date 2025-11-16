/**
 * @file Tesseract.hpp
 * @brief 4D hypercube (tesseract) for hyperdimensional visualization
 */

#pragma once

#include "Vec4D.hpp"

#include <projectM-4/projectM_export.h>

#include <array>
#include <utility>
#include <vector>

namespace libprojectM {
namespace Renderer {

/**
 * @brief 4D hypercube (tesseract)
 *
 * A tesseract is the 4D analog of a cube. It has:
 * - 16 vertices (2^4)
 * - 32 edges (4 * 2^3)
 * - 24 square faces (6 * 2^2)
 * - 8 cubic cells (2^3)
 *
 * This class generates and manipulates tesseract geometry for visualization.
 * The tesseract can be rotated in 4D space and then projected to lower dimensions.
 *
 * Usage:
 * @code
 * Tesseract tesseract(2.0f); // Side length 2
 * tesseract.Rotate(0.1f, 0.2f); // Rotate in XY and ZW planes
 * auto vertices = tesseract.GetVertices(); // Get current vertex positions
 * @endcode
 */
class PROJECTM_EXPORT Tesseract
{
public:
    /**
     * @brief Construct a tesseract
     *
     * @param sideLength Length of each edge (default 1.0)
     */
    explicit Tesseract(float sideLength = 1.0f);

    /**
     * @brief Get current vertex positions
     *
     * @return Vector of 16 vertices in 4D space
     */
    const std::vector<Vec4D>& GetVertices() const { return m_vertices; }

    /**
     * @brief Get edge connections
     *
     * Each edge is a pair of vertex indices.
     *
     * @return Vector of 32 edges (pairs of indices)
     */
    const std::vector<std::pair<size_t, size_t>>& GetEdges() const { return m_edges; }

    /**
     * @brief Rotate tesseract in 4D space
     *
     * Applies rotation to all vertices.
     *
     * @param angleXY Rotation angle in XY plane (radians)
     * @param angleZW Rotation angle in ZW plane (radians)
     */
    void Rotate(float angleXY, float angleZW);

    /**
     * @brief Set uniform scale factor
     *
     * Scales all vertices relative to origin.
     *
     * @param scale Scale multiplier (1.0 = normal size)
     */
    void SetScale(float scale);

    /**
     * @brief Reset to initial configuration
     */
    void Reset();

private:
    /**
     * @brief Generate initial vertex positions
     */
    void GenerateVertices();

    /**
     * @brief Generate edge connections
     */
    void GenerateEdges();

    float m_sideLength;                                  //!< Edge length
    float m_scale{1.0f};                                 //!< Current scale factor
    std::vector<Vec4D> m_vertices;                       //!< 16 vertices
    std::vector<Vec4D> m_baseVertices;                   //!< Original unrotated vertices
    std::vector<std::pair<size_t, size_t>> m_edges;      //!< 32 edge connections
};

} // namespace Renderer
} // namespace libprojectM
