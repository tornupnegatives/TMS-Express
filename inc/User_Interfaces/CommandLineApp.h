// Author: Joseph Bellahcen <joeclb@icloud.com>

#ifndef TMS_EXPRESS_COMMANDLINEAPP_H
#define TMS_EXPRESS_COMMANDLINEAPP_H

#include "Bitstream_Generation/BitstreamGenerator.h"
#include <CLI11.hpp>

class CommandLineApp : public CLI::App {
public:
    CommandLineApp();

    int run(int argc, char** argv);

private:
    CLI::App* encoder;

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

    void setupEncoder();
};

#endif //TMS_EXPRESS_COMMANDLINEAPP_H
