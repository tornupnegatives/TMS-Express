//
// Created by Joseph Bellahcen on 7/16/22.
//

#ifndef TMS_EXPRESS_ENCODER_H
#define TMS_EXPRESS_ENCODER_H

#include "Frame_Encoding/FrameEncoder.h"
#include <string>

class Encoder {
public:
    Encoder(float windowMs, int highpassHz, int lowpassHz, float preemphasis, bool cStyle, char separator,
            bool includeStopFrame, int gainShift, float maxVoicedDb, float maxUnvoicedDb, bool detectRepeats, int maxHz,
            int minHz);

    void encode(const std::string &inputPath, const std::string &outputPath);

private:
    float windowMs;
    int highpassHz;
    int lowpassHz;
    float preemphasisAlpha;
    bool cStyle;
    char separator;
    bool includeStopFrame;
    int gainShift;
    float maxVoicedDB;
    float maxUnvoicedDB;
    bool detectRepeats;
    int maxHz;
    int minHz;

    static bool inputFileExists(const std::string& inputPath);
    //FILE *outputFile(std::string outputPath);
    std::string analyzeAudio(const std::string& inputPath) const;
};


#endif //TMS_EXPRESS_ENCODER_H
