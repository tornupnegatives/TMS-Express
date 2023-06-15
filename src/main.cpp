// Author: Joseph Bellahcen <joeclb@icloud.com>

#include "CLI/CLI.hpp"
#include "Interfaces/BitstreamGenerator.h"
#include "Interfaces/PathUtils.h"
#include <filesystem>
#include <string>

int main(int argc, char **argv) {
    CLI::App appMain;
    CLI::App* appEncode = appMain.add_subcommand("encode", "Convert audio file(s) to TMS5220 bitstream(s)");

    appMain.require_subcommand(1);

    // Encoder parameters
    std::string inputPath;
    float windowWidthMs = 25.0f;
    int highpassCutoff = 1000;
    int lowpassCutoff = 800;
    float preEmphasisAlpha = -0.9375f;
    BitstreamGenerator::EncoderStyle bitstreamFormat = BitstreamGenerator::EncoderStyle::ENCODERSTYLE_ASCII;
    bool noStopFrame = false;
    int gainShift = 2;
    float maxVoicedGain = 37.5f;
    float maxUnvoicedGain = 30.0f;
    bool useRepeatFrames = false;
    int maxPitchFrq = 500;
    int minPitchFrq = 50;
    std::string outputPath;

    appEncode->add_option("-i,--input,input", inputPath, "Path to audio file")->required();
    appEncode->add_option("-w,--window", windowWidthMs, "Window width/speed (ms)");
    appEncode->add_option("-b,--highpass", highpassCutoff, "Highpass filter cutoff for upper tract analysis (Hz)");
    appEncode->add_option("-l,--lowpass", lowpassCutoff, "Lowpass filter cutoff for lower tract analysis (Hz)");
    appEncode->add_option("-a,--alpha", preEmphasisAlpha, "Pre-emphasis filter coefficient for upper tract analysis");
    appEncode->add_option("-f,--format", bitstreamFormat, "Bitstream format: ascii (0), c (1), arduino (2), JSON (3)")->check(CLI::Range(0, 3));
    appEncode->add_flag("-n,--no-stop-frame", noStopFrame, "Do not end bitstream with stop frame");
    appEncode->add_option("-g,--gain-shift", gainShift, "Quantized gain shift");
    appEncode->add_option("-v,--max-voiced-gain", maxVoicedGain, "Max voiced/vowel gain (dB)");
    appEncode->add_option("-u,--max-unvoiced-gain", maxUnvoicedGain, "Max unvoiced/consonant gain (dB)");
    appEncode->add_flag("-r,--use-repeat-frames", useRepeatFrames, "Compress bitstream by detecting and repeating similar frames");
    appEncode->add_option("-M,--max-pitch", maxPitchFrq, "Max pitch frequency (Hz)");
    appEncode->add_option("-m,--min-pitch", minPitchFrq, "Min pitch frequency (Hz)");
    appEncode->add_option("-o,--output,output", outputPath, "Path to output file")->required();

    CLI11_PARSE(appMain, argc, argv);

    if (appMain.got_subcommand(appEncode)) {
        // Open input and output files for inspection
        auto input = PathUtils(inputPath);
        auto output = PathUtils(outputPath);

        if (!input.fileExists()) {
            std::cerr << "Input file does not exist or is empty" << std::endl;
            exit(EXIT_FAILURE);
        }

        if (input.isDirectory() && !bitstreamFormat && (!output.isDirectory() && output.fileExists())) {
            std::cerr << "Batch mode requires a directory for ASCII bitstreams" << std::endl;
            exit(EXIT_FAILURE);
        }

        if (!input.isDirectory() && output.isDirectory()) {
            std::cerr << "Single-file encode requires a single-file output" << std::endl;
            exit(EXIT_FAILURE);
        }

        // Extract IO paths and encode
        auto encoder = BitstreamGenerator(windowWidthMs, highpassCutoff, lowpassCutoff,
                                          preEmphasisAlpha, bitstreamFormat, !noStopFrame,
                                          gainShift, maxVoicedGain,maxUnvoicedGain,
                                          useRepeatFrames, maxPitchFrq,minPitchFrq);
        auto inputPaths = input.getPaths();
        auto inputFilenames = input.getFilenames();
        auto outputPathOrDirectory = output.getPaths().at(0);

        try {
            if (input.isDirectory()) {
                encoder.encodeBatch(inputPaths, inputFilenames, outputPathOrDirectory);
            } else {
                encoder.encode(inputPaths.at(0), inputFilenames.at(0), outputPathOrDirectory);
            }
        } catch (const std::exception &e) {
            std::cerr << "Error: " << e.what() << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    exit(EXIT_SUCCESS);
}
