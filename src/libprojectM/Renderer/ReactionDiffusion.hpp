/**
 * @file ReactionDiffusion.hpp
 * @brief Reaction-Diffusion pattern generator using Gray-Scott model
 *
 * Implements the Gray-Scott reaction-diffusion system for creating organic
 * pattern formations like spots, stripes, spirals, and maze-like structures.
 *
 * The system simulates two chemicals (U and V) reacting and diffusing:
 * - dU/dt = Du * ∇²U - UV² + F(1-U)
 * - dV/dt = Dv * ∇²V + UV² - (F+k)V
 *
 * Different F (feed) and k (kill) parameters create different patterns.
 */

#pragma once

#include <projectM-4/projectM_export.h>

#include <cstddef>
#include <vector>

namespace libprojectM {
namespace Renderer {

/**
 * @brief Cell in reaction-diffusion grid
 */
struct RDCell
{
    float u{1.0f};  //!< Concentration of chemical U
    float v{0.0f};  //!< Concentration of chemical V
};

/**
 * @brief Boundary conditions for reaction-diffusion
 */
enum class RDBoundaryMode
{
    Wrap,   //!< Toroidal (wrap around)
    Fixed   //!< Fixed boundary values
};

/**
 * @brief Preset parameter configurations
 */
enum class ReactionDiffusionPreset
{
    Spots,      //!< Spot pattern (F=0.055, k=0.062)
    Stripes,    //!< Stripe pattern (F=0.035, k=0.065)
    Spirals,    //!< Spiral waves (F=0.014, k=0.054)
    Chaos,      //!< Chaotic pattern (F=0.062, k=0.061)
    Waves,      //!< Wave pattern (F=0.020, k=0.050)
    Custom      //!< User-defined parameters
};

/**
 * @brief Reaction-Diffusion pattern generator
 *
 * Simulates the Gray-Scott model on a 2D grid to create organic patterns.
 * The system exhibits emergent behavior where two chemicals react and diffuse,
 * creating self-organizing patterns.
 *
 * **The Gray-Scott Equations:**
 * ```
 * dU/dt = Du * ∇²U - UV² + F(1-U)
 * dV/dt = Dv * ∇²V + UV² - (F+k)V
 * ```
 *
 * Where:
 * - U, V: Chemical concentrations (0-1)
 * - Du, Dv: Diffusion rates
 * - F: Feed rate (how fast U is added)
 * - k: Kill rate (how fast V is removed)
 * - ∇²: Laplacian operator (diffusion)
 *
 * **Parameter Guide:**
 * - Spots: F=0.055, k=0.062 (leopard spots, cell division)
 * - Stripes: F=0.035, k=0.065 (zebra stripes, coral)
 * - Spirals: F=0.014, k=0.054 (spiral waves, chemical oscillators)
 * - Chaos: F=0.062, k=0.061 (unstable, rapidly changing)
 *
 * **Audio Reactivity:**
 * - Feed rate modulation: Energy affects pattern growth
 * - Kill rate modulation: Beat detection affects pattern decay
 * - Diffusion rate: Frequency spectrum affects spread
 *
 * Usage:
 * @code
 * ReactionDiffusion rd(256, 256);
 * rd.SetPreset(ReactionDiffusionPreset::Spots);
 * rd.SeedRandom(10, 5); // 10 seed points, radius 5
 *
 * // Each frame
 * rd.SetAudioModulation(audioEnergy, beatIntensity, 0.0f);
 * rd.Update(deltaTime);
 *
 * // Get texture for rendering
 * auto textureData = rd.GetTextureData();
 * @endcode
 */
class PROJECTM_EXPORT ReactionDiffusion
{
public:
    /**
     * @brief Construct reaction-diffusion grid
     *
     * @param width Grid width (typically 64-512)
     * @param height Grid height (typically 64-512)
     */
    ReactionDiffusion(size_t width, size_t height);

    /**
     * @brief Update simulation
     *
     * Performs one time step of the reaction-diffusion simulation.
     *
     * @param deltaTime Time step (smaller = more accurate, but slower)
     */
    void Update(float deltaTime);

    /**
     * @brief Reset grid to initial state
     *
     * Fills grid with U, clears V.
     */
    void Reset();

    // ====== Grid Access ======

    /**
     * @brief Get grid dimensions
     */
    size_t GetWidth() const { return m_width; }
    size_t GetHeight() const { return m_height; }

    /**
     * @brief Get entire grid
     */
    const std::vector<RDCell>& GetGrid() const { return m_grid; }

    /**
     * @brief Get cell at position
     */
    const RDCell& GetCell(size_t x, size_t y) const;

    /**
     * @brief Set cell concentrations
     */
    void SetCell(size_t x, size_t y, float u, float v);

    // ====== Seeding ======

    /**
     * @brief Seed V at center of grid
     *
     * Creates a circular spot of chemical V.
     *
     * @param radius Spot radius in pixels
     */
    void SeedCenter(int radius = 5);

    /**
     * @brief Seed V at random positions
     *
     * @param count Number of seed points
     * @param radius Radius of each seed spot
     */
    void SeedRandom(int count, int radius = 3);

    /**
     * @brief Seed V at specific position
     */
    void SeedAt(int x, int y, int radius);

    // ====== Parameters ======

    /**
     * @brief Set preset parameter configuration
     */
    void SetPreset(ReactionDiffusionPreset preset);

    /**
     * @brief Set feed rate (F parameter)
     *
     * Controls how fast U is replenished.
     * Higher = faster pattern growth.
     *
     * @param feedRate Feed rate (0.01 - 0.1)
     */
    void SetFeedRate(float feedRate) { m_feedRate = feedRate; }

    /**
     * @brief Set kill rate (k parameter)
     *
     * Controls how fast V decays.
     * Higher = faster pattern decay.
     *
     * @param killRate Kill rate (0.04 - 0.07)
     */
    void SetKillRate(float killRate) { m_killRate = killRate; }

    /**
     * @brief Set U diffusion rate
     */
    void SetDiffusionRateU(float rate) { m_diffusionU = rate; }

    /**
     * @brief Set V diffusion rate
     */
    void SetDiffusionRateV(float rate) { m_diffusionV = rate; }

    /**
     * @brief Get feed rate
     */
    float GetFeedRate() const { return m_feedRate; }

    /**
     * @brief Get kill rate
     */
    float GetKillRate() const { return m_killRate; }

    /**
     * @brief Get U diffusion rate
     */
    float GetDiffusionRateU() const { return m_diffusionU; }

    /**
     * @brief Get V diffusion rate
     */
    float GetDiffusionRateV() const { return m_diffusionV; }

    // ====== Boundary Conditions ======

    /**
     * @brief Set boundary mode
     */
    void SetBoundaryMode(RDBoundaryMode mode) { m_boundaryMode = mode; }

    // ====== Audio Reactivity ======

    /**
     * @brief Set audio modulation parameters
     *
     * Modulates reaction-diffusion parameters based on audio.
     *
     * @param energy Overall audio energy (0-1)
     * @param beatIntensity Beat intensity (0-1)
     * @param bassLevel Bass level (0-1)
     */
    void SetAudioModulation(float energy, float beatIntensity, float bassLevel);

    // ====== Texture Generation ======

    /**
     * @brief Get RGB texture data
     *
     * Generates texture data suitable for OpenGL upload.
     * Maps V concentration to color (0-255).
     *
     * @return Vector of RGB bytes (width * height * 3)
     */
    std::vector<float> GetTextureData() const;

private:
    /**
     * @brief Calculate Laplacian (diffusion operator)
     */
    float CalculateLaplacian(const std::vector<RDCell>& grid, size_t x, size_t y, bool isU);

    /**
     * @brief Get grid index from coordinates
     */
    size_t GetIndex(size_t x, size_t y) const { return y * m_width + x; }

    /**
     * @brief Clamp concentration to [0, 1]
     */
    float Clamp01(float value) const;

    // ====== Grid Storage ======

    size_t m_width;                              //!< Grid width
    size_t m_height;                             //!< Grid height
    std::vector<RDCell> m_grid;                  //!< Current grid state
    std::vector<RDCell> m_gridNext;              //!< Next grid state (double buffering)

    // ====== Parameters ======

    float m_feedRate{0.055f};                    //!< F parameter
    float m_killRate{0.062f};                    //!< k parameter
    float m_diffusionU{0.16f};                   //!< Du (U diffusion rate)
    float m_diffusionV{0.08f};                   //!< Dv (V diffusion rate)

    RDBoundaryMode m_boundaryMode{RDBoundaryMode::Wrap};  //!< Boundary handling

    // ====== Audio Modulation State ======

    float m_audioEnergy{0.0f};                   //!< Current audio energy
    float m_beatIntensity{0.0f};                 //!< Current beat intensity
    float m_bassLevel{0.0f};                     //!< Current bass level

    float m_baseFeedRate{0.055f};                //!< Base F (before modulation)
    float m_baseKillRate{0.062f};                //!< Base k (before modulation)

    // ====== Constants ======

    static constexpr float TimeScale = 1.0f;     //!< Simulation time scale
};

} // namespace Renderer
} // namespace libprojectM
