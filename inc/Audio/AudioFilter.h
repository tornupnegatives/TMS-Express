// Author: Joseph Bellahcen <joeclb@icloud.com>

#ifndef TMS_EXPRESS_AUDIOFILTER_H
#define TMS_EXPRESS_AUDIOFILTER_H

#include "AudioBuffer.h"
#include "Iir.h"
#include <string>
#include <vector>

class AudioFilter {
public:
    explicit AudioFilter(int sampleRateHz, int highpassCutoffHz, int lowpassCutoffHz, float preemphasisAlpha);

    std::vector<float> applyHighpass(std::vector<float> segment);
    std::vector<float> applyLowpass(std::vector<float> segment);
    std::vector<float> applyPreemphasis(std::vector<float> segment) const;
    std::vector<float> applyHammingWindow(std::vector<float> segment) const;

private:
    int sampleRate;
    int highpassCutoff;
    int lowpassCutoff;
    float alpha;
    const float pi = M_PI;
    Iir::Butterworth::HighPass<4> hpf;
    Iir::Butterworth::LowPass<4> lpf;
};

#endif //TMS_EXPRESS_AUDIOFILTER_H
