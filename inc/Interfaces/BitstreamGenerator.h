#ifndef TMS_EXPRESS_BITSTREAMGENERATOR_H
#define TMS_EXPRESS_BITSTREAMGENERATOR_H

#include <string>

typedef enum {ENCODERSTYLE_ASCII, ENCODERSTYLE_C, ENCODERSTYLE_ARDUINO} EncoderStyle;

class BitstreamGenerator {
public:
    BitstreamGenerator(float windowMs, int highpassHz, int lowpassHz, float preemphasis, EncoderStyle style,
                       bool includeStopFrame, int gainShift, float maxVoicedDb, float maxUnvoicedDb, bool detectRepeats,
                       int maxHz, int minHz);

    void encode(const std::string &inputPath, const std::string &outputPath);
    //encodeBatch

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

    static bool inputFileExists(const std::string &inputPath);
    //FILE *outputFile(std::string outputPath);
    std::string generateBitstream(const std::string &inputPath) const;
    std::string formatBitstream(std::string bitstream, const std::string &filename);
};

#endif //TMS_EXPRESS_BITSTREAMGENERATOR_H
