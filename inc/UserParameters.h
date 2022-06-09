#ifndef TMS_EXPRESS_USERPARAMETERS_H
#define TMS_EXPRESS_USERPARAMETERS_H

#include "boost/program_options.hpp"

namespace po = boost::program_options;

class UserParameters {
public:
    UserParameters(int argc, char **argv);

    const std::string &getInputPath() const;
    float getWindowWidthMs() const;
    int getHighpassFilterCutoffHz() const;
    int getLowpassFilterCutoffHz() const;
    float getPreemphasisAlpha() const;
    bool getIncludeHexPrefix() const;
    char getHexStreamSeparator() const;
    bool getShouldAppendStopFrame() const;
    int getGainShift() const;
    float getMaxVoicedGainDb() const;
    float getMaxUnvoicedGainDb() const;
    bool getDetectRepeats() const;
    int getMinFrqHz() const;
    int getMaxFrqHz() const;
    const std::string &getOutputLpcPath() const;

private:
    // AudioBuffer
    std::string inputPath;
    float windowWidthMs;

    // AudioPreprocessor
    int highpassFilterCutoffHz;
    int lowpassFilterCutoffHz;
    float preemphasisAlpha;

    // FrameEncoder
    bool includeHexPrefix;
    char hexStreamSeparator;
    bool shouldAppendStopFrame;

    // FramePostprocessor
    int gainShift;
    float maxVoicedGainDb;
    float maxUnvoicedGainDb;
    bool detectRepeats;

    // PitchEstimator
    int minFrqHz;
    int maxFrqHz;

    // Program output
    std::string outputLpcPath;
};

#endif //TMS_EXPRESS_USERPARAMETERS_H