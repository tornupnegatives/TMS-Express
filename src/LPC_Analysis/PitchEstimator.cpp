///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class: PitchEstimator
//
// Description: The PitchEstimator detects the pitch of a segment based on its cumulative mean normalized distribution.
//              This class implements a very basic autocorrelation-based pitch detector, which exploits the fact that
//              the autocorrelation of a periodic signal is also periodic
//
// Author: Joseph Bellahcen <joeclb@icloud.com>
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "LPC_Analysis/PitchEstimator.h"
#include <algorithm>
#include <vector>

PitchEstimator::PitchEstimator(int sampleRateHz, int minFrqHz, int maxFrqHz) {
    sampleRate = sampleRateHz;
    minPeriod = sampleRateHz / maxFrqHz;
    maxPeriod = sampleRateHz / minFrqHz;
}

int PitchEstimator::getMinPeriod() {
    return minPeriod;
}

void PitchEstimator::setMinPeriod(int maxFrqHz) {
    minPeriod = sampleRate / maxFrqHz;
}

int PitchEstimator::getMaxPeriod() {
    return maxPeriod;
}

void PitchEstimator::setMaxPeriod(int minFrqHz) {
    maxPeriod = sampleRate / minFrqHz;
}

// Estimate the pitch period of the segment from its autocorrelation
//
// Because a small enough segment of speech is roughly periodic, the autocorrelation will also be periodic. This makes
// it a useful estimator of pitch
int PitchEstimator::estimatePeriod(const std::vector<float> &acf) const {
    // Restrict the search window to the min and max pitch periods set during initialization
    auto acfStart = acf.begin() + minPeriod;
    auto acfEnd = acf.begin()  + maxPeriod;

    // Identify the first local minimum and subsequent local maximum. The distance between these values likely
    // corresponds to the pitch period of the segment
    auto firstLocalMin = std::min_element(acfStart, acfEnd);
    auto period = int(std::distance(acf.begin(), std::max_element(firstLocalMin, acfEnd)));

    if (period > maxPeriod) {
        return maxPeriod;
    } else if (period < minPeriod) {
        return minPeriod;
    } else {
        return period;
    }
}

float PitchEstimator::estimateFrequency(const std::vector<float> &acf) const {
    auto period = estimatePeriod(acf);
    return float(sampleRate) / float(period);
}
