// Author: Joseph Bellahcen <joeclb@icloud.com>

#ifndef TMS_EXPRESS_AUDIOFILTER_H
#define TMS_EXPRESS_AUDIOFILTER_H

#include "AudioBuffer.hpp"
#include <array>
#include <string>
#include <vector>

namespace tms_express {

class AudioFilter {
public:
    AudioFilter();

    void hammingWindow(AudioBuffer &buffer);
    void hammingWindow(std::vector<float> &segment);
    void highpass(AudioBuffer &buffer, int cutoffHz);
    void lowpass(AudioBuffer &buffer, int cutoffHz);
    void preEmphasis(AudioBuffer &buffer, float alpha = 0.9375);

private:
    typedef enum FilterMode {HPF, LPF} FilterMode;
    std::array<float, 6> coeffs{0, 0, 0, 0, 0, 0};

    void applyBiquad(AudioBuffer &buffer);
    void computeCoeffs(FilterMode mode, int cutoffHz);
};

};  // namespace tms_express

#endif //TMS_EXPRESS_AUDIOFILTER_H
