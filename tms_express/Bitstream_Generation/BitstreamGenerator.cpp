// Copyright (C) 2023 Joseph Bellahcen <joeclb@icloud.com>

#include "Bitstream_Generation/BitstreamGenerator.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "Audio/AudioBuffer.hpp"
#include "Audio/AudioFilter.hpp"
#include "Frame_Encoding/Frame.hpp"
#include "Frame_Encoding/FrameEncoder.hpp"
#include "Frame_Encoding/FramePostprocessor.hpp"
#include "LPC_Analysis/Autocorrelation.hpp"
#include "LPC_Analysis/LinearPredictor.h"
#include "LPC_Analysis/PitchEstimator.h"

namespace tms_express {

BitstreamGenerator::BitstreamGenerator(float window_width_ms,
    int highpass_cutoff_hz, int lowpass_cutoff_hz, float pre_emphasis_alpha,
    EncoderStyle style, bool include_stop_frame, int gain_shift,
    float max_voiced_gain_db, float max_unvoiced_gain_db,
    bool detect_repeat_frames, int max_pitch_hz, int min_pitch_hz) {

    window_width_ms_ = window_width_ms;
    highpass_cutoff_hz_ = highpass_cutoff_hz;
    lowpass_cutoff_hz_ = lowpass_cutoff_hz;
    pre_emphasis_alpha_ = pre_emphasis_alpha;
    style_ = style;
    include_stop_frame_ = include_stop_frame;
    gain_shift_ = gain_shift;
    main_voiced_gain_db_ = max_voiced_gain_db;
    max_unvoiced_gain_db_ = max_unvoiced_gain_db;
    detect_repeat_frames_ = detect_repeat_frames;
    max_pitch_hz_ = max_pitch_hz;
    min_pitch_hz_ = min_pitch_hz;
}

void BitstreamGenerator::encode(const std::string &audio_input_path,
    const std::string &bitstream_name, const std::string &output_path) const {
    // Perform LPC analysis and convert audio data to a bitstream
    auto frames = generateFrames(audio_input_path);
    auto bitstream = serializeFrames(frames, bitstream_name);

    // Write bitstream to disk
    std::ofstream lpcOut;
    lpcOut.open(output_path);
    lpcOut << bitstream;
    lpcOut.close();
}

void BitstreamGenerator::encodeBatch(
    const std::vector<std::string> &audio_input_paths,
    const std::vector<std::string> &bitstream_names,
    const std::string &output_path) const {
    std::string in_path, filename;

    if (style_ == ENCODERSTYLE_ASCII) {
        // Create directory to populate with encoded files
        std::filesystem::create_directory(output_path);

        for (int i = 0; i < audio_input_paths.size(); i++) {
            in_path = audio_input_paths[i];
            filename = bitstream_names[i];

            std::filesystem::path out_path = output_path;
            out_path /= (filename + ".lpc");

            encode(in_path, filename, out_path.string());
        }
    } else {
        std::ofstream lpcOut;
        lpcOut.open(output_path);

        for (int i = 0; i < audio_input_paths.size(); i++) {
            in_path = audio_input_paths[i];
            filename = bitstream_names[i];

            auto frames = generateFrames(in_path);
            auto bitstream = serializeFrames(frames, filename);

            lpcOut << bitstream << std::endl;
        }

        lpcOut.close();
    }
}

std::vector<Frame> BitstreamGenerator::generateFrames(
    const std::string &path) const {
    // Mix audio to 8kHz mono and store in a segmented buffer
    // TODO(Joseph Bellahcen): Handle nullptr
    auto lpc_buffer = *AudioBuffer::Create(path, 8000, window_width_ms_);

    // Copy the buffer so that upper and lower vocal tract analysis may occur
    // separately
    auto pitch_buffer = lpc_buffer.copy();

    // Apply preprocessing
    //
    // The pitch buffer will ONLY be lowpass-filtered, as pitch is a
    // low-frequency component of the signal. Neither highpass filtering nor
    // pre-emphasis, which exaggerate high-frequency components, will improve
    // pitch estimation
    auto preprocessor = AudioFilter();
    preprocessor.applyPreEmphasis(lpc_buffer, pre_emphasis_alpha_);
    preprocessor.applyHighpass(lpc_buffer, highpass_cutoff_hz_);
    preprocessor.applyLowpass(pitch_buffer, lowpass_cutoff_hz_);

    // Extract buffer metadata
    //
    // Only the LPC buffer is queried for metadata, since it will have the same
    // number of samples as the pitch buffer. The sample rate of the buffer is
    // extracted despite being known, as future iterations of TMS Express may
    // support encoding 10kHz/variable sample rate audio for the TMS5200C
    auto n_segments = lpc_buffer.getNSegments();
    auto sample_rate = lpc_buffer.getSampleRateHz();

    // Initialize analysis objects and data structures
    auto linearPredictor = LinearPredictor();
    auto pitchEstimator = PitchEstimator(sample_rate, min_pitch_hz_,
        max_pitch_hz_);
    auto frames = std::vector<Frame>();

    for (int i = 0; i < n_segments; i++) {
        // Get segment for frame
        auto pitch_segment = pitch_buffer.getSegment(i);
        auto lpc_segment = lpc_buffer.getSegment(i);

        // Apply a window function to the segment to smoothen its boundaries
        //
        // Because information about the transition between adjacent frames is
        // lost during segmentation, a window will help produce smoother results
        preprocessor.applyHammingWindow(lpc_segment);

        // Compute the autocorrelation of each segment, which serves as the
        // basis of all analysis
        auto lpc_acf = tms_express::Autocorrelation(lpc_segment);
        auto pitch_acf = tms_express::Autocorrelation(pitch_segment);

        // Extract LPC reflector coefficients and compute the predictor gain
        auto coeffs = linearPredictor.reflectorCoefficients(lpc_acf);
        auto gain = linearPredictor.gain();

        // Estimate pitch
        auto pitch_period = pitchEstimator.estimatePeriod(pitch_acf);

        // Decide whether the segment is voiced or unvoiced
        auto segment_is_voiced = coeffs[0] < 0;

        // Store parameters in a Frame object
        auto frame = Frame(pitch_period, segment_is_voiced, gain, coeffs);
        frames.push_back(frame);
    }

    // Apply post-processing
    auto post_processor = FramePostprocessor(&frames, main_voiced_gain_db_,
        max_unvoiced_gain_db_);
    post_processor.normalizeGain();
    post_processor.shiftGain(gain_shift_);

    if (detect_repeat_frames_) {
        post_processor.detectRepeatFrames();
    }

    return frames;
}

std::string BitstreamGenerator::serializeFrames(
    const std::vector<Frame>& frames, const std::string &filename) const {
    // Encode frames to hex bitstreams
    auto encoder = FrameEncoder(frames, style_ != ENCODERSTYLE_ASCII);
    std::string bitstream;

    switch (style_) {
        case ENCODERSTYLE_ASCII:
            bitstream = encoder.toHex(include_stop_frame_);
            break;

        case ENCODERSTYLE_C:
            // C-style bitstreams are headers which contain a byte array
            // Format: const int bitstream_name [] = {<values>};
            bitstream = encoder.toHex(include_stop_frame_);
            bitstream = "const int " + filename + "[] = {" + bitstream + "};\n";
            break;

        case ENCODERSTYLE_ARDUINO:
            // Arduino-style bitstreams are C-style bitstreams which include the
            // Arduino header and PROGMEM keyword. This is for compatibility
            // with the Arduino Talkie library
            // Format: extern const uint8_t name [] PROGMEM = {<values>};
            bitstream = encoder.toHex(include_stop_frame_);
            bitstream = "extern const uint8_t " + filename + "[] PROGMEM = {" +
                bitstream + "};\n";
            break;

        case ENCODERSTYLE_JSON:
            bitstream = encoder.toJSON();
            break;
    }

    return bitstream;
}

};  // namespace tms_express
