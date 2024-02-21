// Copyright 2023 Joseph Bellahcen <joeclb@icloud.com>

#include "ui/cli/CommandLineApp.hpp"

#include "lib/CLI11.hpp"

#include "bitstream/BitstreamGenerator.hpp"
#include "bitstream/PathUtils.hpp"

namespace tms_express::ui {

///////////////////////////////////////////////////////////////////////////////
// Initializers ///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

CommandLineApp::CommandLineApp() {
    encoder = add_subcommand("encode",
        "Converts audio file(s) to TMS5220 bitstream(s)");

    require_subcommand(1);
    setupEncoder();
}

///////////////////////////////////////////////////////////////////////////////
// Interface //////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

int CommandLineApp::run(int argc, char** argv) {
    try {
        parse(argc, argv);
    } catch(const CLI::ParseError &e) {
        return this->exit(e);
    }

    if (got_subcommand(encoder)) {
        // Open input and output files for inspection
        auto input = PathUtils(input_path_);
        auto output = PathUtils(output_path_);

        if (!input.exists()) {
            std::cerr << "Input file does not exist or is empty" << std::endl;
            return 1;
        }

        if (input.isDirectory() && !bitstream_format_ &&
            (!output.isDirectory() && output.exists())) {
            std::cerr <<
                "Batch mode requires a directory for ASCII bitstreams" <<
                std::endl;

            return 0;
        }

        if (!input.isDirectory() && output.isDirectory()) {
            std::cerr <<
                "Single-file encode requires a single-file output" << std::endl;
            return 1;
        }

        // Extract IO paths and encode
        auto bitstream_generator = BitstreamGenerator(analysis_window_ms_,
            hpf_cutoff_, lpf_cutoff_, preemphasis_alpha_, bitstream_format_,
            !no_stop_frame_, gain_shift_, max_voiced_gain_, max_unvoiced_gain_,
            repeat_frames_, max_pitch_frq_, min_pitch_frq_);

        auto input_paths = input.getPaths();
        auto input_filenames = input.getFilenames();
        auto output_path_directory = output.getPaths().at(0);

        try {
            if (input.isDirectory()) {
                bitstream_generator.encodeBatch(input_paths, input_filenames,
                    output_path_directory);

            } else {
                bitstream_generator.encode(input_paths.at(0),
                    input_filenames.at(0), output_path_directory);
            }
        } catch (const std::exception &e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return 1;
        }
    }

    return 0;
}

///////////////////////////////////////////////////////////////////////////////
// Helper Methods /////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void CommandLineApp::setupEncoder() {
    encoder->add_option("-i,--input,input", input_path_,
        "Path to audio file")->required();

    encoder->add_option("-w,--window", analysis_window_ms_,
        "Window width/speed (ms)");

    encoder->add_option("-b,--highpass", hpf_cutoff_,
        "Highpass filter cutoff for upper tract analysis (Hz)");

    encoder->add_option("-l,--lowpass", lpf_cutoff_,
        "Lowpass filter cutoff for lower tract analysis (Hz)");

    encoder->add_option("-a,--alpha", preemphasis_alpha_,
        "Pre-emphasis filter coefficient for upper tract analysis");

    encoder->add_option("-f,--format", bitstream_format_,
        "Bitstream format: ascii (0), c (1), arduino (2), JSON (3)")->
        check(CLI::Range(0, 3));

    encoder->add_flag("-n,--no-stop-frame", no_stop_frame_,
        "Do not end bitstream with stop frame");

    encoder->add_option("-g,--gain-shift", gain_shift_,
        "Quantized gain shift");

    encoder->add_option("-v,--max-voiced-gain", max_voiced_gain_,
        "Max voiced/vowel gain (dB)");

    encoder->add_option("-u,--max-unvoiced-gain", max_unvoiced_gain_,
        "Max unvoiced/consonant gain (dB)");

    encoder->add_flag("-r,--use-repeat-frames", repeat_frames_,
        "Compress bitstream by detecting and repeating similar frames");

    encoder->add_option("-M,--max-pitch", max_pitch_frq_,
        "Max pitch frequency (Hz)");

    encoder->add_option("-m,--min-pitch", min_pitch_frq_,
        "Min pitch frequency (Hz)");

    encoder->add_option("-o,--output,output", output_path_,
        "Path to output file")->required();
}

};  // namespace tms_express::ui
