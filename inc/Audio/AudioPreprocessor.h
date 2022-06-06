#ifndef TMS_EXPRESS_AUDIOPREPROCESSOR_H
#define TMS_EXPRESS_AUDIOPREPROCESSOR_H

#include "AudioBuffer.h"
#include <vector>

class AudioPreprocessor {
public:
    explicit AudioPreprocessor();

    enum FilterBiquadMode {FILTER_LOWPASS, FILTER_HIGHPASS, FILTER_NONE};

    void applyBiquad(AudioBuffer &buffer, unsigned int cutoffHz, FilterBiquadMode mode);
    void applyPreemphasis(AudioBuffer &buffer, float alpha = -0.9375);
    void applyHammingWindow(std::vector<float> &segment);

private:
    FilterBiquadMode lastFilterMode;
    unsigned int lastCutoffHz;
    std::vector<float> coeffs;

    void setCoefficients(FilterBiquadMode mode, unsigned int cutoff);
};

#endif //TMS_EXPRESS_AUDIOPREPROCESSOR_H
