// Copyright (C) 2024 Joseph Bellahcen <joeclb@icloud.com>

#ifndef TMS_EXPRESS_SRC_BITSTREAM_BITSTREAMGENERATORPARAMETERS_HPP_
#define TMS_EXPRESS_SRC_BITSTREAM_BITSTREAMGENERATORPARAMETERS_HPP_

namespace tms_express {

///////////////////////////////////////////////////////////////////////////////
// Enums //////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/// @brief Defines the format of the bitstream
enum EncoderStyle {
    /// @brief Bitstream with comma-delimited ASCII hex bytes
    ENCODER_STYLE_ASCII,

    /// @brief Bitstream as C header which defines array of bytes
    ENCODER_STYLE_C,

    /// @brief Bitstream as C header which defines array of bytes in PROGMEM
    ENCODER_STYLE_C_ARDUINO,

    /// @brief Bitstream as JSON file
    ENCODER_STYLE_JSON
};

///////////////////////////////////////////////////////////////////////////////
// Parameter Structs //////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/// @brief Defines parameters which must match for all analysis structures
struct SharedParameters {
    /// @brief Sampling rate of source audio, in Hertz
    int sample_rate_hz;

    /// @brief Segmentation/analysis window width (frame length) in milliseconds
    float window_width_ms;
};

/// @brief Defines upper vocal tract (LPC analysis) parameters
struct UpperVocalTractParameters {
    int highpass_cutoff_hz;
    int lowpass_cutoff_hz;
    float pre_emphasis_alpha;
};

/// @brief Defines lower vocal tract (pitch and voicing analysis) parameters
struct LowerVocalTractParameters {
    int highpass_cutoff_hz;
    int lowpass_cutoff_hz;
    float pre_emphasis_alpha;
    int max_pitch_hz;
    int min_pitch_hz;
};

struct BitstreamParameters {
    EncoderStyle encoder_style;
    bool include_stop_frame;
};

struct PostProcessorParameters {
    int gain_shift;
    float max_voiced_gain_db;
    float max_unvoiced_gain_db;
    bool detect_repeat_frames;
};

};  //  namespace tms_express

#endif  //  TMS_EXPRESS_SRC_BITSTREAM_BITSTREAMGENERATORPARAMETERS_HPP_
