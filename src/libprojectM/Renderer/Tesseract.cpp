/**
 * @file Tesseract.cpp
 * @brief Implementation of 4D hypercube
 */

#include "Tesseract.hpp"

namespace libprojectM {
namespace Renderer {

Tesseract::Tesseract(float sideLength)
    : m_sideLength(sideLength)
{
    GenerateVertices();
    GenerateEdges();

    // Store base configuration
    m_baseVertices = m_vertices;
}

void Tesseract::GenerateVertices()
{
    m_vertices.clear();
    m_vertices.reserve(16);

    // A tesseract has 16 vertices at all combinations of (±1, ±1, ±1, ±1)
    // scaled by sideLength/2
    float half = m_sideLength / 2.0f;

    for (int i = 0; i < 16; ++i)
    {
        // Use bit pattern to generate all combinations
        float x = (i & 1) ? half : -half;
        float y = (i & 2) ? half : -half;
        float z = (i & 4) ? half : -half;
        float w = (i & 8) ? half : -half;

        m_vertices.emplace_back(x, y, z, w);
    }
}

void Tesseract::GenerateEdges()
{
    m_edges.clear();
    m_edges.reserve(32);

    // In a tesseract, two vertices are connected by an edge if they differ
    // in exactly one coordinate. Each vertex has 4 edges (one per dimension).
    // Total edges = 16 vertices * 4 edges / 2 (each edge counted twice) = 32

    for (size_t i = 0; i < 16; ++i)
    {
        // Connect to vertices that differ by one bit (one coordinate flip)
        for (int dim = 0; dim < 4; ++dim)
        {
            size_t j = i ^ (1 << dim); // Flip bit in dimension 'dim'

            // Only add edge once (avoid duplicates)
            if (i < j)
            {
                m_edges.emplace_back(i, j);
            }
        }
    }
}

void Tesseract::Rotate(float angleXY, float angleZW)
{
    // Rotate all vertices using the base configuration
    for (size_t i = 0; i < m_vertices.size(); ++i)
    {
        m_vertices[i] = Rotate4D_Double(m_baseVertices[i], angleXY, angleZW);

        // Apply scale
        m_vertices[i] = m_vertices[i] * m_scale;
    }
}

void Tesseract::SetScale(float scale)
{
    m_scale = scale;

    // Re-apply current rotation with new scale
    // (assumes rotation is cumulative from base)
    for (size_t i = 0; i < m_vertices.size(); ++i)
    {
        // Scale from base vertices
        m_vertices[i] = m_baseVertices[i] * m_scale;
    }
}

void Tesseract::Reset()
{
    m_vertices = m_baseVertices;
    m_scale = 1.0f;
}

} // namespace Renderer
} // namespace libprojectM
