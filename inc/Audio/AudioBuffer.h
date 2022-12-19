// Author: Joseph Bellahcen <joeclb@icloud.com>

#ifndef TMS_EXPRESS_AUDIOBUFFER_H
#define TMS_EXPRESS_AUDIOBUFFER_H

#include <string>
#include <vector>

class AudioBuffer {
public:
    explicit AudioBuffer(const std::string &path, float segmentDurationMs = 20.0f);
    AudioBuffer(const AudioBuffer &buffer);

    void resample(int targetSampleRateHz);

    // Properties
    unsigned int sampleRate() const;
    unsigned int nSegments() const;
    unsigned int samplesPerSegment() const;

    // Getters/Setters
    std::vector<float> getSamples();
    void setSamples(const std::vector<float> &newSamples);
    std::vector<float> getSegment(int i);

    // Unused attributes remain implemented, as this class will likely reappear in other projects ;-)
    __attribute__((unused)) std::vector<std::vector<float>> getSegments();
    __attribute__((unused)) void exportAudio(const std::string &path);

private:
    const int normalizationFactor = 1<<15;
    unsigned int sampleRateHz;
    float segmentDuration;
    std::vector<float> samples;

    void mixToMono(int nChannels);
    void pad();
};

#endif //TMS_EXPRESS_AUDIOBUFFER_H
