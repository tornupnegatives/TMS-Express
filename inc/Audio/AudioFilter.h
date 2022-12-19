// Author: Joseph Bellahcen <joeclb@icloud.com>

#ifndef TMS_EXPRESS_AUDIOFILTER_H
#define TMS_EXPRESS_AUDIOFILTER_H

#include "AudioBuffer.h"
#include <string>
#include <vector>

class AudioFilter {
public:
    explicit AudioFilter();

    enum FilterBiquadMode {FILTER_LOWPASS, FILTER_HIGHPASS, FILTER_NONE};
    std::vector<float> applyBiquad(std::vector<float> segment, unsigned int cutoffHz, FilterBiquadMode mode);
    std::vector<float> applyPreemphasis(std::vector<float> segment, float alpha = -0.9375);
    std::vector<float> applyHammingWindow(std::vector<float> segment);

private:
    FilterBiquadMode lastFilterMode;
    unsigned int lastCutoffHz;
    std::vector<float> coeffs;
    float normalizationCoeff;

    void setCoefficients(FilterBiquadMode mode, unsigned int cutoff);
};

#endif //TMS_EXPRESS_AUDIOFILTER_H
