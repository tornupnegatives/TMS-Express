///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class: UserParameters
//
// Description: The UserParameters class parses and command-line arguments, which can be used to tweak the parameters
//              of the speech analysis and encoding process
//
// Author: Joseph Bellahcen <joeclb@icloud.com>
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "boost/program_options.hpp"
#include "UserParameters.h"
#include <iostream>
#include <string>

namespace po = boost::program_options;
using po::value;

UserParameters::UserParameters(int argc, char **argv) {
    // Build Boost options
    auto desc = po::options_description("Allowed options");
    desc.add_options()
        ("help", "display summary of allowed options")
        ("input", value<std::string>(),"path to audio file for processing")
        ("width", value<float>()->default_value(25.0), "width of segmentation window (ms)")
        ("highpass", value<int>()->default_value(600), "highpass filter cutoff (Hz)")
        ("lowpass", value<int>()->default_value(400), "lowpass filter cutoff (Hz)")
        ("alpha", value<float>()->default_value(-0.9375), "pre-emphasis alpha")
        ("include-prefix", value<bool>()->default_value(false), "prefix hex output with \"0x\"")
        ("separator", value<char>()->default_value(','), "bitstream separator")
        ("include-stop-frame", value<bool>()->default_value(true), "end the bitstream with a stop frame")
        ("max-voiced-gain", value<float>()->default_value(37.5), "max gain (dB) for voiced frames")
        ("max-unvoiced-gain", value<float>()->default_value(37.5), "max gain (dB) for unvoiced frames")
        ("max-frq", value<int>()->default_value(50), "max pitch frequency (Hz)")
        ("min-frq", value<int>()->default_value(500), "min pitch frequency (Hz)")
        ("output", value<std::string>(), "path to output bitstream")
        ;

    // Get arguments
    po:boost::program_options::variables_map variables;
    po::store(po::command_line_parser(argc, argv).options(desc).run(), variables);
    po::notify(variables);

    // Input validation
    if (!variables.count("input") || !variables.count("output")) {
        std::cerr << "Error: Input and output paths are required" << std::endl;
        std::cout << desc << std::endl;
        exit(EXIT_FAILURE);

    } else if (variables.count("help")) {
        std::cout << desc << std::endl;
        exit(EXIT_SUCCESS);
    }

    // Store arguments
    inputPath = variables["input"].as<std::string>();
    windowWidthMs = variables["width"].as<float>();
    highpassFilterCutoffHz = variables["highpass"].as<int>();
    lowpassFilterCutoffHz = variables["lowpass"].as<int>();
    preemphasisAlpha = variables["alpha"].as<float>();
    includeHexPrefix = variables["include-prefix"].as<bool>();
    hexStreamSeparator = variables["separator"].as<char>();
    shouldAppendStopFrame = variables["include-stop-frame"].as<bool>();
    maxVoicedGainDb = variables["max-voiced-gain"].as<float>();
    maxUnvoicedGainDb = variables["max-unvoiced-gain"].as<float>();
    minFrqHz = variables["max-frq"].as<int>();
    maxFrqHz = variables["min-frq"].as<int>();
    outputLpcPath = variables["output"].as<std::string>();
}

const std::string &UserParameters::getInputPath() const {
    return inputPath;
}

float UserParameters::getWindowWidthMs() const {
    return windowWidthMs;
}

int UserParameters::getHighpassFilterCutoffHz() const {
    return highpassFilterCutoffHz;
}

int UserParameters::getLowpassFilterCutoffHz() const {
    return lowpassFilterCutoffHz;
}

float UserParameters::getPreemphasisAlpha() const {
    return preemphasisAlpha;
}

bool UserParameters::getIncludeHexPrefix() const {
    return includeHexPrefix;
}

char UserParameters::getHexStreamSeparator() const {
    return hexStreamSeparator;
}

bool UserParameters::getShouldAppendStopFrame() const {
    return shouldAppendStopFrame;
}

float UserParameters::getMaxVoicedGainDb() const {
    return maxVoicedGainDb;
}

float UserParameters::getMaxUnvoicedGainDb() const {
    return maxUnvoicedGainDb;
}

int UserParameters::getMinFrqHz() const {
    return minFrqHz;
}

int UserParameters::getMaxFrqHz() const {
    return maxFrqHz;
}

const std::string &UserParameters::getOutputLpcPath() const {
    return outputLpcPath;
}

