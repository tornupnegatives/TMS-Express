// Copyright (C) 2022-2024 Joseph Bellahcen <joeclb@icloud.com>

#include "ui/cli/SynthesizerCommand.hpp"

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

void SynthesizerCommand::setup(CLI::App* parent,
                               const std::shared_ptr<Application>& app) {
    auto* sub =
        parent->add_subcommand("synth", "Converts bitstream to audio file");

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

    // Callback ///////////////////////////////////////////////////////////////

    sub->callback([app, input_path, output_path, post_params]() {
        run(app, *input_path, *output_path, *post_params);
    });
}

int SynthesizerCommand::run(const std::shared_ptr<Application>& app,
                            const std::string& input_path,
                            const std::string& output_path,
                            const PostProcessorParameters& post_params) {
    // Open input and output files for inspection
    auto input = PathUtils(input_path);
    auto output = PathUtils(output_path);

    auto bitstream_path = input.getPaths().at(0);
    const std::string logger_id = "[synth:" + bitstream_path + "]:\t";

    auto frame_table = app->importBitstream(bitstream_path);
    app->postProcessFrameTable(&frame_table, post_params);
    auto samples = app->synthesizeFrameTable(frame_table);
    std::cout << logger_id << "Loaded " << samples.size() << " samples from "
              << frame_table.size() << " frames" << std::endl;

    auto audio_path = output.getPaths().at(0);
    AudioBuffer buffer(samples, kDefaultSampleRateHz, kDefaultWindowWidthMs);
    buffer.render(audio_path);
    return 0;
}

};  // namespace tms_express::ui
