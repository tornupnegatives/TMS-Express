// Author: Joseph Bellahcen <joeclb@icloud.com>

#ifndef TMS_EXPRESS_BITSTREAMGENERATOR_H
#define TMS_EXPRESS_BITSTREAMGENERATOR_H

#include "Frame_Encoding/Frame.h"
#include <string>

namespace tms_express {

class BitstreamGenerator {
public:
    typedef enum {ENCODERSTYLE_ASCII, ENCODERSTYLE_C, ENCODERSTYLE_ARDUINO, ENCODERSTYLE_JSON} EncoderStyle;
    BitstreamGenerator(float windowMs, int highpassHz, int lowpassHz, float preemphasis, EncoderStyle style,
                       bool includeStopFrame, int gainShift, float maxVoicedDb, float maxUnvoicedDb, bool detectRepeats,
                       int maxHz, int minHz);

    void encode(const std::string &inputPath, const std::string &inputFilename, const std::string &outputPath);
    void encodeBatch(const std::vector<std::string> &inputPaths, const std::vector<std::string> &inputFilenames,
                     const std::string &outputPath);

private:
    float windowMs;
    int highpassHz;
    int lowpassHz;
    float preemphasisAlpha;
    EncoderStyle style;
    bool includeStopFrame;
    int gainShift;
    float maxVoicedDB;
    float maxUnvoicedDB;
    bool detectRepeats;
    int maxHz;
    int minHz;

    std::vector<Frame> generateFrames(const std::string &inputPath) const;
    std::string formatBitstream(const std::vector<Frame>& frames, const std::string &filename);
};

};  // namespace tms_express

#endif //TMS_EXPRESS_BITSTREAMGENERATOR_H
