/**
 * @file AudioFeatureExtractorTest.cpp
 * @brief Test suite for advanced audio feature extraction
 */

#include <gtest/gtest.h>

#include <Audio/AudioFeatureExtractor.hpp>
#include <Audio/AudioFeatures.hpp>
#include <Audio/FrameAudioData.hpp>

#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace libprojectM::Audio;

class AudioFeatureExtractorTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        extractor = std::make_unique<AudioFeatureExtractor>();
    }

    /**
     * Generate a sine wave test signal at specified frequency
     */
    void GenerateSineWave(FrameAudioData& audioData, float frequency, float sampleRate = 44100.0f)
    {
        constexpr float amplitude = 0.5f;
        for (size_t i = 0; i < WaveformSamples; ++i)
        {
            float t = static_cast<float>(i) / sampleRate;
            float value = amplitude * std::sin(2.0f * static_cast<float>(M_PI) * frequency * t);
            audioData.waveformLeft[i] = value;
            audioData.waveformRight[i] = value;
        }
    }

    /**
     * Generate spectrum for a specific frequency
     */
    void GenerateSpectrumPeak(FrameAudioData& audioData, size_t peakBin, float peakValue = 1.0f)
    {
        std::fill(audioData.spectrumLeft.begin(), audioData.spectrumLeft.end(), 0.0f);
        std::fill(audioData.spectrumRight.begin(), audioData.spectrumRight.end(), 0.0f);

        if (peakBin < SpectrumSamples)
        {
            audioData.spectrumLeft[peakBin] = peakValue;
            audioData.spectrumRight[peakBin] = peakValue;
        }
    }

    /**
     * Generate a beat pattern for BPM testing
     */
    void GenerateBeatPattern(std::vector<FrameAudioData>& frames, float bpm, size_t numFrames)
    {
        frames.resize(numFrames);
        constexpr float fps = 60.0f; // Assuming 60 FPS
        float beatInterval = 60.0f / bpm;     // seconds per beat
        float framesPerBeat = beatInterval * fps;

        for (size_t i = 0; i < numFrames; ++i)
        {
            // Check if this frame is close to a beat
            float phase = std::fmod(static_cast<float>(i), framesPerBeat) / framesPerBeat;
            bool isBeat = phase < 0.1f; // Beat occurs in first 10% of cycle

            if (isBeat)
            {
                // Strong beat: high bass energy
                frames[i].bass = 1.0f;
                frames[i].mid = 0.5f;
                frames[i].treb = 0.3f;
            }
            else
            {
                // Between beats: low energy
                frames[i].bass = 0.2f;
                frames[i].mid = 0.3f;
                frames[i].treb = 0.2f;
            }

            // Volume follows same pattern
            frames[i].vol = frames[i].bass * 0.8f + frames[i].mid * 0.15f + frames[i].treb * 0.05f;
        }
    }

    std::unique_ptr<AudioFeatureExtractor> extractor;
};

// ====== BPM Detection Tests ======

TEST_F(AudioFeatureExtractorTest, BPMDetection_SlowTempo)
{
    // Test detection of slow tempo (60 BPM = 1 beat per second)
    std::vector<FrameAudioData> frames;
    GenerateBeatPattern(frames, 60.0f, 600); // 10 seconds at 60 FPS

    AudioFeatures features;
    for (const auto& frame : frames)
    {
        extractor->UpdateFeatures(frame, features, 1.0f / 60.0f);
    }

    // After processing many frames, BPM should converge to 60
    EXPECT_NEAR(features.bpm, 60.0f, 5.0f);
    EXPECT_GT(features.bpmConfidence, 0.5f); // Should have reasonable confidence
}

TEST_F(AudioFeatureExtractorTest, BPMDetection_FastTempo)
{
    // Test detection of fast tempo (140 BPM = common dance music)
    std::vector<FrameAudioData> frames;
    GenerateBeatPattern(frames, 140.0f, 600);

    AudioFeatures features;
    for (const auto& frame : frames)
    {
        extractor->UpdateFeatures(frame, features, 1.0f / 60.0f);
    }

    EXPECT_NEAR(features.bpm, 140.0f, 10.0f);
    EXPECT_GT(features.bpmConfidence, 0.5f);
}

TEST_F(AudioFeatureExtractorTest, BPMDetection_LowConfidenceForRandomNoise)
{
    // Random noise should produce low BPM confidence
    FrameAudioData audioData{};

    AudioFeatures features;
    for (int i = 0; i < 600; ++i)
    {
        // Random values
        audioData.bass = static_cast<float>(rand()) / RAND_MAX;
        audioData.mid = static_cast<float>(rand()) / RAND_MAX;
        audioData.treb = static_cast<float>(rand()) / RAND_MAX;

        extractor->UpdateFeatures(audioData, features, 1.0f / 60.0f);
    }

    // Confidence should be low for random data
    EXPECT_LT(features.bpmConfidence, 0.6f);
}

// ====== Spectral Centroid Tests ======

TEST_F(AudioFeatureExtractorTest, SpectralCentroid_BassHeavy)
{
    FrameAudioData audioData{};

    // Put energy in low frequencies
    GenerateSpectrumPeak(audioData, 5, 1.0f);  // Low frequency bin

    AudioFeatures features;
    extractor->UpdateFeatures(audioData, features, 1.0f / 60.0f);

    // Centroid should be low (bass-heavy)
    EXPECT_LT(features.spectralCentroid, 0.3f);
}

TEST_F(AudioFeatureExtractorTest, SpectralCentroid_TrebleHeavy)
{
    FrameAudioData audioData{};

    // Put energy in high frequencies
    GenerateSpectrumPeak(audioData, SpectrumSamples - 10, 1.0f);

    AudioFeatures features;
    extractor->UpdateFeatures(audioData, features, 1.0f / 60.0f);

    // Centroid should be high (treble-heavy)
    EXPECT_GT(features.spectralCentroid, 0.7f);
}

// ====== Energy Tests ======

TEST_F(AudioFeatureExtractorTest, Energy_HighVolume)
{
    FrameAudioData audioData{};
    audioData.vol = 0.9f;
    audioData.bass = 0.8f;
    audioData.mid = 0.7f;
    audioData.treb = 0.6f;

    AudioFeatures features;
    extractor->UpdateFeatures(audioData, features, 1.0f / 60.0f);

    // Energy should be high
    EXPECT_GT(features.energy, 0.6f);
}

TEST_F(AudioFeatureExtractorTest, Energy_LowVolume)
{
    FrameAudioData audioData{};
    audioData.vol = 0.1f;
    audioData.bass = 0.1f;
    audioData.mid = 0.1f;
    audioData.treb = 0.1f;

    AudioFeatures features;
    extractor->UpdateFeatures(audioData, features, 1.0f / 60.0f);

    // Energy should be low
    EXPECT_LT(features.energy, 0.3f);
}

TEST_F(AudioFeatureExtractorTest, EnergySmoothing)
{
    FrameAudioData audioData{};
    AudioFeatures features;

    // Start with high energy
    audioData.vol = 1.0f;
    extractor->UpdateFeatures(audioData, features, 1.0f / 60.0f);
    float initialEnergy = features.energySmoothed;

    // Drop to low energy
    audioData.vol = 0.0f;
    extractor->UpdateFeatures(audioData, features, 1.0f / 60.0f);

    // Smoothed energy should decrease slowly
    EXPECT_LT(features.energySmoothed, initialEnergy);
    EXPECT_GT(features.energySmoothed, 0.0f); // But not instant drop
}

// ====== Onset Detection Tests ======

TEST_F(AudioFeatureExtractorTest, OnsetDetection_BassKick)
{
    FrameAudioData audioData{};
    AudioFeatures features;

    // Low energy
    audioData.bass = 0.2f;
    extractor->UpdateFeatures(audioData, features, 1.0f / 60.0f);

    // Sudden bass spike (kick drum)
    audioData.bass = 1.0f;
    extractor->UpdateFeatures(audioData, features, 1.0f / 60.0f);

    // Should detect onset
    EXPECT_TRUE(features.onsetDetected);
    EXPECT_GT(features.onsetStrength, 0.3f);
}

TEST_F(AudioFeatureExtractorTest, OnsetDetection_NoSpike)
{
    FrameAudioData audioData{};
    AudioFeatures features;

    // Constant low energy
    for (int i = 0; i < 10; ++i)
    {
        audioData.bass = 0.3f;
        audioData.mid = 0.3f;
        extractor->UpdateFeatures(audioData, features, 1.0f / 60.0f);
    }

    // No onset should be detected
    EXPECT_FALSE(features.onsetDetected);
    EXPECT_LT(features.onsetStrength, 0.1f);
}

// ====== Spectral Flatness Tests ======

TEST_F(AudioFeatureExtractorTest, SpectralFlatness_PureTone)
{
    FrameAudioData audioData{};

    // Single frequency peak = pure tone
    GenerateSpectrumPeak(audioData, 50, 1.0f);

    AudioFeatures features;
    extractor->UpdateFeatures(audioData, features, 1.0f / 60.0f);

    // Flatness should be low (not flat = tonal)
    EXPECT_LT(features.spectralFlatness, 0.3f);
}

TEST_F(AudioFeatureExtractorTest, SpectralFlatness_WhiteNoise)
{
    FrameAudioData audioData{};

    // Uniform spectrum = white noise
    std::fill(audioData.spectrumLeft.begin(), audioData.spectrumLeft.end(), 0.5f);
    std::fill(audioData.spectrumRight.begin(), audioData.spectrumRight.end(), 0.5f);

    AudioFeatures features;
    extractor->UpdateFeatures(audioData, features, 1.0f / 60.0f);

    // Flatness should be high (flat = noisy)
    EXPECT_GT(features.spectralFlatness, 0.7f);
}

// ====== Excitement/Calmness Tests ======

TEST_F(AudioFeatureExtractorTest, Excitement_HighEnergyFastTempo)
{
    std::vector<FrameAudioData> frames;
    GenerateBeatPattern(frames, 150.0f, 600); // Fast BPM

    AudioFeatures features;
    for (auto& frame : frames)
    {
        // Boost energy
        frame.vol = 0.8f;
        frame.bass = 0.9f;

        extractor->UpdateFeatures(frame, features, 1.0f / 60.0f);
    }

    // Should be exciting
    EXPECT_GT(features.excitement, 0.5f);
    EXPECT_LT(features.calmness, 0.5f);
}

TEST_F(AudioFeatureExtractorTest, Calmness_LowEnergySlowTempo)
{
    std::vector<FrameAudioData> frames;
    GenerateBeatPattern(frames, 60.0f, 600); // Slow BPM

    AudioFeatures features;
    for (auto& frame : frames)
    {
        // Low energy
        frame.vol = 0.2f;
        frame.bass = 0.2f;

        extractor->UpdateFeatures(frame, features, 1.0f / 60.0f);
    }

    // Should be calm
    EXPECT_GT(features.calmness, 0.5f);
    EXPECT_LT(features.excitement, 0.5f);
}

// ====== Beat Phase Tests ======

TEST_F(AudioFeatureExtractorTest, BeatPhase_Cycles)
{
    std::vector<FrameAudioData> frames;
    GenerateBeatPattern(frames, 120.0f, 600);

    AudioFeatures features;
    float previousPhase = 0.0f;
    int phaseResets = 0;

    for (const auto& frame : frames)
    {
        extractor->UpdateFeatures(frame, features, 1.0f / 60.0f);

        // Phase should cycle 0.0 -> 1.0 -> 0.0
        if (features.beatPhase < previousPhase)
        {
            phaseResets++;
        }

        EXPECT_GE(features.beatPhase, 0.0f);
        EXPECT_LE(features.beatPhase, 1.0f);

        previousPhase = features.beatPhase;
    }

    // Should see multiple phase resets (beats)
    EXPECT_GT(phaseResets, 10);
}
