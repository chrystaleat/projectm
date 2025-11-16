/**
 * @file AudioFeatureExtractor.hpp
 * @brief Extracts advanced audio features for intelligent visualization
 */

#pragma once

#include "AudioFeatures.hpp"
#include "FrameAudioData.hpp"

#include <projectM-4/projectM_export.h>

#include <array>
#include <deque>
#include <vector>

namespace libprojectM {
namespace Audio {

/**
 * @brief Extracts high-level audio features from frame audio data
 *
 * This class analyzes audio data over time to extract perceptual and musical
 * features that go beyond basic FFT and beat detection. Features include:
 * - BPM (tempo) detection
 * - Musical key and scale
 * - Energy and dynamic characteristics
 * - Spectral properties
 * - Onset detection
 *
 * Usage:
 * @code
 * AudioFeatureExtractor extractor;
 * AudioFeatures features;
 *
 * // Each frame:
 * extractor.UpdateFeatures(frameAudioData, features, deltaTime);
 * // features now contains extracted features
 * @endcode
 */
class PROJECTM_EXPORT AudioFeatureExtractor
{
public:
    AudioFeatureExtractor();
    ~AudioFeatureExtractor() = default;

    /**
     * @brief Update features based on current frame audio data
     *
     * Call this once per frame to update the feature analysis.
     *
     * @param audioData Current frame's audio data (spectrum, waveform, beats)
     * @param features Output features structure to update
     * @param deltaTime Time since last frame in seconds (e.g., 1/60 for 60 FPS)
     */
    void UpdateFeatures(const FrameAudioData& audioData,
                       AudioFeatures& features,
                       float deltaTime);

    /**
     * @brief Reset all temporal state
     *
     * Clears history buffers and resets confidence scores.
     * Useful when switching tracks or after long pauses.
     */
    void Reset();

private:
    // ====== BPM Detection ======

    /**
     * @brief Detect tempo using onset-based autocorrelation
     */
    void DetectBPM(const FrameAudioData& audioData, AudioFeatures& features, float deltaTime);

    /**
     * @brief Update beat phase based on detected BPM
     */
    void UpdateBeatPhase(AudioFeatures& features, float deltaTime);

    // ====== Spectral Analysis ======

    /**
     * @brief Calculate spectral centroid (brightness)
     */
    float CalculateSpectralCentroid(const std::array<float, SpectrumSamples>& spectrum);

    /**
     * @brief Calculate spectral flatness (tonality vs noise)
     */
    float CalculateSpectralFlatness(const std::array<float, SpectrumSamples>& spectrum);

    /**
     * @brief Calculate spectral rolloff (85% energy point)
     */
    float CalculateSpectralRolloff(const std::array<float, SpectrumSamples>& spectrum);

    /**
     * @brief Calculate spectral flux (rate of change)
     */
    float CalculateSpectralFlux(const std::array<float, SpectrumSamples>& spectrum);

    // ====== Energy Analysis ======

    /**
     * @brief Calculate overall energy from audio data
     */
    void CalculateEnergy(const FrameAudioData& audioData, AudioFeatures& features);

    /**
     * @brief Update smoothed energy with attack/release envelope
     */
    void UpdateSmoothedEnergy(AudioFeatures& features, float deltaTime);

    // ====== Onset Detection ======

    /**
     * @brief Detect note onsets (kicks, snares, transients)
     */
    void DetectOnsets(const FrameAudioData& audioData, AudioFeatures& features);

    // ====== Perceptual Features ======

    /**
     * @brief Calculate excitement/calmness metrics
     */
    void CalculateEmotionalFeatures(AudioFeatures& features);

    /**
     * @brief Update dynamic range estimate
     */
    void UpdateDynamicRange(const FrameAudioData& audioData, AudioFeatures& features);

    // ====== Genre Hints ======

    /**
     * @brief Calculate simple heuristic-based genre probabilities
     */
    void CalculateGenreHints(const AudioFeatures& features, AudioFeatures& output);

    // ====== History Buffers ======

    std::deque<float> m_onsetHistory;              //!< Onset strength history for BPM detection
    std::deque<float> m_energyHistory;             //!< Energy history for dynamic range
    std::array<float, SpectrumSamples> m_prevSpectrum{}; //!< Previous frame spectrum for flux

    // ====== BPM State ======

    float m_beatPhaseAccumulator{0.0f};            //!< Accumulated beat phase
    float m_bpmSmoothingFactor{0.95f};             //!< BPM smoothing (higher = more stable)
    std::vector<float> m_bpmCandidates;            //!< Recent BPM estimates for voting
    float m_lastOnsetTime{0.0f};                   //!< Time of last detected onset
    std::deque<float> m_interBeatIntervals;        //!< Inter-beat interval history

    // ====== Energy Envelope State ======

    float m_energyAttackTime{0.01f};               //!< Energy attack time (seconds)
    float m_energyReleaseTime{0.3f};               //!< Energy release time (seconds)

    // ====== Onset Detection State ======

    float m_onsetThreshold{0.3f};                  //!< Minimum onset strength to trigger
    float m_prevOnsetFunction{0.0f};               //!< Previous onset detection function value

    // ====== Dynamic Range State ======

    float m_peakLevel{0.0f};                       //!< Recent peak level
    float m_rmsLevel{0.0f};                        //!< Recent RMS level

    // ====== Constants ======

    static constexpr size_t MaxOnsetHistory = 300;  //!< ~5 seconds at 60 FPS
    static constexpr size_t MaxEnergyHistory = 600; //!< ~10 seconds at 60 FPS
    static constexpr size_t MaxBPMCandidates = 100; //!< BPM vote history
    static constexpr size_t MaxInterBeatIntervals = 16; //!< Recent beat intervals
};

} // namespace Audio
} // namespace libprojectM
