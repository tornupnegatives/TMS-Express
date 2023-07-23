// Copyright (C) 2023 Joseph Bellahcen <joeclb@icloud.com>

#ifndef TMS_EXPRESS_BITSTREAM_GENERATION_BITSTREAMGENERATOR_HPP_
#define TMS_EXPRESS_BITSTREAM_GENERATION_BITSTREAMGENERATOR_HPP_

#include <string>
#include <vector>

#include "Frame_Encoding/Frame.hpp"

namespace tms_express {

/// @brief Facilitates IO, preprocessing, analysis, and encoding
class BitstreamGenerator {
 public:
    ///////////////////////////////////////////////////////////////////////////
    // Enums //////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Defines the format of the bitstream
    enum EncoderStyle {
        /// @brief Bitstream with comma-delimited ASCII hex bytes
        ENCODERSTYLE_ASCII,

        /// @brief Bitstream as C header which defines array of bytes
        ENCODERSTYLE_C,

        /// @brief Bitstream as C header which defines array of bytes in PROGMEM
        ENCODERSTYLE_ARDUINO,

        /// @brief Bitstream as JSON file
        ENCODERSTYLE_JSON
    };

    ///////////////////////////////////////////////////////////////////////////
    // Initializers ///////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Creates a new Bitstream Generator with the given configuration
    /// @param window_width_ms Segmentation widnow width, in milliseconds
    /// @param highpass_cutoff_hz Highpass filter cutoff frequency, in Hertz
    /// @param lowpass_cutoff_hz Lowpass filter cutoff frequency, in Hertz
    /// @param pre_emphasis_alpha Pre-emphasis filter coefficient
    /// @param style Encoder Style dictating bitstream format
    /// @param include_stop_frame true to end the bitstream with a stop frame,
    ///                             false otherwise
    /// @param gain_shift Integer gain shift to be applied in post-processing
    /// @param max_voiced_gain_db Max voiced frame gain, in decibels
    /// @param max_unvoiced_gain_db Max unvoiced frame gain, in decibels
    /// @param detect_repeat_frames true to detect similar frames, essentially
    ///                             compressing the bitstream, false otherwise
    /// @param max_pitch_hz Pitch frequency ceiling, in Hertz
    /// @param min_pitch_hz Pitch frequency floor, in Hertz
    BitstreamGenerator(float window_width_ms, int highpass_cutoff_hz,
        int lowpass_cutoff_hz, float pre_emphasis_alpha, EncoderStyle style,
        bool include_stop_frame, int gain_shift, float max_voiced_gain_db,
        float max_unvoiced_gain_db, bool detect_repeat_frames,
        int max_pitch_hz, int min_pitch_hz);

    ///////////////////////////////////////////////////////////////////////////
    // Encoding ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Produces bitstream from provided audio file
    /// @param audio_input_path Path to audio file
    /// @param bitstream_name Name of bitstream, for C headers, which will
    ///                         become the name of the byte array
    /// @param output_path Output path of bitstream file
    void encode(const std::string &audio_input_path,
        const std::string &bitstream_name, const std::string &output_path)
        const;

    /// @brief Produces composite bitstream from multiple audio files
    /// @param audio_input_paths Vector of audio file paths as inputs
    /// @param bitstream_names Names of each bitstream for filenames and
    ///                         C headers
    /// @param output_path Output path to bitstream directory in the case
    ///                     of ASCII-style bitstream, path to single bitstream
    ///                     file otherwise
    /// @note If instructed to produce ASCII bitstreams, this function will
    ///         produce on bitstream per audio file in a directory specified
    ///         by the output path. For all other formats, the bitstream
    ///         will be a single file
    void encodeBatch(const std::vector<std::string> &audio_input_paths,
        const std::vector<std::string> &bitstream_names,
        const std::string &output_path) const;

 private:
    ///////////////////////////////////////////////////////////////////////////
    // Helpers ////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Converts audio file to sequence of LPC frames which characterize
    ///         the sample within each segmentation window
    /// @param path Path to audio file
    /// @return Vector of encoded frames
    std::vector<Frame> generateFrames(const std::string &path) const;

    /// @brief Converts Frame vector to bitstream file(s)
    /// @param frames Vector of Frames
    /// @param filename Name of bitstream, for C headers
    /// @return Bitstream, as a string
    std::string serializeFrames(const std::vector<Frame>& frames,
        const std::string &filename) const;

    ///////////////////////////////////////////////////////////////////////////
    // Members ////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Target bitstream format
    EncoderStyle style_;

    /// @brief Segmentation window width, in milliseconds
    float window_width_ms_;

    /// @brief Highpass filter cutoff, in Hertz
    int highpass_cutoff_hz_;

    /// @brief Lowpass filter cutoff, in Hertz
    int lowpass_cutoff_hz_;

    /// @brief Pre-emphasis filter coefficient
    float pre_emphasis_alpha_;

    /// @brief true if bitstream should end with explicit stop frame,
    ///         false otherwise
    bool include_stop_frame_;

    /// @brief Post-processing gain shift, as TMS5220 Coding Table index offset
    int gain_shift_;

    /// @brief Max gain for voiced (vowel) frames, in decibels
    float main_voiced_gain_db_;

    /// @brief Max gain for unvoiced (consonant) frames, in decibels
    float max_unvoiced_gain_db_;

    /// @brief true if bitstream should be "compressed" via detection of similar
    ///         frames, which are marked a repeats and fully encoded just once
    bool detect_repeat_frames_;

    /// @brief Max pitch frequency, in Hertz
    int max_pitch_hz_;

    /// @brief Min pitch frequency, in Hertz
    int min_pitch_hz_;
};

};  // namespace tms_express

#endif  //  TMS_EXPRESS_BITSTREAM_GENERATION_BITSTREAMGENERATOR_HPP_
