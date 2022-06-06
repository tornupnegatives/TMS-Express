#ifndef TMS_EXPRESS_PITCHESTIMATOR_H
#define TMS_EXPRESS_PITCHESTIMATOR_H

#include "Audio/AudioBuffer.h"
#include <vector>

class PitchEstimator {
public:
    explicit PitchEstimator(int sampleRateHz, int minFrqHz = 50, int maxFrqHz = 500);

    int estimatePeriod(const std::vector<float> &acf) const;

private:
    int minPeriod;
    int maxPeriod;
};

#endif //TMS_EXPRESS_PITCHESTIMATOR_H