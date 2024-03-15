// Copyright (C) 2022-2024 Joseph Bellahcen <joeclb@icloud.com>

#include "bitstream/BitstreamGenerator.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "analysis/Autocorrelation.hpp"
#include "analysis/LinearPredictor.hpp"
#include "analysis/PitchEstimator.hpp"
#include "audio/AudioBuffer.hpp"
#include "audio/AudioFilter.hpp"
#include "bitstream/BitstreamGeneratorParameters.hpp"
#include "encoding/Frame.hpp"
#include "encoding/FrameEncoder.hpp"
#include "encoding/FramePostprocessor.hpp"

namespace tms_express {

///////////////////////////////////////////////////////////////////////////////
// Initializers ///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

BitstreamGenerator::BitstreamGenerator(SharedParameters params)
    : shared_params_(params) {}

///////////////////////////////////////////////////////////////////////////////
// Analysis ///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

std::vector<int> BitstreamGenerator::analyzeLowerTract(
    LowerVocalTractParameters params) {
    auto buffer =
        *AudioBuffer::Create(input_path_, shared_params_.sample_rate_hz,
                             shared_params_.window_width_ms);

    // Apply preprocessing
    //
    // Pitch estimation will likely only benefit from lowpass filtering, as
    // pitch is a low-frequency component of the signal
    auto preprocessor = AudioFilter();
    preprocessor.applyPreEmphasis(buffer, params.pre_emphasis_alpha);
    preprocessor.applyHighpass(buffer, params.highpass_cutoff_hz);
    preprocessor.applyLowpass(buffer, params.lowpass_cutoff_hz);

    // Extract buffer metadata
    const auto n_segments = buffer.getNSegments();
    const auto sample_rate = buffer.getSampleRateHz();

    // Initialize analysis objects and data structures
    auto pitch_estimator =
        PitchEstimator(sample_rate, params.max_pitch_hz, params.max_pitch_hz);
    auto pitch_table = std::vector<int>(n_segments);

    for (int i = 0; i < n_segments; i++) {
        auto segment = buffer.getSegment(i);
        auto acf = tms_express::Autocorrelation(segment);
        auto pitch_period = pitch_estimator.estimatePeriod(acf);

        pitch_table.at(i) = (pitch_period);
    }

    return pitch_table;
}

std::tuple<std::vector<std::vector<float>>, std::vector<float>>
BitstreamGenerator::analyzeUpperTract(UpperVocalTractParameters params) {
    auto buffer =
        *AudioBuffer::Create(input_path_, shared_params_.sample_rate_hz,
                             shared_params_.window_width_ms);

    // Apply preprocessing
    auto preprocessor = AudioFilter();
    preprocessor.applyPreEmphasis(buffer, params.pre_emphasis_alpha);
    preprocessor.applyHighpass(buffer, params.highpass_cutoff_hz);
    preprocessor.applyLowpass(buffer, params.lowpass_cutoff_hz);

    // Extract buffer metadata
    const auto n_segments = buffer.getNSegments();

    // Initialize analysis objects and data structures
    auto linear_predictor = LinearPredictor();
    auto coeff_table = std::vector<std::vector<float>>(n_segments);
    auto gain_table = std::vector<float>(n_segments);

    for (int i = 0; i < n_segments; i++) {
        auto segment = buffer.getSegment(i);

        // Apply a window function to the segment to smoothen its boundaries
        //
        // Because information about the transition between adjacent frames is
        // lost during segmentation, a window will help produce smoother results
        preprocessor.applyHammingWindow(segment);

        // Compute the autocorrelation of each segment, which serves as the
        // basis of all analysis
        auto acf = tms_express::Autocorrelation(segment);

        // Extract LPC reflector coefficients and compute the predictor gain
        auto coeffs = linear_predictor.computeCoeffs(acf);
        auto gain = linear_predictor.gain();

        coeff_table.at(i) = coeffs;
        gain_table.at(i) = gain;
    }

    return {coeff_table, gain_table};
}

std::vector<bool> BitstreamGenerator::estimateVoicing(
    const std::vector<std::vector<float>>& coeff_table) {
    auto voicing_table = std::vector<bool>(coeff_table.size());

    for (int i = 0; i < static_cast<int>(coeff_table.size()); i++) {
        voicing_table.at(i) = coeff_table.at(i).at(0) < 0;
    }

    return voicing_table;
}

///////////////////////////////////////////////////////////////////////////////
// Encoding ///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void BitstreamGenerator::applyPostProcessing(std::vector<Frame>* frame_table,
                                             PostProcessorParameters params) {
    auto post_processor = FramePostprocessor(
        frame_table, params.max_voiced_gain_db, params.max_unvoiced_gain_db);
    post_processor.normalizeGain();
    post_processor.shiftGain(params.gain_shift);

    if (params.detect_repeat_frames) {
        post_processor.detectRepeatFrames();
    }
}

std::string BitstreamGenerator::serializeFrames(
    const std::string& name, const std::vector<Frame>& frame_table,
    BitstreamParameters params) {
    auto encoder =
        FrameEncoder(frame_table, params.encoder_style != ENCODER_STYLE_ASCII);
    std::string bitstream;

    switch (params.encoder_style) {
        case ENCODER_STYLE_ASCII:
            bitstream = encoder.toHex(params.include_stop_frame);
            break;

        case ENCODER_STYLE_C:
            // C-style bitstreams are headers which contain a byte array
            // Format: const int bitstream_name [] = {<values>};
            bitstream = encoder.toHex(params.include_stop_frame);
            bitstream = "const int " + name + "[] = {" + bitstream + "};\n";
            break;

        case ENCODER_STYLE_C_ARDUINO:
            // Arduino-style bitstreams are C-style bitstreams which include the
            // Arduino header and PROGMEM keyword. This is for compatibility
            // with the Arduino Talkie library
            // Format: extern const uint8_t name [] PROGMEM = {<values>};
            bitstream = encoder.toHex(params.include_stop_frame);
            bitstream = "extern const uint8_t " + name + "[] PROGMEM = {" +
                        bitstream + "};\n";
            break;

        case ENCODER_STYLE_JSON:
            bitstream = encoder.toJSON();
            break;
    }

    return bitstream;
}

///////////////////////////////////////////////////////////////////////////////
// Accessors //////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

std::string BitstreamGenerator::getInputPath() const {
    return input_path_;
}

void BitstreamGenerator::setInputPath(const std::string& path) {
    input_path_ = path;
}

};  // namespace tms_express
