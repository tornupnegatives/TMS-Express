//
// Created by Joseph Bellahcen on 4/6/22.
//

#ifndef TMS_EXPRESS_AUDIOBUFFER_H
#define TMS_EXPRESS_AUDIOBUFFER_H

#include <sndfile.hh>

class AudioBuffer {
public:
    AudioBuffer(const char *path, int targetSampleRate, float windowSize);
    ~AudioBuffer();

    float *getSegment(int i, int *size);
    float *getSamples(int *size);
    int getSampleRate();
    int getNumSegments();
    int getSamplesPerSegment();

private:
    int size;
    float *samples;
    int channels;
    int sampleRate;
    int samplesPerSegment;
    int numSegments;

    int frames();
    void mixdown();
    void resample(int targetSampleRate);
    void padFinalSegment();
};

#endif //TMS_EXPRESS_AUDIOBUFFER_H
