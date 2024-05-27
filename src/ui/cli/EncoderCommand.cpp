// Copyright (C) 2022-2024 Joseph Bellahcen <joeclb@icloud.com>

#include "ui/cli/EncoderCommand.hpp"

#include <iostream>
#include <iterator>
#include <memory>
#include <string>
#include <vector>

#include "audio/AudioBuffer.hpp"
#include "bitstream/BitstreamGeneratorParameters.hpp"
#include "bitstream/PathUtils.hpp"
#include "ui/Application.hpp"

namespace tms_express::ui {

///////////////////////////////////////////////////////////////////////////////
// Interface //////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void EncoderCommand::setup(CLI::App* parent,
                           const std::shared_ptr<Application>& app) {
    auto* sub =
        parent->add_subcommand("encode", "Converts audio file to bitstream");

    // Parameter allocation ///////////////////////////////////////////////////

    auto* shared_params = app->getSharedParams();

    auto input_path = std::make_shared<std::string>();
    auto output_path = std::make_shared<std::string>();
    auto upper_params = std::make_shared<UpperVocalTractParameters>();
    auto lower_params = std::make_shared<LowerVocalTractParameters>();
    auto post_params = std::make_shared<PostProcessorParameters>();
    auto bitstream_params = std::make_shared<BitstreamParameters>();

    // Required parameters ////////////////////////////////////////////////////

    sub->add_option("-i,input", *input_path, "Path to audio file")
        ->required()
        ->check(CLI::ExistingFile);

    sub->add_option("-o,output", *output_path, "Path to output file")
        ->required();

    // Shared parameters //////////////////////////////////////////////////////

    sub->add_option("--samplerate", shared_params->sample_rate_hz,
                    "Audio sampling rate (Hz)")
        ->default_val(kDefaultSampleRateHz);

    sub->add_option("--window", shared_params->window_width_ms,
                    "Analysis window width (ms)")
        ->default_val(kDefaultWindowWidthMs);

    // Upper tract ////////////////////////////////////////////////////////////

    sub->add_option("--uhpf", upper_params->highpass_cutoff_hz,
                    "Highpass filter cutoff for upper tract (Hz)")
        ->default_val(kDefaultHighpassCutoffHz);

    sub->add_option("--ulpf", upper_params->lowpass_cutoff_hz,
                    "Lowpass filter cutoff for upper tract (Hz)")
        ->default_val(kDefaultLowpassCutoffHz);

    sub->add_option("--ualpha", upper_params->pre_emphasis_alpha,
                    "Pre-emphasis coeff for upper tract")
        ->default_val(kDefaultPreEmphasisAlpha);

    // Lower tract ////////////////////////////////////////////////////////////

    sub->add_option("--lhpf", lower_params->highpass_cutoff_hz,
                    "Highpass filter cutoff for lower tract (Hz)")
        ->default_val(kDefaultHighpassCutoffHz);

    sub->add_option("--llpf", lower_params->lowpass_cutoff_hz,
                    "Lowpass filter cutoff for lower tract (Hz)")
        ->default_val(kDefaultLowpassCutoffHz);

    sub->add_option("--lalpha", lower_params->pre_emphasis_alpha,
                    "Pre-emphasis coeff for lower tract")
        ->default_val(kDefaultPreEmphasisAlpha);

    sub->add_option("--minpitch", lower_params->min_pitch_hz,
                    "Pitch estimate floor (Hz)")
        ->default_val(kDefaultMinPitchHz);

    sub->add_option("--maxpitch", lower_params->max_pitch_hz,
                    "Pitch estimate ceiling (Hz)")
        ->default_val(kDefaultMaxPitchHz);

    // Post-processor /////////////////////////////////////////////////////////

    sub->add_option("--gainshift", post_params->gain_shift, "Gain shift")
        ->default_val(kDefaultGainShift);

    sub->add_option("--gainnorm", post_params->normalize_gain, "Gain normalize")
        ->default_val(kDefaultNormalizeGain);

    sub->add_option("--voicedgain", post_params->max_voiced_gain_db,
                    "Voiced gain estimate ceiling (dB)")
        ->default_val(kDefaultVoicedGainDb);

    sub->add_option("--unvoicedgain", post_params->max_unvoiced_gain_db,
                    "Unvoiced gain estimate ceiling (dB)")
        ->default_val(kDefaultUnvoicedGainDb);

    sub->add_option("--repeat", post_params->detect_repeat_frames,
                    "Detect repeat frames")
        ->default_val(kDefaultDetectRepeatFrames);

    // Bitstream //////////////////////////////////////////////////////////////

    sub->add_option("--format", bitstream_params->encoder_style,
                    "Bitstream format")
        ->default_val(kDefaultStyle);

    sub->add_option("--stopframe", bitstream_params->include_stop_frame,
                    "Add stop frame")
        ->default_val(kDefaultIncludeStopFrame);

    // Callback ///////////////////////////////////////////////////////////////

    sub->callback([app, input_path, output_path, upper_params, lower_params,
                   post_params, bitstream_params]() {
        run(app, *input_path, *output_path, *upper_params, *lower_params,
            *post_params, *bitstream_params);
    });
}

int EncoderCommand::run(const std::shared_ptr<Application>& app,
                        const std::string& input_path,
                        const std::string& output_path,
                        const UpperVocalTractParameters& upper_params,
                        const LowerVocalTractParameters& lower_params,
                        const PostProcessorParameters& post_params,
                        const BitstreamParameters& bitstream_params) {
    // Open input and output files for inspection
    auto input = PathUtils(input_path);
    auto output = PathUtils(output_path);

    if (input.isDirectory() &&
        (bitstream_params.encoder_style == ENCODER_STYLE_ASCII) &&
        (!output.isDirectory() && output.exists())) {
        std::cerr << "Batch mode requires a directory for ASCII bitstreams"
                  << std::endl;

        return 0;
    }

    if (!input.isDirectory() && output.isDirectory()) {
        std::cerr << "Single-file encode requires a single-file output"
                  << std::endl;
        return 1;
    }

    auto audio_input = input.getPaths().at(0);

    const std::string logger_id = "[encoder:" + audio_input + "]:\t";

    auto buffer =
        AudioBuffer::Create(audio_input, app->getSharedParams()->sample_rate_hz,
                            app->getSharedParams()->window_width_ms);

    std::cout << logger_id << "Loaded " << buffer->getSamples().size()
              << " samples"
              << " (" << buffer->getSampleRateHz() << " Hz)" << std::endl;

    auto pitch_table =
        app->analyzeLowerVocalTract(lower_params, buffer->getSamples());
    auto [coeff_table, gain_table] =
        app->analyzeUpperVocalTract(upper_params, buffer->getSamples());
    auto voicing_table = app->estimateVoicing(coeff_table);

    auto frame_table = app->buildFrameTable(pitch_table, coeff_table,
                                            gain_table, voicing_table);

    app->postProcessFrameTable(&frame_table, post_params);

    std::cout << logger_id << "Packed " << frame_table.size() << " frames"
              << std::endl;

    auto bitstream_path = output.getPaths().at(0);
    app->exportBitstream(frame_table, bitstream_params, bitstream_path);
    return 0;
}

};  // namespace tms_express::ui
