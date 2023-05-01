// Author: Joseph Bellahcen <joeclb@icloud.com>

#include "Bitstream_Generation/BitstreamGenerator.h"
#include "Bitstream_Generation/PathUtils.h"
#include "User_Interfaces/CommandLineApp.h"
#include <CLI11.hpp>

CommandLineApp::CommandLineApp() {
    encoder = add_subcommand("encode", "Convert audio file(s) to TMS5220 bitstream(s)");
    require_subcommand(1);

    setupEncoder();
}

int CommandLineApp::run(int argc, char** argv) {
    try {
        parse(argc, argv);
    } catch(const CLI::ParseError &e) {
        return this->exit(e);
    }

    if (got_subcommand(encoder)) {
        // Open input and output files for inspection
        auto input = PathUtils(inputPath);
        auto output = PathUtils(outputPath);

        if (!input.fileExists()) {
            std::cerr << "Input file does not exist or is empty" << std::endl;
            return 1;
        }

        if (input.isDirectory() && !bitstreamFormat && (!output.isDirectory() && output.fileExists())) {
            std::cerr << "Batch mode requires a directory for ASCII bitstreams" << std::endl;
            return 0;
        }

        if (!input.isDirectory() && output.isDirectory()) {
            std::cerr << "Single-file encode requires a single-file output" << std::endl;
            return 1;
        }

        // Extract IO paths and encode
        auto bitGen = BitstreamGenerator(windowWidthMs, highpassCutoff, lowpassCutoff,
                                          preEmphasisAlpha, bitstreamFormat, !noStopFrame,
                                          gainShift, maxVoicedGain,maxUnvoicedGain,
                                          useRepeatFrames, maxPitchFrq,minPitchFrq);
        auto inputPaths = input.getPaths();
        auto inputFilenames = input.getFilenames();
        auto outputPathOrDirectory = output.getPaths().at(0);

        try {
            if (input.isDirectory()) {
                bitGen.encodeBatch(inputPaths, inputFilenames, outputPathOrDirectory);
            } else {
                bitGen.encode(inputPaths.at(0), inputFilenames.at(0), outputPathOrDirectory);
            }
        } catch (const std::exception &e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return 1;
        }
    }

    return 0;
}

void CommandLineApp::setupEncoder() {
    encoder->add_option("-i,--input,input", inputPath, "Path to audio file")->required();
    encoder->add_option("-w,--window", windowWidthMs, "Window width/speed (ms)");
    encoder->add_option("-b,--highpass", highpassCutoff, "Highpass filter cutoff for upper tract analysis (Hz)");
    encoder->add_option("-l,--lowpass", lowpassCutoff, "Lowpass filter cutoff for lower tract analysis (Hz)");
    encoder->add_option("-a,--alpha", preEmphasisAlpha, "Pre-emphasis filter coefficient for upper tract analysis");
    encoder->add_option("-f,--format", bitstreamFormat, "Bitstream format: ascii (0), c (1), arduino (2), JSON (3)")->check(CLI::Range(0, 3));
    encoder->add_flag("-n,--no-stop-frame", noStopFrame, "Do not end bitstream with stop frame");
    encoder->add_option("-g,--gain-shift", gainShift, "Quantized gain shift");
    encoder->add_option("-v,--max-voiced-gain", maxVoicedGain, "Max voiced/vowel gain (dB)");
    encoder->add_option("-u,--max-unvoiced-gain", maxUnvoicedGain, "Max unvoiced/consonant gain (dB)");
    encoder->add_flag("-r,--use-repeat-frames", useRepeatFrames, "Compress bitstream by detecting and repeating similar frames");
    encoder->add_option("-M,--max-pitch", maxPitchFrq, "Max pitch frequency (Hz)");
    encoder->add_option("-m,--min-pitch", minPitchFrq, "Min pitch frequency (Hz)");
    encoder->add_option("-o,--output,output", outputPath, "Path to output file")->required();
}
