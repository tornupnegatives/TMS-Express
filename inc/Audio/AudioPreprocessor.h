//
// Created by Joseph Bellahcen on 4/14/22.
//

#ifndef TMS_EXPRESS_AUDIOPREPROCESSOR_H
#define TMS_EXPRESS_AUDIOPREPROCESSOR_H

#include "AudioBuffer.h"

class AudioPreprocessor {
public:
    explicit AudioPreprocessor(AudioBuffer *audioBuffer);

    void preEmphasize(float alpha = -0.9375);
    void lowpassFilter(float cutoff);
    void highpassFilter(float cutoff);
    void hammingWindow();

private:
    AudioBuffer *buffer;
    float *samples;
    int size;
    float filter_dt;

    static void hammingWindow(float *segment, int size);
};

#endif //TMS_EXPRESS_AUDIOPREPROCESSOR_H
