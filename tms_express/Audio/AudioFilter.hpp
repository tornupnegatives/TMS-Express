// Copyright (C) 2023 Joseph Bellahcen <joeclb@icloud.com>

#ifndef TMS_EXPRESS_AUDIO_AUDIOFILTER_HPP_
#define TMS_EXPRESS_AUDIO_AUDIOFILTER_HPP_

#include <array>
#include <string>
#include <vector>

#include "Audio/AudioBuffer.hpp"

namespace tms_express {

/// @brief Implements various digital filters for processing audio samples
class AudioFilter {
 public:
    ///////////////////////////////////////////////////////////////////////////
    // Windowing //////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Applies Hamming window to entire buffer
    /// @param buffer Audio Buffer to apply window to
    void applyHammingWindow(AudioBuffer &buffer) const;

    /// @brief Applies Hamming window to segment of samples
    /// @param buffer Segment vector to apply window to
    void applyHammingWindow(std::vector<float> &segment) const;

    ///////////////////////////////////////////////////////////////////////////
    // Bi-Quadratic Filters ///////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Applies highpass filter to entire buffer
    /// @param buffer Audio Buffer to apply filter to
    /// @param cutoffHz Highpass cutoff frequency, in Hertz
    void applyHighpass(AudioBuffer &buffer, int cutoff_hz);

    /// @brief Applies lowpass filter to entire buffer
    /// @param buffer Audio Buffer to apply filter to
    /// @param cutoffHz Lowpass cutoff frequency, in Hertz
    void applyLowpass(AudioBuffer &buffer, int cutoff_hz);

    ///////////////////////////////////////////////////////////////////////////
    // Simple Filters /////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Applies pre-emphasis filter to entire buffer
    /// @param buffer Audio Buffer to apply filter to
    /// @param alpha Pre-emphasis coefficient (usually 0.9375)
    void applyPreEmphasis(AudioBuffer &buffer, float alpha = 0.9375) const;

 private:
    ///////////////////////////////////////////////////////////////////////////
    // Enums //////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Last-used filter mode
    /// @note The Filter Mode is stored to prevent unnecessary re-calculation
    ///         of filter coefficients
    enum FilterMode {HPF, LPF};

    ///////////////////////////////////////////////////////////////////////////
    // Helpers ////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Applies bi-quadratic filter coefficients to entire Buffer
    /// @param buffer Audio Buffer to apply filter to
    void applyBiquad(AudioBuffer &buffer);

    /// @brief Computes bi-quadratic filter coefficients for a highpass or
    ///         lowpass filter
    void computeCoeffs(FilterMode mode, int cutoff_hz);

    ///////////////////////////////////////////////////////////////////////////
    // Members ////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Bi-quadratic filter coefficients
    std::array<float, 6> coeffs_{0, 0, 0, 0, 0, 0};
};

};  // namespace tms_express

#endif  //  TMS_EXPRESS_AUDIO_AUDIOFILTER_HPP_
