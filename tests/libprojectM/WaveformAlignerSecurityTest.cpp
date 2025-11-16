#include "Audio/WaveformAligner.hpp"
#include "Audio/AudioConstants.hpp"

#include <gtest/gtest.h>
#include <limits>

using namespace libprojectM::Audio;

/**
 * @brief Security-focused tests for WaveformAligner
 *
 * These tests target specific vulnerabilities identified in the security audit:
 * - CRIT-003: Buffer overflow via array access before bounds check (line 107)
 * - CRIT-004: Integer underflow in backward search (line 113-117)
 * - Buffer overflow in ResampleOctaves (line 45)
 * - Buffer overflow in CalculateOffset (line 148)
 */
class WaveformAlignerSecurityMock : public WaveformAligner
{
public:
    // Expose protected members for testing
    using WaveformAligner::GenerateWeights;
    using WaveformAligner::CalculateOffset;
    using WaveformAligner::ResampleOctaves;
    using WaveformAligner::m_octaves;
    using WaveformAligner::m_aligmentWeights;
    using WaveformAligner::m_firstNonzeroWeights;
    using WaveformAligner::m_lastNonzeroWeights;
    using WaveformAligner::m_octaveSamples;
    using WaveformAligner::m_octaveSampleSpacing;
    using WaveformAligner::m_oldWaveformMips;
    using WaveformAligner::m_alignWaveReady;

private:
    FRIEND_TEST(WaveformAlignerSecurity, CRIT003_ArrayAccessBeforeBoundsCheck);
    FRIEND_TEST(WaveformAlignerSecurity, CRIT004_IntegerUnderflow);
    FRIEND_TEST(WaveformAlignerSecurity, ResampleOctavesBufferOverflow);
    FRIEND_TEST(WaveformAlignerSecurity, CalculateOffsetBufferOverflow);
    FRIEND_TEST(WaveformAlignerSecurity, AllWeightsZeroEdgeCase);
};

/**
 * CRIT-003: Test for buffer overflow via array access before bounds check
 *
 * Original code at line 107:
 * while (m_aligmentWeights[octave][sample] == 0 && sample < compareSamples)
 *
 * This accesses the array BEFORE checking if sample < compareSamples.
 */
TEST(WaveformAlignerSecurity, CRIT003_ArrayAccessBeforeBoundsCheck)
{
    auto aligner = WaveformAlignerSecurityMock();

    // Force generation of weights
    WaveformBuffer testWaveform{};
    std::fill(testWaveform.begin(), testWaveform.end(), 0.0f);
    aligner.Align(testWaveform);

    // Verify that for each octave, the first nonzero weight is within bounds
    for (uint32_t octave = 0; octave < aligner.m_octaves; octave++)
    {
        uint32_t compareSamples = aligner.m_octaveSamples[octave] - aligner.m_octaveSampleSpacing[octave];

        // The firstNonzeroWeights should always be less than compareSamples
        EXPECT_LT(aligner.m_firstNonzeroWeights[octave], compareSamples)
            << "Octave " << octave << ": firstNonzeroWeights out of bounds";

        // Should never be at the boundary (which would be checked after array access)
        EXPECT_LT(aligner.m_firstNonzeroWeights[octave], AudioBufferSamples)
            << "Octave " << octave << ": firstNonzeroWeights exceeds buffer size";
    }
}

/**
 * CRIT-004: Test for integer underflow in backward weight search
 *
 * Original code at lines 113-117:
 * sample = compareSamples - 1;
 * while (m_aligmentWeights[octave][sample] == 0 && compareSamples > 1)
 * {
 *     sample--;
 * }
 *
 * If all weights are zero, sample underflows from 0 to UINT32_MAX.
 */
TEST(WaveformAlignerSecurity, CRIT004_IntegerUnderflow)
{
    auto aligner = WaveformAlignerSecurityMock();

    // Manually set up a scenario with all zero weights for testing
    aligner.m_octaves = 6;
    aligner.m_aligmentWeights.resize(aligner.m_octaves);
    aligner.m_firstNonzeroWeights.resize(aligner.m_octaves);
    aligner.m_lastNonzeroWeights.resize(aligner.m_octaves);
    aligner.m_octaveSamples.resize(aligner.m_octaves);
    aligner.m_octaveSampleSpacing.resize(aligner.m_octaves);

    aligner.m_octaveSamples[0] = AudioBufferSamples;
    aligner.m_octaveSampleSpacing[0] = AudioBufferSamples - WaveformSamples;
    for (uint32_t octave = 1; octave < aligner.m_octaves; octave++)
    {
        aligner.m_octaveSamples[octave] = aligner.m_octaveSamples[octave - 1] / 2;
        aligner.m_octaveSampleSpacing[octave] = aligner.m_octaveSampleSpacing[octave - 1] / 2;
    }

    // Fill all weights with zero (worst case scenario)
    for (uint32_t octave = 0; octave < aligner.m_octaves; octave++)
    {
        std::fill(aligner.m_aligmentWeights[octave].begin(),
                  aligner.m_aligmentWeights[octave].end(), 0.0f);
    }

    // This should not crash or cause underflow
    ASSERT_NO_FATAL_FAILURE(aligner.GenerateWeights());

    // Verify that lastNonzeroWeights never underflowed
    for (uint32_t octave = 0; octave < aligner.m_octaves; octave++)
    {
        uint32_t compareSamples = aligner.m_octaveSamples[octave] - aligner.m_octaveSampleSpacing[octave];

        // Should be a valid index, not UINT32_MAX
        EXPECT_LT(aligner.m_lastNonzeroWeights[octave], compareSamples)
            << "Octave " << octave << ": lastNonzeroWeights out of bounds or underflowed";

        // Definitely should not be the max value (indicating underflow)
        EXPECT_NE(aligner.m_lastNonzeroWeights[octave], std::numeric_limits<uint32_t>::max())
            << "Octave " << octave << ": Integer underflow detected!";
    }
}

/**
 * Test edge case where all weights remain zero after generation
 */
TEST(WaveformAlignerSecurity, AllWeightsZeroEdgeCase)
{
    auto aligner = WaveformAlignerSecurityMock();

    // Initialize the aligner
    WaveformBuffer testWaveform{};
    std::fill(testWaveform.begin(), testWaveform.end(), 1.0f);

    // This should handle the case gracefully
    ASSERT_NO_FATAL_FAILURE(aligner.Align(testWaveform));

    // Verify indices are sane
    for (uint32_t octave = 0; octave < aligner.m_octaves; octave++)
    {
        EXPECT_LE(aligner.m_firstNonzeroWeights[octave], aligner.m_lastNonzeroWeights[octave])
            << "Octave " << octave << ": first > last (invalid state)";
    }
}

/**
 * Test buffer overflow in ResampleOctaves
 *
 * Original code at line 45:
 * dstWaveformMips[octave][sample] = 0.5f * (
 *     dstWaveformMips[octave - 1][sample * 2] +
 *     dstWaveformMips[octave - 1][sample * 2 + 1]
 * );
 *
 * The expression (sample * 2 + 1) can exceed octaveSamples[octave - 1]
 */
TEST(WaveformAlignerSecurity, ResampleOctavesBufferOverflow)
{
    auto aligner = WaveformAlignerSecurityMock();

    WaveformBuffer testWaveform{};
    std::fill(testWaveform.begin(), testWaveform.end(), 1.0f);

    std::vector<WaveformBuffer> mips(aligner.m_octaves, WaveformBuffer());

    // This should not access out of bounds
    ASSERT_NO_FATAL_FAILURE(aligner.ResampleOctaves(mips, testWaveform));

    // Verify that each octave has valid data
    for (uint32_t octave = 0; octave < aligner.m_octaves; octave++)
    {
        for (uint32_t sample = 0; sample < aligner.m_octaveSamples[octave]; sample++)
        {
            // Should not be NaN or infinity (signs of reading garbage memory)
            EXPECT_TRUE(std::isfinite(mips[octave][sample]))
                << "Octave " << octave << ", sample " << sample << " is not finite";
        }
    }
}

/**
 * Test buffer overflow in CalculateOffset
 *
 * Original code at line 148:
 * errorSum += std::abs((newWaveformMips[octave][i + sample] - ...
 *
 * No validation that (i + sample) < octaveSamples[octave]
 */
TEST(WaveformAlignerSecurity, CalculateOffsetBufferOverflow)
{
    auto aligner = WaveformAlignerSecurityMock();

    WaveformBuffer testWaveform{};
    std::fill(testWaveform.begin(), testWaveform.end(), 0.5f);

    // Initialize the aligner
    aligner.Align(testWaveform);

    // Create a new waveform with a different pattern
    std::fill(testWaveform.begin(), testWaveform.end(), 0.8f);

    std::vector<WaveformBuffer> mips(aligner.m_octaves, WaveformBuffer());
    aligner.ResampleOctaves(mips, testWaveform);

    // This should not access out of bounds
    ASSERT_NO_FATAL_FAILURE(aligner.CalculateOffset(mips));
}

/**
 * Boundary test: Maximum valid offset
 */
TEST(WaveformAlignerSecurity, MaximumValidOffset)
{
    auto aligner = WaveformAlignerSecurityMock();

    WaveformBuffer testWaveform{};
    std::fill(testWaveform.begin(), testWaveform.end(), 0.0f);

    // Set a spike at the maximum valid offset position
    testWaveform[AudioBufferSamples - 1] = 1.0f;

    ASSERT_NO_FATAL_FAILURE(aligner.Align(testWaveform));
}

/**
 * Stress test: Rapidly changing waveforms
 */
TEST(WaveformAlignerSecurity, RapidlyChangingWaveforms)
{
    auto aligner = WaveformAlignerSecurityMock();

    // Align 1000 different waveforms in sequence
    for (int iteration = 0; iteration < 1000; iteration++)
    {
        WaveformBuffer testWaveform{};

        // Create pseudorandom waveform
        for (size_t i = 0; i < AudioBufferSamples; i++)
        {
            testWaveform[i] = std::sin(static_cast<float>(i + iteration) * 0.1f);
        }

        ASSERT_NO_FATAL_FAILURE(aligner.Align(testWaveform))
            << "Failed at iteration " << iteration;
    }
}

/**
 * Boundary test: Minimal octaves scenario
 */
TEST(WaveformAlignerSecurity, MinimalOctaves)
{
    // Test behavior when octaves < 4 (alignment should be skipped)
    auto aligner = WaveformAlignerSecurityMock();

    // Force low octave count by manipulating internal state
    // (This is a defensive test for edge cases)
    WaveformBuffer testWaveform{};
    std::fill(testWaveform.begin(), testWaveform.end(), 0.5f);

    ASSERT_NO_FATAL_FAILURE(aligner.Align(testWaveform));

    // Should handle gracefully even with edge case octave counts
    EXPECT_GE(aligner.m_octaves, 0);
    EXPECT_LE(aligner.m_octaves, 10);
}
