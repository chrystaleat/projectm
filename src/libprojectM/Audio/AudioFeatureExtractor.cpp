/**
 * @file AudioFeatureExtractor.cpp
 * @brief Implementation of advanced audio feature extraction
 */

#include "AudioFeatureExtractor.hpp"

#include <algorithm>
#include <cmath>
#include <numeric>

namespace libprojectM {
namespace Audio {

AudioFeatureExtractor::AudioFeatureExtractor()
{
    m_prevSpectrum.fill(0.0f);
    m_bpmCandidates.reserve(MaxBPMCandidates);
}

void AudioFeatureExtractor::UpdateFeatures(const FrameAudioData& audioData,
                                          AudioFeatures& features,
                                          float deltaTime)
{
    // 1. Calculate spectral features
    features.spectralCentroid = CalculateSpectralCentroid(audioData.spectrumLeft);
    features.spectralFlatness = CalculateSpectralFlatness(audioData.spectrumLeft);
    features.spectralRolloff = CalculateSpectralRolloff(audioData.spectrumLeft);
    features.spectralFlux = CalculateSpectralFlux(audioData.spectrumLeft);

    // 2. Calculate energy
    CalculateEnergy(audioData, features);
    UpdateSmoothedEnergy(features, deltaTime);

    // 3. Detect onsets
    DetectOnsets(audioData, features);

    // 4. Update BPM detection
    DetectBPM(audioData, features, deltaTime);
    UpdateBeatPhase(features, deltaTime);

    // 5. Calculate perceptual features
    CalculateEmotionalFeatures(features);
    UpdateDynamicRange(audioData, features);

    // 6. Genre hints
    CalculateGenreHints(features, features);

    // Store spectrum for next frame's flux calculation
    m_prevSpectrum = audioData.spectrumLeft;
}

void AudioFeatureExtractor::Reset()
{
    m_onsetHistory.clear();
    m_energyHistory.clear();
    m_prevSpectrum.fill(0.0f);
    m_beatPhaseAccumulator = 0.0f;
    m_bpmCandidates.clear();
    m_lastOnsetTime = 0.0f;
    m_interBeatIntervals.clear();
    m_prevOnsetFunction = 0.0f;
    m_peakLevel = 0.0f;
    m_rmsLevel = 0.0f;
}

// ====== Spectral Analysis ======

float AudioFeatureExtractor::CalculateSpectralCentroid(const std::array<float, SpectrumSamples>& spectrum)
{
    float weightedSum = 0.0f;
    float totalMagnitude = 0.0f;

    for (size_t i = 0; i < SpectrumSamples; ++i)
    {
        float magnitude = spectrum[i];
        weightedSum += static_cast<float>(i) * magnitude;
        totalMagnitude += magnitude;
    }

    if (totalMagnitude < 0.0001f)
    {
        return 0.5f; // Default to mid-range if silent
    }

    // Normalize to 0-1 range
    float centroid = weightedSum / (totalMagnitude * static_cast<float>(SpectrumSamples));
    return std::clamp(centroid, 0.0f, 1.0f);
}

float AudioFeatureExtractor::CalculateSpectralFlatness(const std::array<float, SpectrumSamples>& spectrum)
{
    // Spectral flatness = geometric mean / arithmetic mean
    // High flatness = noisy (uniform spectrum)
    // Low flatness = tonal (peaked spectrum)

    float geometricMean = 0.0f;
    float arithmeticMean = 0.0f;
    int validBins = 0;

    for (size_t i = 0; i < SpectrumSamples; ++i)
    {
        float magnitude = spectrum[i];
        if (magnitude > 0.0001f) // Avoid log(0)
        {
            geometricMean += std::log(magnitude);
            arithmeticMean += magnitude;
            validBins++;
        }
    }

    if (validBins == 0)
    {
        return 0.5f; // Default for silence
    }

    geometricMean = std::exp(geometricMean / static_cast<float>(validBins));
    arithmeticMean /= static_cast<float>(validBins);

    if (arithmeticMean < 0.0001f)
    {
        return 0.5f;
    }

    float flatness = geometricMean / arithmeticMean;
    return std::clamp(flatness, 0.0f, 1.0f);
}

float AudioFeatureExtractor::CalculateSpectralRolloff(const std::array<float, SpectrumSamples>& spectrum)
{
    // Find frequency below which 85% of spectral energy is contained

    float totalEnergy = 0.0f;
    for (size_t i = 0; i < SpectrumSamples; ++i)
    {
        totalEnergy += spectrum[i] * spectrum[i];
    }

    if (totalEnergy < 0.0001f)
    {
        return 0.5f; // Default for silence
    }

    float threshold = 0.85f * totalEnergy;
    float cumulativeEnergy = 0.0f;

    for (size_t i = 0; i < SpectrumSamples; ++i)
    {
        cumulativeEnergy += spectrum[i] * spectrum[i];
        if (cumulativeEnergy >= threshold)
        {
            return static_cast<float>(i) / static_cast<float>(SpectrumSamples);
        }
    }

    return 1.0f; // All energy throughout spectrum
}

float AudioFeatureExtractor::CalculateSpectralFlux(const std::array<float, SpectrumSamples>& spectrum)
{
    // Spectral flux = measure of how quickly spectrum changes
    // Sum of squared differences between consecutive frames

    float flux = 0.0f;

    for (size_t i = 0; i < SpectrumSamples; ++i)
    {
        float diff = spectrum[i] - m_prevSpectrum[i];
        // Only positive changes (increasing energy)
        if (diff > 0.0f)
        {
            flux += diff * diff;
        }
    }

    // Normalize roughly to 0-1 range
    flux = std::sqrt(flux / static_cast<float>(SpectrumSamples));
    return std::clamp(flux, 0.0f, 1.0f);
}

// ====== Energy Analysis ======

void AudioFeatureExtractor::CalculateEnergy(const FrameAudioData& audioData, AudioFeatures& features)
{
    // Combine RMS volume with spectral flux for overall energy
    float volumeEnergy = audioData.vol;

    // Add spectral flux contribution (changes are exciting)
    float fluxContribution = features.spectralFlux * 0.3f;

    features.energy = std::clamp(volumeEnergy + fluxContribution, 0.0f, 1.0f);

    // Store in history for dynamic range calculation
    m_energyHistory.push_back(features.energy);
    if (m_energyHistory.size() > MaxEnergyHistory)
    {
        m_energyHistory.pop_front();
    }
}

void AudioFeatureExtractor::UpdateSmoothedEnergy(AudioFeatures& features, float deltaTime)
{
    // Attack/release envelope follower
    float target = features.energy;
    float current = features.energySmoothed;

    if (target > current)
    {
        // Attack (fast rise)
        float attackCoeff = 1.0f - std::exp(-deltaTime / m_energyAttackTime);
        features.energySmoothed = current + (target - current) * attackCoeff;
    }
    else
    {
        // Release (slow fall)
        float releaseCoeff = 1.0f - std::exp(-deltaTime / m_energyReleaseTime);
        features.energySmoothed = current + (target - current) * releaseCoeff;
    }

    features.energySmoothed = std::clamp(features.energySmoothed, 0.0f, 1.0f);
}

// ====== Onset Detection ======

void AudioFeatureExtractor::DetectOnsets(const FrameAudioData& audioData, AudioFeatures& features)
{
    // Onset detection function: combine spectral flux with bass energy change
    float onsetFunction = features.spectralFlux * 0.7f + (audioData.bass - audioData.bassAtt) * 0.3f;
    onsetFunction = std::max(0.0f, onsetFunction);

    // Peak picking: onset when function exceeds threshold and is local maximum
    bool isPeak = onsetFunction > m_prevOnsetFunction && onsetFunction > m_onsetThreshold;

    if (isPeak)
    {
        features.onsetDetected = true;
        features.onsetStrength = std::min(onsetFunction, 1.0f);

        // Store onset for BPM detection
        m_onsetHistory.push_back(onsetFunction);
        if (m_onsetHistory.size() > MaxOnsetHistory)
        {
            m_onsetHistory.pop_front();
        }
    }
    else
    {
        features.onsetDetected = false;
        features.onsetStrength = 0.0f;
    }

    m_prevOnsetFunction = onsetFunction;
}

// ====== BPM Detection ======

void AudioFeatureExtractor::DetectBPM(const FrameAudioData& audioData, AudioFeatures& features, float deltaTime)
{
    // Simple but effective BPM detection using onset intervals

    if (features.onsetDetected)
    {
        // Calculate interval since last onset
        if (m_lastOnsetTime > 0.0f)
        {
            float interval = deltaTime; // Simplified: would accumulate in real impl
            m_interBeatIntervals.push_back(interval);

            if (m_interBeatIntervals.size() > MaxInterBeatIntervals)
            {
                m_interBeatIntervals.pop_front();
            }

            // Estimate BPM from median interval
            if (m_interBeatIntervals.size() >= 4)
            {
                std::vector<float> sorted(m_interBeatIntervals.begin(), m_interBeatIntervals.end());
                std::sort(sorted.begin(), sorted.end());

                float medianInterval = sorted[sorted.size() / 2];

                // Convert interval to BPM (with sanity check)
                if (medianInterval > 0.001f)
                {
                    float estimatedBPM = 60.0f / medianInterval;
                    estimatedBPM = std::clamp(estimatedBPM, 40.0f, 200.0f);

                    m_bpmCandidates.push_back(estimatedBPM);
                    if (m_bpmCandidates.size() > MaxBPMCandidates)
                    {
                        m_bpmCandidates.erase(m_bpmCandidates.begin());
                    }

                    // Vote-based BPM from recent candidates
                    float sumBPM = std::accumulate(m_bpmCandidates.begin(), m_bpmCandidates.end(), 0.0f);
                    float avgBPM = sumBPM / static_cast<float>(m_bpmCandidates.size());

                    // Smooth BPM updates
                    features.bpm = features.bpm * m_bpmSmoothingFactor + avgBPM * (1.0f - m_bpmSmoothingFactor);

                    // Confidence based on variance of candidates
                    float variance = 0.0f;
                    for (float bpm : m_bpmCandidates)
                    {
                        float diff = bpm - features.bpm;
                        variance += diff * diff;
                    }
                    variance /= static_cast<float>(m_bpmCandidates.size());

                    // Low variance = high confidence
                    features.bpmConfidence = 1.0f / (1.0f + std::sqrt(variance) / 10.0f);
                    features.bpmConfidence = std::clamp(features.bpmConfidence, 0.0f, 1.0f);
                }
            }
        }

        m_lastOnsetTime += deltaTime;
    }
    else
    {
        m_lastOnsetTime += deltaTime;
    }
}

void AudioFeatureExtractor::UpdateBeatPhase(AudioFeatures& features, float deltaTime)
{
    if (features.bpm > 0.0f)
    {
        // Increment phase based on BPM
        float beatsPerSecond = features.bpm / 60.0f;
        float phaseIncrement = beatsPerSecond * deltaTime;

        m_beatPhaseAccumulator += phaseIncrement;

        // Wrap phase to 0-1
        features.beatPhase = std::fmod(m_beatPhaseAccumulator, 1.0f);

        // Bar phase (assuming 4/4 time)
        features.barPhase = std::fmod(m_beatPhaseAccumulator / 4.0f, 1.0f);

        // Reset accumulator on detected onset to stay locked
        if (features.onsetDetected && features.beatPhase > 0.5f)
        {
            m_beatPhaseAccumulator = 0.0f;
            features.beatPhase = 0.0f;
        }
    }
}

// ====== Perceptual Features ======

void AudioFeatureExtractor::CalculateEmotionalFeatures(AudioFeatures& features)
{
    // Excitement = high energy + high BPM + high spectral centroid
    float energyFactor = features.energy * 0.4f;
    float bpmFactor = std::clamp((features.bpm - 80.0f) / 100.0f, 0.0f, 1.0f) * 0.3f;
    float brightnessFactor = features.spectralCentroid * 0.3f;

    features.excitement = std::clamp(energyFactor + bpmFactor + brightnessFactor, 0.0f, 1.0f);

    // Calmness = inverse of excitement (with some smoothing)
    features.calmness = 1.0f - features.excitement;
}

void AudioFeatureExtractor::UpdateDynamicRange(const FrameAudioData& audioData, AudioFeatures& features)
{
    // Track peak and RMS levels
    float currentLevel = audioData.vol;

    // Update peak (with slow decay)
    if (currentLevel > m_peakLevel)
    {
        m_peakLevel = currentLevel;
    }
    else
    {
        m_peakLevel *= 0.999f; // Slow decay
    }

    // Update RMS (running average)
    m_rmsLevel = m_rmsLevel * 0.95f + currentLevel * 0.05f;

    // Dynamic range = difference between peak and RMS
    if (m_peakLevel > 0.001f)
    {
        float range = (m_peakLevel - m_rmsLevel) / m_peakLevel;
        features.dynamicRange = std::clamp(range, 0.0f, 1.0f);
    }
    else
    {
        features.dynamicRange = 0.5f; // Neutral for silence
    }

    // Also calculate perceived loudness (simplified LUFS-like)
    features.loudness = std::sqrt(m_rmsLevel); // Closer to perceived loudness
}

// ====== Genre Hints ======

void AudioFeatureExtractor::CalculateGenreHints(const AudioFeatures& features, AudioFeatures& output)
{
    // Simple heuristic-based genre classification
    // This is NOT machine learning, just educated guesses!

    // Electronic/EDM: High BPM, high energy, mid-high brightness
    float edmScore = 0.0f;
    if (features.bpm > 120.0f)
    {
        edmScore += 0.3f;
    }
    if (features.energy > 0.6f)
    {
        edmScore += 0.3f;
    }
    if (features.spectralCentroid > 0.5f)
    {
        edmScore += 0.2f;
    }
    if (features.excitement > 0.7f)
    {
        edmScore += 0.2f;
    }

    // Rock/Metal: High energy, mid BPM, high dynamic range
    float rockScore = 0.0f;
    if (features.bpm > 100.0f && features.bpm < 160.0f)
    {
        rockScore += 0.3f;
    }
    if (features.energy > 0.7f)
    {
        rockScore += 0.4f;
    }
    if (features.dynamicRange > 0.4f)
    {
        rockScore += 0.3f;
    }

    // Hip-Hop/Rap: Mid BPM, strong bass, rhythmic
    float hiphopScore = 0.0f;
    if (features.bpm > 80.0f && features.bpm < 110.0f)
    {
        hiphopScore += 0.5f;
    }
    if (features.spectralCentroid < 0.4f) // Bass-heavy
    {
        hiphopScore += 0.3f;
    }
    if (features.bpmConfidence > 0.6f) // Steady rhythm
    {
        hiphopScore += 0.2f;
    }

    // Classical/Orchestral: High dynamic range, low flatness (tonal)
    float classicalScore = 0.0f;
    if (features.dynamicRange > 0.6f)
    {
        classicalScore += 0.4f;
    }
    if (features.spectralFlatness < 0.3f) // Tonal
    {
        classicalScore += 0.3f;
    }
    if (features.calmness > 0.5f)
    {
        classicalScore += 0.3f;
    }

    // Jazz/Blues: Mid dynamic range, moderate energy
    float jazzScore = 0.0f;
    if (features.dynamicRange > 0.4f && features.dynamicRange < 0.7f)
    {
        jazzScore += 0.4f;
    }
    if (features.spectralFlatness < 0.4f)
    {
        jazzScore += 0.3f;
    }
    if (features.energy > 0.3f && features.energy < 0.7f)
    {
        jazzScore += 0.3f;
    }

    // Ambient/Drone: Low energy, high calmness, low dynamic range
    float ambientScore = 0.0f;
    if (features.calmness > 0.7f)
    {
        ambientScore += 0.5f;
    }
    if (features.energy < 0.4f)
    {
        ambientScore += 0.3f;
    }
    if (features.dynamicRange < 0.3f)
    {
        ambientScore += 0.2f;
    }

    // Normalize scores to probabilities
    float totalScore = edmScore + rockScore + hiphopScore + classicalScore + jazzScore + ambientScore;

    if (totalScore > 0.001f)
    {
        output.genreHints[0] = std::clamp(edmScore / totalScore, 0.0f, 1.0f);
        output.genreHints[1] = std::clamp(rockScore / totalScore, 0.0f, 1.0f);
        output.genreHints[2] = std::clamp(hiphopScore / totalScore, 0.0f, 1.0f);
        output.genreHints[3] = std::clamp(classicalScore / totalScore, 0.0f, 1.0f);
        output.genreHints[4] = std::clamp(jazzScore / totalScore, 0.0f, 1.0f);
        output.genreHints[5] = std::clamp(ambientScore / totalScore, 0.0f, 1.0f);
    }
    else
    {
        // No strong hints, distribute evenly
        output.genreHints.fill(1.0f / 6.0f);
    }
}

} // namespace Audio
} // namespace libprojectM
