// Author: Joseph Bellahcen <joeclb@icloud.com>

#ifndef TMS_EXPRESS_FRAMEPOSTPROCESSOR_H
#define TMS_EXPRESS_FRAMEPOSTPROCESSOR_H

#include "Frame.h"
#include <vector>

class FramePostprocessor {
public:
    explicit FramePostprocessor(std::vector<Frame> *frames, float maxVoicedGainDB = 37.5, float maxUnvoicedGainDB = 37.5);

    void normalizeGain();
    void shiftGain(int offset);
    void detectRepeatFrames();

    // TODO: Implement in v2.0.0
    void fixPitch(int freqHz);

private:
    std::vector<Frame> *frameData;
    float maxVoicedGain;
    float maxUnvoicedGain;

    void normalizeGain(bool voiced);
};

#endif //TMS_EXPRESS_FRAMEPOSTPROCESSOR_H
