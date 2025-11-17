/**
 * @file Hyperdimensional4DSettings.hpp
 * @brief 4D tesseract visualization settings
 */

#pragma once

#include <projectM-4/projectM_export.h>

namespace libprojectM {
namespace Settings {

/**
 * @brief Rendering mode for 4D visualization
 */
enum class Render4DMode
{
    Wireframe,     //!< Wireframe (all 32 edges)
    Vertices,      //!< Vertex points only
    Hybrid,        //!< Wireframe + glowing vertices
    Faces          //!< Filled faces
};

/**
 * @brief 4D hyperdimensional visualization settings
 */
struct PROJECTM_EXPORT Hyperdimensional4DSettings
{
    // Enable/Disable
    bool enabled{true};                  //!< Enable 4D visualization

    // Tesseract Properties
    float tesseractSize{2.0f};           //!< Size of tesseract
    float cameraDistance4D{8.0f};        //!< 4D camera distance

    // Rotation
    float rotationSpeedXY{0.02f};        //!< Rotation speed in XY plane
    float rotationSpeedZW{0.014f};       //!< Rotation speed in ZW plane
    bool enableXYRotation{true};         //!< Enable XY rotation
    bool enableZWRotation{true};         //!< Enable ZW rotation

    // Audio Reactivity
    bool audioReactiveRotation{true};    //!< Modulate rotation with BPM
    bool audioReactiveScale{true};       //!< Scale with energy

    // Rendering
    Render4DMode renderMode{Render4DMode::Hybrid};  //!< Rendering mode
    float edgeThickness{2.0f};           //!< Edge line thickness
    float vertexSize{4.0f};              //!< Vertex point size
    bool depthFading{true};              //!< Fade based on 4D depth

    // Color
    bool colorByWAxis{true};             //!< Color by W-axis position
    bool colorByDepth{false};            //!< Color by depth from camera

    /**
     * @brief Reset to default values
     */
    void ResetToDefaults()
    {
        *this = Hyperdimensional4DSettings{};
    }

    /**
     * @brief Validate and clamp values
     */
    void Validate()
    {
        // Clamp size
        if (tesseractSize < 0.5f) tesseractSize = 0.5f;
        if (tesseractSize > 10.0f) tesseractSize = 10.0f;

        // Clamp camera distance
        if (cameraDistance4D < 2.0f) cameraDistance4D = 2.0f;
        if (cameraDistance4D > 50.0f) cameraDistance4D = 50.0f;

        // Clamp rotation speeds
        if (rotationSpeedXY < 0.0f) rotationSpeedXY = 0.0f;
        if (rotationSpeedXY > 0.5f) rotationSpeedXY = 0.5f;

        if (rotationSpeedZW < 0.0f) rotationSpeedZW = 0.0f;
        if (rotationSpeedZW > 0.5f) rotationSpeedZW = 0.5f;

        // Clamp rendering parameters
        if (edgeThickness < 0.5f) edgeThickness = 0.5f;
        if (edgeThickness > 10.0f) edgeThickness = 10.0f;

        if (vertexSize < 1.0f) vertexSize = 1.0f;
        if (vertexSize > 20.0f) vertexSize = 20.0f;
    }
};

} // namespace Settings
} // namespace libprojectM
