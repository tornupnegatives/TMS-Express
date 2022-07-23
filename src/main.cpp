
#include "Interfaces/BitstreamGenerator.h"

#include "CLI/CLI.hpp"

#include <iostream>
#include <fstream>
#include <string>

int main(int argc, char **argv) {
    CLI::App appMain;
    CLI::App* appEncode = appMain.add_subcommand("encode", "Convert single audio file to TMS5220 bitstream");

    appMain.require_subcommand(1);

    // Encoder parameters
    std::string inputPath;
    float windowWidthMs = 25.0f;
    int highpassCutoff = 600;
    int lowpassCutoff = 400;
    float preEmphasisAlpha = -0.9375f;
    EncoderStyle bitstreamFormat = ENCODERSTYLE_ASCII;
    char lpcSeparator = ',';
    bool noStopFrame = false;
    int gainShift = 2;
    float maxVoicedGain = 37.5f;
    float maxUnvoicedGain = 30.0f;
    bool useRepeatFrames = false;
    int maxPitchFrq = 500;
    int minPitchFrq = 50;
    std::string outputPath;

    // Parse command-line options and flags
    appEncode->add_option("-i,--input,input", inputPath, "Path to audio file")->required();
    appEncode->add_option("-w,--window", windowWidthMs, "Window width/speed (ms)");
    appEncode->add_option("-b,--highpass", highpassCutoff, "Highpass filter cutoff (Hz)");
    appEncode->add_option("-l,--lowpass", lowpassCutoff, "Lowpass filter cutoff (Hz)");
    appEncode->add_option("-a,--alpha", preEmphasisAlpha, "Pre-emphasis filter coefficient");
    appEncode->add_option("-f,--format", bitstreamFormat, "Bitstream format: ascii (0), c (1), arduino (2)")->check(CLI::Range(0, 2));
    appEncode->add_option("-s,--separator", lpcSeparator, "Separator for hex bitstreams");
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
        auto encoder = BitstreamGenerator(windowWidthMs, highpassCutoff, lowpassCutoff,
                                          preEmphasisAlpha, bitstreamFormat, !noStopFrame,
                                          gainShift, maxVoicedGain,maxUnvoicedGain,
                                          useRepeatFrames, maxPitchFrq,minPitchFrq);

        encoder.encode(inputPath, outputPath);
    }

    exit(EXIT_SUCCESS);
}
