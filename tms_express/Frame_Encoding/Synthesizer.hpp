// Copyright (C) 2023 Joseph Bellahcen <joeclb@icloud.com>

#ifndef TMS_EXPRESS_FRAME_ENCODING_SYNTHESIZER_HPP_
#define TMS_EXPRESS_FRAME_ENCODING_SYNTHESIZER_HPP_

#include <string>
#include <vector>

#include "Frame_Encoding/Frame.hpp"

namespace tms_express {

/// @brief Synthesizes Frame table as PCM audio samples
class Synthesizer {
 public:
    ///////////////////////////////////////////////////////////////////////////
    // Initializers ///////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Creates a new Synthesizer
    /// @param sample_rate_hz Sample rate of synthesized audio, in Hertz
    /// @param frame_rate_ms Duration of each Frame, in milliseconds
    explicit Synthesizer(int sample_rate_hz = 8000,
        float frame_rate_ms = 25.0f);

    ///////////////////////////////////////////////////////////////////////////
    // Synthesis Interfaces ///////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Push Frame table through synthesis filter
    /// @param frames Frame table to synthesize
    /// @return Synthesized PCM samples
    std::vector<float> synthesize(const std::vector<Frame>& frames);

    ///////////////////////////////////////////////////////////////////////////
    // Accessors //////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Accesses synthesized samples
    /// @return Synthesized PCM samples
    std::vector<float> getSamples() const;

    ///////////////////////////////////////////////////////////////////////////
    // Static Utilities ///////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Renders samples to audio file
    /// @param samples Synthesized PCM audio samples
    /// @param path Output path to audio file
    /// @param sample_rate_hz Sample rate used to generate samples, in Hertz
    /// @param frame_rate_ms Duration of each Frame, in milliseconds
    static void render(const std::vector<float> &samples,
        const std::string& path, int sample_rate_hz, float frame_rate_ms);

 private:
    ///////////////////////////////////////////////////////////////////////////
    // Synthesis Functions ////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Advances random noise generator state machine
    /// @return Polarity of unvoiced sample to generate, true for positive,
    ///         false for negative
    bool updateNoiseGenerator();

    /// @brief Loads new Frame into synthesizer
    /// @param frame Frame to synthesize
    /// @return true if stop Frame encountered, at which point synthesis should
    ///             halt, false otherwise
    bool updateSynthTable(Frame frame);

    /// @brief Pushes Frame parameters through synthesis lattice filter
    /// @return Newly synthesized sample
    float updateLatticeFilter();

    ///////////////////////////////////////////////////////////////////////////
    // Utility Functions //////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Resets synthesizer to initialization state
    void reset();

    ///////////////////////////////////////////////////////////////////////////
    // Members: Audio IO //////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Sampling rate of synthesized audio, in Hertz
    int sample_rate_hz_;

    /// @brief Window width (duration of each Frame), in milliseconds
    float window_width_ms_;

    /// @brief Number of audio samples which comprise a single Frame
    int n_samples_per_frame_;

    /// @brief Synthesized PCM samples
    std::vector<float> samples_;

    ///////////////////////////////////////////////////////////////////////////
    // Members: Lattice Filter ////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    float energy_, period_;
    float k1_, k2_, k3_, k4_, k5_, k6_, k7_, k8_, k9_, k10_;
    float x0_, x1_, x2_, x3_, x4_, x5_, x6_, x7_, x8_, x9_, u0_;
    int rand_noise_, period_count_;
};

};  // namespace tms_express

#endif  // TMS_EXPRESS_FRAME_ENCODING_SYNTHESIZER_HPP_
