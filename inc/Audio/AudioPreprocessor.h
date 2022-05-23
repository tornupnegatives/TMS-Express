//
// Created by Joseph Bellahcen on 4/14/22.
//

#ifndef TMS_EXPRESS_AUDIOPREPROCESSOR_H
#define TMS_EXPRESS_AUDIOPREPROCESSOR_H

#include "AudioBuffer.h"

class AudioPreprocessor {
public:
    explicit AudioPreprocessor(AudioBuffer *audioBuffer);

    void preEmphasize(float *segment, float alpha = -0.9375);
    void lowpassFilter(float *segment, float cutoff);
    void highpassFilter(float *segment, float cutoff);
    void hammingWindow(float *segment);

private:
    int samplesPerSegment;
    float filter_dt;
};

#endif //TMS_EXPRESS_AUDIOPREPROCESSOR_H
