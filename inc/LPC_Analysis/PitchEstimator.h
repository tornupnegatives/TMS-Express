// Author: Joseph Bellahcen <joeclb@icloud.com>

#ifndef TMS_EXPRESS_PITCHESTIMATOR_H
#define TMS_EXPRESS_PITCHESTIMATOR_H

#include "Audio/AudioBuffer.h"
#include <vector>

class PitchEstimator {
public:
    explicit PitchEstimator(int sampleRateHz, int minFrqHz = 50, int maxFrqHz = 500);

    // Getters & setters
    [[nodiscard]] int getMaxPeriod() const;
    int getMaxFrq() const;
    void setMaxPeriod(int minFrqHz);

    [[nodiscard]] int getMinPeriod() const;
    int getMinFrq() const;
    void setMinPeriod(int maxFrqHz);

    // Estimator functions
    [[nodiscard]] float estimateFrequency(const std::vector<float> &acf) const;
    [[nodiscard]] int estimatePeriod(const std::vector<float> &acf) const;

private:
    int maxPeriod;
    int minPeriod;
    int sampleRate;
};

#endif //TMS_EXPRESS_PITCHESTIMATOR_H