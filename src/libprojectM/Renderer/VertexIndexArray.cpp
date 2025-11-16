#include "VertexIndexArray.hpp"

#include <limits>

namespace libprojectM {
namespace Renderer {

VertexIndexArray::VertexIndexArray()
{
    glGenBuffers(1, &m_veabID);
}

VertexIndexArray::VertexIndexArray(VertexBufferUsage usage)
    : m_vboUsage(usage)
{
    glGenBuffers(1, &m_veabID);
}

VertexIndexArray::~VertexIndexArray()
{
    glDeleteBuffers(1, &m_veabID);
}

void VertexIndexArray::Bind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_veabID);
}

void VertexIndexArray::Unbind()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

auto VertexIndexArray::Get() -> std::vector<uint32_t>&
{
    return m_indices;
}

auto VertexIndexArray::Get() const -> const std::vector<uint32_t>&
{
    return m_indices;
}

void VertexIndexArray::Set(const std::vector<uint32_t>& buffer)
{
    m_indices = buffer;
}

auto VertexIndexArray::VertexIndex(size_t index) -> uint32_t
{
    return m_indices.at(index);
}

auto VertexIndexArray::VertexIndex(size_t index) const -> uint32_t
{
    return m_indices.at(index);
}

void VertexIndexArray::SetVertexIndex(size_t index, uint32_t value)
{
    m_indices.at(index) = value;
}

auto VertexIndexArray::Empty() const -> bool
{
    return m_indices.empty();
}

auto VertexIndexArray::Size() const -> size_t
{
    return m_indices.size();
}

void VertexIndexArray::Resize(size_t size)
{
    m_indices.resize(size);
}

void VertexIndexArray::Resize(size_t size, uint32_t value)
{
    m_indices.resize(size, value);
}

void VertexIndexArray::MakeContinuous()
{
    for (size_t index = 0; index < m_indices.size(); index++)
    {
        m_indices.at(index) = index;
    }
}

void VertexIndexArray::Update()
{
    Bind();

    if (m_indices.empty())
    {
        return;
    }

    // SECURITY FIX (HIGH-006): Check for integer overflow in size calculation
    size_t indexCount = m_indices.size();
    size_t bytesPerIndex = sizeof(uint32_t);
    size_t totalBytes = indexCount * bytesPerIndex;

    // Check for overflow: if multiplication overflowed, division won't equal original
    if (indexCount > 0 && totalBytes / indexCount != bytesPerIndex)
    {
        // Overflow detected - buffer too large
        return;
    }

    // Also check that result fits in GLsizei (typically int32_t)
    if (totalBytes > static_cast<size_t>(std::numeric_limits<GLsizei>::max()))
    {
        // Too large for OpenGL
        return;
    }

    GLsizei bufferSize = static_cast<GLsizei>(totalBytes);

    if (m_veabSize == indexCount)
    {
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, bufferSize, m_indices.data());
    }
    else
    {
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, bufferSize, m_indices.data(), VertexBufferUsageToGL(m_vboUsage));
        m_veabSize = indexCount;
    }
}

auto VertexIndexArray::operator[](size_t index) -> uint32_t&
{
    return m_indices.at(index);
}

auto VertexIndexArray::operator[](size_t index) const -> uint32_t
{
    return m_indices.at(index);
}

} // namespace Renderer
} // namespace libprojectM
