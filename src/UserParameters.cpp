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
using po::bool_switch;

UserParameters::UserParameters(int argc, char **argv) {
    // Build Boost options
    auto desc = po::options_description("TMS Express options");
    desc.add_options()
        ("help,h", "display summary of allowed options")
        ("input,i", value<std::string>()->required(),"path to audio file for processing")
        ("width,w", value<float>()->default_value(25.0), "width of segmentation window (ms)")
        ("highpass,j", value<int>()->default_value(600), "highpass filter cutoff (Hz)")
        ("lowpass,l", value<int>()->default_value(400), "lowpass filter cutoff (Hz)")
        ("alpha,a", value<float>()->default_value(-0.9375), "pre-emphasis alpha")
        ("include-prefix,p", bool_switch()->default_value(false), "prefix hex output with \"0x\"")
        ("separator,s", value<char>()->default_value(','), "bitstream separator")
        ("no-stop-frame,n", bool_switch()->default_value(true), "end the bitstream with a stop frame")
        ("gain,g", value<int>()->default_value(2), "increase the prediction gain by offsetting the coding table index")
        ("max-voiced-gain,v", value<float>()->default_value(37.5), "max gain (dB) for voiced frames")
        ("max-unvoiced-gain,u", value<float>()->default_value(30), "max gain (dB) for unvoiced frames")
        ("max-frq,c", value<int>()->default_value(50), "max pitch frequency (Hz)")
        ("min-frq,d", value<int>()->default_value(500), "min pitch frequency (Hz)")
        ("output,o", value<std::string>()->required(), "path to output bitstream")
        ;

    // Try parsing arguments. Show help message and crash on failure
    po:boost::program_options::variables_map variables;
    try {
        po::store(po::command_line_parser(argc, argv).options(desc).run(), variables);
        po::notify(variables);

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        std::cout << desc << std::endl;
        exit(EXIT_FAILURE);
    }

    // If help is requested, do not perform further operations
    if (variables.count("help")) {
        std::cout << desc << std::endl;
        exit(EXIT_SUCCESS);
    }

    // Try to store arguments. Show help message and crash on failure
    try {
        inputPath = variables["input"].as<std::string>();
        windowWidthMs = variables["width"].as<float>();
        highpassFilterCutoffHz = variables["highpass"].as<int>();
        lowpassFilterCutoffHz = variables["lowpass"].as<int>();
        preemphasisAlpha = variables["alpha"].as<float>();
        includeHexPrefix = variables["include-prefix"].as<bool>();
        hexStreamSeparator = variables["separator"].as<char>();
        shouldAppendStopFrame = variables["no-stop-frame"].as<bool>();
        gainShift = variables["gain"].as<int>();
        maxVoicedGainDb = variables["max-voiced-gain"].as<float>();
        maxUnvoicedGainDb = variables["max-unvoiced-gain"].as<float>();
        minFrqHz = variables["max-frq"].as<int>();
        maxFrqHz = variables["min-frq"].as<int>();
        outputLpcPath = variables["output"].as<std::string>();

    } catch (const std::exception& e) {
        std::cerr << "Error: could not parse arguments (" << e.what() << ")" << std::endl;
        std::cerr << e.what() << std::endl;
        std::cout << desc << std::endl;
        exit(EXIT_FAILURE);
    }
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

int UserParameters::getGainShift() const {
    return gainShift;
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
