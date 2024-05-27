// Copyright 2023-2024 Joseph Bellahcen <joeclb@icloud.com>

#include "ui/Application.hpp"

#include <cstddef>
#include <fstream>
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
#include "encoding/Synthesizer.hpp"

namespace tms_express {

///////////////////////////////////////////////////////////////////////////////
// Initializers ///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Application::Application(SharedParameters params) : shared_params_(params) {}

///////////////////////////////////////////////////////////////////////////////
// Bitstream I/O //////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

std::vector<Frame> Application::importBitstream(const std::string& path) {
    std::ifstream file(path);
    const auto flat = std::string((std::istreambuf_iterator<char>(file)),
                                  std::istreambuf_iterator<char>());

    auto frame_encoder = FrameEncoder();
    frame_encoder.importASCIIFromString(flat);
    return frame_encoder.getFrameTable();
}

void Application::exportBitstream(const std::vector<Frame>& frame_table,
                                  BitstreamParameters params,
                                  const std::string& path) {
    auto bitstream = serializeFrameTable(frame_table, params);

    std::ofstream lpcOut;
    lpcOut.open(path);
    lpcOut << bitstream;
    lpcOut.close();
}

///////////////////////////////////////////////////////////////////////////////
// LPC Analysis ///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

std::vector<int> Application::analyzeLowerVocalTract(
    LowerVocalTractParameters params, std::vector<float> samples) const {
    auto buffer = AudioBuffer(std::move(samples), shared_params_.sample_rate_hz,
                              shared_params_.window_width_ms);
    auto table = std::vector<int>(buffer.getNSegments());

    // Apply preprocessing
    //
    // Pitch estimation will likely only benefit from lowpass filtering, as
    // pitch is a low-frequency component of the signal
    auto filter = AudioFilter();

    if (params.pre_emphasis_alpha != kDisableParameter) {
        filter.applyPreEmphasis(buffer, params.pre_emphasis_alpha);
    }

    if (params.highpass_cutoff_hz != kDisableParameter) {
        filter.applyHighpass(buffer, params.highpass_cutoff_hz);
    }

    if (params.lowpass_cutoff_hz != kDisableParameter) {
        filter.applyLowpass(buffer, params.lowpass_cutoff_hz);
    }

    // Configure estimator
    auto estimator = PitchEstimator(shared_params_.sample_rate_hz,
                                    params.min_pitch_hz, params.max_pitch_hz);

    // Build estimate table
    for (int i = 0; i < static_cast<int>(table.size()); i++) {
        auto segment = buffer.getSegment(i);
        auto acf = tms_express::Autocorrelation(segment);
        table.at(i) = estimator.estimatePeriod(acf);
    }

    return table;
}

std::tuple<std::vector<std::vector<float>>, std::vector<float>>
Application::analyzeUpperVocalTract(UpperVocalTractParameters params,
                                    std::vector<float> samples) const {
    auto buffer = AudioBuffer(std::move(samples), shared_params_.sample_rate_hz,
                              shared_params_.window_width_ms);
    auto coeff_table = std::vector<std::vector<float>>(buffer.getNSegments());
    auto gain_table = std::vector<float>(buffer.getNSegments());

    // Apply pre-processing
    auto filter = AudioFilter();

    if (params.pre_emphasis_alpha != kDisableParameter) {
        filter.applyPreEmphasis(buffer, params.pre_emphasis_alpha);
    }

    if (params.highpass_cutoff_hz != kDisableParameter) {
        filter.applyHighpass(buffer, params.highpass_cutoff_hz);
    }

    if (params.lowpass_cutoff_hz != kDisableParameter) {
        filter.applyLowpass(buffer, params.lowpass_cutoff_hz);
    }

    // Configure estimator
    auto estimator = LinearPredictor(params.model_order);

    // Build estimate table
    for (int i = 0; i < static_cast<int>(buffer.getNSegments()); i++) {
        auto segment = buffer.getSegment(i);

        // Apply a window function to the segment to smoothen its boundaries
        //
        // Because information about the transition between adjacent frames is
        // lost during segmentation, a window will help produce smoother results
        filter.applyHammingWindow(segment);

        auto acf = tms_express::Autocorrelation(segment);
        coeff_table.at(i) = estimator.computeCoeffs(acf);
        gain_table.at(i) = estimator.gain();
    }

    return {coeff_table, gain_table};
}

std::vector<bool> Application::estimateVoicing(
    const std::vector<std::vector<float>>& coeff_table) {
    auto voicing_table = std::vector<bool>(coeff_table.size());

    for (int i = 0; i < static_cast<int>(coeff_table.size()); i++) {
        voicing_table.at(i) = coeff_table.at(i).at(0) > 0 ? kUnvoiced : kVoiced;
    }

    return voicing_table;
}

///////////////////////////////////////////////////////////////////////////////
// Frame Table ////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

std::vector<Frame> Application::buildFrameTable(
    std::vector<int> pitch_table, std::vector<std::vector<float>> coeff_table,
    std::vector<float> gain_table, std::vector<bool> voicing_table) const {
    // TODO(Joseph Bellahcen): Ensure all tables are the same size, then
    // pre-allocate the frame table
    auto frame_table = std::vector<Frame>();
    for (size_t i = 0; i < gain_table.size(); i++) {
        auto frame = Frame(pitch_table.at(i), voicing_table.at(i),
                           gain_table.at(i), coeff_table.at(i));
        frame_table.push_back(frame);
    }

    return frame_table;
}

void Application::postProcessFrameTable(std::vector<Frame>* frame_table,
                                        PostProcessorParameters params) const {
    auto post_processor = FramePostprocessor(
        frame_table, params.max_voiced_gain_db, params.max_unvoiced_gain_db);

    if (params.normalize_gain) {
        post_processor.normalizeGain();
    }

    if (params.gain_shift != kDisableParameter) {
        post_processor.shiftGain(params.gain_shift);
    }

    if (params.detect_repeat_frames) {
        post_processor.detectRepeatFrames();
    }
}

std::string Application::serializeFrameTable(
    const std::vector<Frame>& frame_table, BitstreamParameters params) {
    auto encoder = FrameEncoder(frame_table);
    std::string bitstream;

    const std::string name = "DATA";

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

///////////////////////////////////////////////////////////////////////////
// Synthesis //////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

std::vector<float> Application::synthesizeFrameTable(
    const std::vector<Frame>& frame_table) const {
    auto synthesizer = Synthesizer(shared_params_.sample_rate_hz,
                                   shared_params_.window_width_ms);
    return synthesizer.synthesize(frame_table);
}

///////////////////////////////////////////////////////////////////////////////
// Accessors //////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

SharedParameters* Application::getSharedParams() {
    return &shared_params_;
}

};  // namespace tms_express
