/**
 * @file AudioFeatures.hpp
 * @brief Advanced audio feature extraction for intelligent visualization
 *
 * Provides high-level audio analysis beyond basic FFT:
 * - BPM (tempo) detection with confidence scoring
 * - Musical key and scale detection
 * - Energy/excitement profiling
 * - Spectral characteristics (brightness, flatness, rolloff)
 * - Onset detection for transient events
 *
 * These features enable:
 * - Intelligent preset selection
 * - Adaptive visualization parameters
 * - Music-aware effects
 * - Metadata-driven visuals
 */

#pragma once

#include "AudioConstants.hpp"
#include "FrameAudioData.hpp"

#include <projectM-4/projectM_export.h>

#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace libprojectM {
namespace Audio {

/**
 * @brief Musical key representation
 */
enum class MusicalKey : uint8_t
{
    C = 0,
    CSharp = 1,
    D = 2,
    DSharp = 3,
    E = 4,
    F = 5,
    FSharp = 6,
    G = 7,
    GSharp = 8,
    A = 9,
    ASharp = 10,
    B = 11,
    Unknown = 255
};

/**
 * @brief Scale/mode type
 */
enum class ScaleType : uint8_t
{
    Major,
    Minor,
    Unknown
};

/**
 * @brief Advanced audio features extracted from audio analysis
 *
 * Updated once per frame, provides high-level musical and perceptual
 * characteristics of the audio signal.
 */
class PROJECTM_EXPORT AudioFeatures
{
public:
    // ====== BPM & Timing ======

    /**
     * @brief Detected tempo in beats per minute
     * Range: 40.0 - 200.0 BPM (typical music range)
     */
    float bpm{120.0f};

    /**
     * @brief Confidence of BPM detection (0.0 - 1.0)
     *
     * Low confidence (<0.5) indicates unreliable BPM,
     * High confidence (>0.8) indicates stable tempo lock
     */
    float bpmConfidence{0.0f};

    /**
     * @brief Beat phase (0.0 - 1.0)
     *
     * Position within current beat:
     * 0.0 = beat just occurred
     * 0.5 = halfway between beats
     * 1.0 = about to hit next beat
     */
    float beatPhase{0.0f};

    /**
     * @brief Bar phase (0.0 - 1.0)
     *
     * Position within current 4-beat bar (assuming 4/4 time)
     */
    float barPhase{0.0f};

    // ====== Key & Harmony ======

    /**
     * @brief Detected musical key
     */
    MusicalKey key{MusicalKey::Unknown};

    /**
     * @brief Detected scale/mode (major vs minor)
     */
    ScaleType scale{ScaleType::Unknown};

    /**
     * @brief Confidence of key detection (0.0 - 1.0)
     */
    float keyConfidence{0.0f};

    // ====== Energy & Dynamics ======

    /**
     * @brief Overall energy level (0.0 - 1.0)
     *
     * Combines RMS energy with spectral flux.
     * Indicates overall "intensity" of the music.
     */
    float energy{0.0f};

    /**
     * @brief Smoothed energy with attack/release envelope
     *
     * Slower-moving version of energy for smooth transitions
     */
    float energySmoothed{0.0f};

    /**
     * @brief Excitement/aggression metric (0.0 - 1.0)
     *
     * High for metal/EDM, low for ambient/classical
     * Based on spectral centroid, flux, and tempo
     */
    float excitement{0.0f};

    /**
     * @brief Calmness metric (0.0 - 1.0)
     *
     * Inverse of excitement, represents peaceful/meditative qualities
     */
    float calmness{0.0f};

    // ====== Spectral Characteristics ======

    /**
     * @brief Spectral centroid (0.0 - 1.0)
     *
     * "Brightness" of sound:
     * 0.0 = bass-heavy
     * 1.0 = treble-heavy
     */
    float spectralCentroid{0.5f};

    /**
     * @brief Spectral flatness (0.0 - 1.0)
     *
     * Measure of noisiness vs tonality:
     * 0.0 = pure tones (melody)
     * 1.0 = white noise (percussion)
     */
    float spectralFlatness{0.5f};

    /**
     * @brief Spectral rolloff frequency (0.0 - 1.0)
     *
     * Frequency below which 85% of energy is contained.
     * Indicator of bass vs treble emphasis.
     */
    float spectralRolloff{0.5f};

    /**
     * @brief Spectral flux (rate of spectral change)
     *
     * High during transients and dramatic changes.
     * Used for onset detection.
     */
    float spectralFlux{0.0f};

    // ====== Onset Detection ======

    /**
     * @brief Onset detected this frame (kick, snare, transient)
     */
    bool onsetDetected{false};

    /**
     * @brief Onset strength (0.0 - 1.0)
     *
     * How strong the detected onset was
     */
    float onsetStrength{0.0f};

    // ====== Perceptual Qualities ======

    /**
     * @brief Perceived loudness (LUFS-inspired)
     *
     * Perceptually-weighted loudness measurement
     */
    float loudness{0.0f};

    /**
     * @brief Dynamic range (0.0 - 1.0)
     *
     * Difference between loud and quiet parts:
     * 0.0 = heavily compressed (EDM, modern pop)
     * 1.0 = high dynamic range (classical, jazz)
     */
    float dynamicRange{0.5f};

    // ====== Audio Fingerprint ======

    /**
     * @brief Short-term audio fingerprint hash
     *
     * Unique identifier for current audio segment,
     * allows for reproducible visualizations
     */
    uint64_t audioFingerprint{0};

    /**
     * @brief Long-term audio signature
     *
     * 32-element feature vector representing overall
     * spectral characteristics over several seconds
     */
    std::array<float, 32> audioSignature{};

    // ====== Genre Classification Hints ======

    /**
     * @brief Probability distribution over genre categories
     *
     * Simple heuristic-based genre hints:
     * [0] = Electronic/EDM
     * [1] = Rock/Metal
     * [2] = Hip-Hop/Rap
     * [3] = Classical/Orchestral
     * [4] = Jazz/Blues
     * [5] = Ambient/Drone
     */
    std::array<float, 6> genreHints{};
};

} // namespace Audio
} // namespace libprojectM
