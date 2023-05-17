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

/// Create a new pitch estimator, bounded by the min and max frequencies
///
/// \param sampleRateHz Sample rate of the audio samples
/// \param minFrqHz Minimum frequency to search
/// \param maxFrqHz Maximum frequency to search
PitchEstimator::PitchEstimator(int sampleRateHz, int minFrqHz, int maxFrqHz) {\
    maxPeriod = sampleRateHz / minFrqHz;
    minPeriod = sampleRateHz / maxFrqHz;
    sampleRate = sampleRateHz;
}

///////////////////////////////////////////////////////////////////////////////
//                          Getters & Setters
///////////////////////////////////////////////////////////////////////////////

/// Return the minimum pitch period (in samples) to search
int PitchEstimator::getMinPeriod() const {
    return minPeriod;
}

/// Set the minimum pitch period (in samples) to search
///
/// \note Setting a minimum pitch period may reduce the computation time of pitch estimation
///
/// \param maxFrqHz The minimum pitch period is determined by the maximum pitch frequency (in Hertz)
void PitchEstimator::setMinPeriod(int maxFrqHz) {
    minPeriod = sampleRate / maxFrqHz;
}

/// Return the maximum pitch period (in samples) to search
int PitchEstimator::getMaxPeriod() const {
    return maxPeriod;
}

/// Set the maximum pitch period (in samples) to search
///
/// \note Setting a maximum pitch period may reduce the computation time of pitch estimation
///
/// \param minFrqHz The maximum pitch period is determined by the minimum pitch frequency (in Hertz)
void PitchEstimator::setMaxPeriod(int minFrqHz) {
    maxPeriod = sampleRate / minFrqHz;
}

///////////////////////////////////////////////////////////////////////////////
//                                 Estimators
///////////////////////////////////////////////////////////////////////////////

/// Estimate the pitch frequency of the segment (in Hertz) from its autocorrelation
///
/// \param acf Autocorrelation of the segment
/// \return Pitch frequency estimate (in Hertz)
float PitchEstimator::estimateFrequency(const std::vector<float> &acf) const {
    auto period = estimatePeriod(acf);
    return float(sampleRate) / float(period);
}

/// Estimate the pitch period of the segment (in samples) from its autocorrelation
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
