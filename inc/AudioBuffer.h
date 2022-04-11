//
// Created by Joseph Bellahcen on 4/6/22.
//

#ifndef TMS_EXPRESS_AUDIOBUFFER_H
#define TMS_EXPRESS_AUDIOBUFFER_H

#include <sndfile.hh>

class AudioBuffer {
public:
    int numSegments;

    AudioBuffer(const char *path, int targetSampleRate, float windowSize);
    ~AudioBuffer();
    float *segment(int i, int *size);

private:
    int size;
    float *samples;
    int channels;
    int sampleRate;
    int samplesPerSegment;

    int frames();
    void mixdown();
    void resample(int targetSampleRate);
    void padFinalSegment();

    // TODO: Implement AudioBufferPreProcessor
    // preEmphasize(alpha)
    // filter(cutoff)
};

#endif //TMS_EXPRESS_AUDIOBUFFER_H
