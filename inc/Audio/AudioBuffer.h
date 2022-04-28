//
// Created by Joseph Bellahcen on 4/6/22.
//

#ifndef TMS_EXPRESS_AUDIOBUFFER_H
#define TMS_EXPRESS_AUDIOBUFFER_H

class AudioBuffer {
public:
    AudioBuffer(const char *path, int targetSampleRate, float windowSize);
    ~AudioBuffer();

    int getSize() const;
    int getSampleRate() const;
    int getSamplesPerSegment() const;
    int getNumSegments() const;
    float *getSamples();
    float *getSegment(int i);

private:
    int size;
    int sampleRate;
    int channels;
    int samplesPerSegment;
    int numSegments;
    float *samples;

    int frames() const;
    void mixdown();
    void resample(int targetSampleRate);
    void padFinalSegment();
};

#endif //TMS_EXPRESS_AUDIOBUFFER_H
