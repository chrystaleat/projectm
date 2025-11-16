/**
 * @file Vec4D.hpp
 * @brief 4D vector mathematics for hyperdimensional visualization
 */

#pragma once

#include <projectM-4/projectM_export.h>

#include <cmath>

namespace libprojectM {
namespace Renderer {

/**
 * @brief 4-dimensional vector
 *
 * Represents a point or direction in 4D space (x, y, z, w).
 * Used for hyperdimensional visualization and 4D geometric transformations.
 *
 * In 4D visualization:
 * - x, y, z: Standard 3D spatial coordinates
 * - w: Fourth spatial dimension (hyperspace)
 */
class PROJECTM_EXPORT Vec4D
{
public:
    /**
     * @brief Default constructor - creates zero vector
     */
    Vec4D()
        : x(0.0f)
        , y(0.0f)
        , z(0.0f)
        , w(0.0f)
    {
    }

    /**
     * @brief Construct from components
     */
    Vec4D(float x, float y, float z, float w)
        : x(x)
        , y(y)
        , z(z)
        , w(w)
    {
    }

    /**
     * @brief Vector addition
     */
    Vec4D operator+(const Vec4D& other) const
    {
        return Vec4D(x + other.x, y + other.y, z + other.z, w + other.w);
    }

    /**
     * @brief Vector subtraction
     */
    Vec4D operator-(const Vec4D& other) const
    {
        return Vec4D(x - other.x, y - other.y, z - other.z, w - other.w);
    }

    /**
     * @brief Scalar multiplication
     */
    Vec4D operator*(float scalar) const
    {
        return Vec4D(x * scalar, y * scalar, z * scalar, w * scalar);
    }

    /**
     * @brief Scalar division
     */
    Vec4D operator/(float scalar) const
    {
        return Vec4D(x / scalar, y / scalar, z / scalar, w / scalar);
    }

    /**
     * @brief Dot product (4D inner product)
     *
     * @param other The other vector
     * @return Scalar result of dot product
     */
    float Dot(const Vec4D& other) const
    {
        return x * other.x + y * other.y + z * other.z + w * other.w;
    }

    /**
     * @brief Calculate 4D length (magnitude)
     *
     * @return Length of the vector
     */
    float Length() const
    {
        return std::sqrt(x * x + y * y + z * z + w * w);
    }

    /**
     * @brief Calculate squared length (avoids sqrt)
     *
     * @return Squared length
     */
    float LengthSquared() const
    {
        return x * x + y * y + z * z + w * w;
    }

    /**
     * @brief Return normalized vector (length = 1)
     *
     * @return Unit vector in same direction
     */
    Vec4D Normalized() const
    {
        float len = Length();
        if (len < 1e-6f)
        {
            return Vec4D(0.0f, 0.0f, 0.0f, 0.0f);
        }
        return *this / len;
    }

    /**
     * @brief Normalize this vector in place
     */
    void Normalize()
    {
        float len = Length();
        if (len >= 1e-6f)
        {
            x /= len;
            y /= len;
            z /= len;
            w /= len;
        }
    }

    // Components
    float x;
    float y;
    float z;
    float w;
};

/**
 * @brief Rotate 4D vector in XY plane
 *
 * This rotation affects x and y coordinates while leaving z and w unchanged.
 * Similar to 2D rotation in the XY plane of 3D space.
 *
 * @param v Vector to rotate
 * @param angle Rotation angle in radians
 * @return Rotated vector
 */
inline Vec4D Rotate4D_XY(const Vec4D& v, float angle)
{
    float c = std::cos(angle);
    float s = std::sin(angle);

    return Vec4D(
        v.x * c - v.y * s,  // x' = x*cos - y*sin
        v.x * s + v.y * c,  // y' = x*sin + y*cos
        v.z,                 // z unchanged
        v.w                  // w unchanged
    );
}

/**
 * @brief Rotate 4D vector in ZW plane
 *
 * This rotation affects z and w coordinates while leaving x and y unchanged.
 * This is a truly 4D rotation that doesn't exist in 3D space.
 *
 * @param v Vector to rotate
 * @param angle Rotation angle in radians
 * @return Rotated vector
 */
inline Vec4D Rotate4D_ZW(const Vec4D& v, float angle)
{
    float c = std::cos(angle);
    float s = std::sin(angle);

    return Vec4D(
        v.x,                 // x unchanged
        v.y,                 // y unchanged
        v.z * c - v.w * s,  // z' = z*cos - w*sin
        v.z * s + v.w * c   // w' = z*sin + w*cos
    );
}

/**
 * @brief Rotate 4D vector in XZ plane
 *
 * @param v Vector to rotate
 * @param angle Rotation angle in radians
 * @return Rotated vector
 */
inline Vec4D Rotate4D_XZ(const Vec4D& v, float angle)
{
    float c = std::cos(angle);
    float s = std::sin(angle);

    return Vec4D(
        v.x * c - v.z * s,
        v.y,
        v.x * s + v.z * c,
        v.w
    );
}

/**
 * @brief Rotate 4D vector in YW plane
 *
 * @param v Vector to rotate
 * @param angle Rotation angle in radians
 * @return Rotated vector
 */
inline Vec4D Rotate4D_YW(const Vec4D& v, float angle)
{
    float c = std::cos(angle);
    float s = std::sin(angle);

    return Vec4D(
        v.x,
        v.y * c - v.w * s,
        v.z,
        v.y * s + v.w * c
    );
}

/**
 * @brief Rotate 4D vector in XW plane
 *
 * @param v Vector to rotate
 * @param angle Rotation angle in radians
 * @return Rotated vector
 */
inline Vec4D Rotate4D_XW(const Vec4D& v, float angle)
{
    float c = std::cos(angle);
    float s = std::sin(angle);

    return Vec4D(
        v.x * c - v.w * s,
        v.y,
        v.z,
        v.x * s + v.w * c
    );
}

/**
 * @brief Rotate 4D vector in YZ plane
 *
 * @param v Vector to rotate
 * @param angle Rotation angle in radians
 * @return Rotated vector
 */
inline Vec4D Rotate4D_YZ(const Vec4D& v, float angle)
{
    float c = std::cos(angle);
    float s = std::sin(angle);

    return Vec4D(
        v.x,
        v.y * c - v.z * s,
        v.y * s + v.z * c,
        v.w
    );
}

/**
 * @brief Double rotation: Simultaneously rotate in XY and ZW planes
 *
 * This creates a true 4D rotation that combines two orthogonal plane rotations.
 * This is one of the characteristic rotations of 4D space.
 *
 * @param v Vector to rotate
 * @param angleXY Rotation angle in XY plane (radians)
 * @param angleZW Rotation angle in ZW plane (radians)
 * @return Rotated vector
 */
inline Vec4D Rotate4D_Double(const Vec4D& v, float angleXY, float angleZW)
{
    // First rotate in XY plane
    Vec4D temp = Rotate4D_XY(v, angleXY);

    // Then rotate in ZW plane
    return Rotate4D_ZW(temp, angleZW);
}

} // namespace Renderer
} // namespace libprojectM
