// Author: Joseph Bellahcen <joeclb@icloud.com>

#ifndef TMS_EXPRESS_PITCHESTIMATOR_H
#define TMS_EXPRESS_PITCHESTIMATOR_H

#include "Audio/AudioBuffer.h"
#include <vector>

class PitchEstimator {
public:
    explicit PitchEstimator(int sampleRateHz, int minFrqHz = 50, int maxFrqHz = 500);

    int getMinPeriod();
    void setMinPeriod(int maxFrqHz);

    int getMaxPeriod();
    void setMaxPeriod(int minFrqHz);

    int estimatePeriod(const std::vector<float> &acf) const;
    float estimateFrequency(const std::vector<float> &acf) const;

private:
    int sampleRate;
    int minPeriod;
    int maxPeriod;
};

#endif //TMS_EXPRESS_PITCHESTIMATOR_H