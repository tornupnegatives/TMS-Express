// Author: Joseph Bellahcen <joeclb@icloud.com>

#ifndef TMS_EXPRESS_FRAMEPOSTPROCESSOR_H
#define TMS_EXPRESS_FRAMEPOSTPROCESSOR_H

#include "Frame.h"
#include <vector>

class FramePostprocessor {
public:
    explicit FramePostprocessor(std::vector<Frame> *frames, float maxVoicedGainDB = 37.5, float maxUnvoicedGainDB = 37.5);

    int detectRepeatFrames();
    void normalizeGain();
    void shiftGain(int offset);

    // TODO: Implement in v2.0.0
    //void setFixedPitch(int freqHz);

private:
    std::vector<Frame> *frameData;
    float maxUnvoicedGain;
    float maxVoicedGain;

    void normalizeGain(bool normalizeVoicedFrames);
};

#endif //TMS_EXPRESS_FRAMEPOSTPROCESSOR_H
