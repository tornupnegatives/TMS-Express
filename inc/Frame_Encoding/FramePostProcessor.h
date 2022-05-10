//
// Created by Joseph Bellahcen on 5/8/22.
//

#ifndef TMS_EXPRESS_FRAMEPOSTPROCESSOR_H
#define TMS_EXPRESS_FRAMEPOSTPROCESSOR_H

#include "Frame_Encoding/Frame.h"
#include "LPC_Analysis/LowerVocalTractAnalyzer.h"

class FramePostProcessor {
public:
    FramePostProcessor(Frame **frames, int count, float maxVoicedGainDB = 37.5, float maxUnvoicedGainDB = 37.5);

    void normalizeGain();
    void shiftGain(int offset);
    void detectRepeatFrames();

private:
    Frame **frames;
    int count;
    float maxVoicedGain;
    float maxUnvoicedGain;

    void normalizeGain(Voicing voicing);
};

#endif //TMS_EXPRESS_FRAMEPOSTPROCESSOR_H
