// Copyright 2023 Joseph Bellahcen <joeclb@icloud.com>

#ifndef TMS_EXPRESS_USER_INTERFACES_COMMANDLINEAPP_HPP_
#define TMS_EXPRESS_USER_INTERFACES_COMMANDLINEAPP_HPP_

#include <string>

#include <CLI/CLI.hpp>

#include "bitstream/BitstreamGenerator.hpp"

namespace tms_express::ui {

/// @brief Exposes command-line interface (CLI) for application
class CommandLineApp: public CLI::App {
 public:
    ///////////////////////////////////////////////////////////////////////////
    // Initializers ///////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Creates and configures a new CLI for application
    CommandLineApp();

    ///////////////////////////////////////////////////////////////////////////
    // Interface //////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Runs command-line application
    /// @param argc Number of command-line arguments, passed from main()
    /// @param argv Command line arguments, passed from main()
    /// @return Zero if exitted successfully, non-zero otherwise
    int run(int argc, char** argv);

 private:
    ///////////////////////////////////////////////////////////////////////////
    // Helper Methods /////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Attaches command-line arguments to Encoder application
    void setupEncoder();

    ///////////////////////////////////////////////////////////////////////////
    // Command-Line Applications //////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Encoder application, exposed as "encode" command, which converts
    ///         audio files to LPC bitstreams
    CLI::App* encoder;

    ///////////////////////////////////////////////////////////////////////////
    // Encoder Application Members ////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Path to existing input audio file
    std::string input_path_;

    /// @brief Path to new output bitstream file
    std::string output_path_;

    /// @brief Analysis (segmentation) window width, in milliseconds
    float analysis_window_ms_ = 25.0f;

    /// @brief Highpass filter cutoff, in Hertz
    int hpf_cutoff_ = 1000;

    /// @brief Lowpass filter cutoff, in Hertz
    int lpf_cutoff_ = 800;

    /// @brief Pre-emphasis filter coefficient
    float preemphasis_alpha_ = -0.9375f;

    /// @brief Bitstream format
    BitstreamGenerator::EncoderStyle bitstream_format_ =
        BitstreamGenerator::EncoderStyle::ENCODERSTYLE_ASCII;

    /// @brief true to append stop frame to end of bitstream, false otherwise
    bool no_stop_frame_ = false;

    /// @brief Gain shift (offset) from gain/energy entry in coding table
    int gain_shift_ = 2;

    /// @brief Max voiced (vowel) gain, in decibels
    float max_voiced_gain_ = 37.5f;

    /// @brief Max unvoiced (vowel) gain, in decibels
    float max_unvoiced_gain_ = 30.0f;

    /// @brief true to detect repeat Frames, false otherwise
    bool repeat_frames_ = false;

    /// @brief Pitch analysis ceiling frequency, in Hertz
    int max_pitch_frq_ = 500;

    /// @brief Pitch analysis floor frequency, in Hertz
    int min_pitch_frq_ = 50;
};

};  // namespace tms_express::ui

#endif  // TMS_EXPRESS_USER_INTERFACES_COMMANDLINEAPP_HPP_
