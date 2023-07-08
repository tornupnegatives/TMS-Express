// Author: Joseph Bellahcen <joeclb@icloud.com>

#ifndef TMS_EXPRESS_AUDIOBUFFER_H
#define TMS_EXPRESS_AUDIOBUFFER_H

#include <string>
#include <vector>

class AudioBuffer {
public:
    explicit AudioBuffer(const std::string &path, int targetSampleRateHz = 8000, float windowWidthMs = 25.0f);
    explicit AudioBuffer(std::vector<float> pcmSamples, int sampleRate = 8000, float windowWidthMs = 25.0f);
    AudioBuffer(int sampleRate = 8000, float windowWidthMs = 25.0f);
    AudioBuffer(const AudioBuffer &buffer);

    // Getters & setters
    std::vector<float> getSamples();
    void setSamples(const std::vector<float> &newSamples);

    [[nodiscard]] float getWindowWidth() const;
    void setWindowWidth(float windowWidthMs);

    // Const getters
    [[nodiscard]] int sampleRate() const;
    std::vector<float> segment(int i);
    std::vector<std::vector<float>> segments();
    [[nodiscard]] size_t segmentSize() const;
    [[nodiscard]] size_t size() const;
    bool isEmpty() const;
    
    // Utility
    void render(const std::string &path);
    void reset();

private:
    size_t nSegments;
    std::vector<float> originalSamples;
    int sampleRateHz;
    std::vector<float> samples;
    size_t samplesPerSegment;

    void mixToMono(int nOriginalChannels);
    void resample(int targetSampleRateHz);
};

#endif //TMS_EXPRESS_AUDIOBUFFER_H
