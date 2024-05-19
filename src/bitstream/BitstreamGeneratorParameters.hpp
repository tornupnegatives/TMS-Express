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
    ENCODER_STYLE_JSON,

    //ENCODER_STYLE_BIN
};

/// @brief Supported LPC model order (number of coefficients)
enum ModelOrder { MODEL_ORDER_10 = 10 };

/// @brief Supported sample rates (in Hertz)
enum SampleRate { SAMPLE_RATE_8KHZ = 8000 };

///////////////////////////////////////////////////////////////////////////////
// Constants //////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/// @brief Instructs bitstream generator to ignore a given parameter
static const int kDisableParameter = -1;

// Defaults
static const SampleRate kDefaultSampleRateHz = SAMPLE_RATE_8KHZ;
static const float kDefaultWindowWidthMs = 25.0F;
static const int kDefaultHighpassCutoffHz = kDisableParameter;
static const int kDefaultLowpassCutoffHz = kDisableParameter;
static const float kDefaultPreEmphasisAlpha = kDisableParameter;
static const int kDefaultMaxPitchHz = 500;
static const int kDefaultMinPitchHz = 50;
static const ModelOrder kDefaultModelOrder = MODEL_ORDER_10;
static const EncoderStyle kDefaultStyle = ENCODER_STYLE_ASCII;
static const bool kDefaultIncludeStopFrame = true;
static const int kDefaultGainShift = 0;
static const bool kDefaultNormalizeGain = true;
static const float kDefaultUnvoicedGainDb = 30.0F;
static const float kDefaultVoicedGainDb = 37.5F;
static const bool kDefaultDetectRepeatFrames = false;

///////////////////////////////////////////////////////////////////////////////
// Parameter Structs //////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/// @brief Defines parameters which must match for all analysis structures
struct SharedParameters {
    /// @brief Sampling rate of source audio, in Hertz
    SampleRate sample_rate_hz = kDefaultSampleRateHz;

    /// @brief Segmentation/analysis window width (frame length) in milliseconds
    float window_width_ms = kDefaultWindowWidthMs;
};

/// @brief Defines upper vocal tract (LPC analysis) parameters
struct UpperVocalTractParameters {
    int highpass_cutoff_hz = kDefaultHighpassCutoffHz;
    int lowpass_cutoff_hz = kDefaultLowpassCutoffHz;
    float pre_emphasis_alpha = kDefaultPreEmphasisAlpha;
    int model_order = kDefaultModelOrder;
};

/// @brief Defines lower vocal tract (pitch and voicing analysis) parameters
struct LowerVocalTractParameters {
    int highpass_cutoff_hz = kDefaultHighpassCutoffHz;
    int lowpass_cutoff_hz = kDefaultLowpassCutoffHz;
    float pre_emphasis_alpha = kDefaultPreEmphasisAlpha;
    int max_pitch_hz = kDefaultMaxPitchHz;
    int min_pitch_hz = kDefaultMinPitchHz;
};

struct BitstreamParameters {
    EncoderStyle encoder_style = kDefaultStyle;
    bool include_stop_frame = kDefaultIncludeStopFrame;
};

struct PostProcessorParameters {
    int gain_shift = kDefaultGainShift;
    bool normalize_gain = kDefaultNormalizeGain;
    float max_voiced_gain_db = kDefaultUnvoicedGainDb;
    float max_unvoiced_gain_db = kDefaultVoicedGainDb;
    bool detect_repeat_frames = kDefaultDetectRepeatFrames;
};

};  //  namespace tms_express

#endif  //  TMS_EXPRESS_SRC_BITSTREAM_BITSTREAMGENERATORPARAMETERS_HPP_
