// Copyright 2023 Joseph Bellahcen <joeclb@icloud.com>

#include "LPC_Analysis/PitchEstimator.hpp"

#include <algorithm>
#include <vector>

namespace tms_express {

///////////////////////////////////////////////////////////////////////////////
// Initializers ///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

PitchEstimator::PitchEstimator(int sample_rate_hz, int min_frq_hz,
    int max_frq_hz) {
    //
    max_period_ = sample_rate_hz / min_frq_hz;
    min_period_ = sample_rate_hz / max_frq_hz;
    sample_rate_hz_ = sample_rate_hz;
}

///////////////////////////////////////////////////////////////////////////////
// Accessors //////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

int PitchEstimator::getMinPeriod() const {
    return min_period_;
}

int PitchEstimator::getMinFrq() const {
    return sample_rate_hz_ / max_period_;
}

void PitchEstimator::setMinPeriod(int maxFrqHz) {
    min_period_ = sample_rate_hz_ / maxFrqHz;
}

int PitchEstimator::getMaxPeriod() const {
    return max_period_;
}

int PitchEstimator::getMaxFrq() const {
    return sample_rate_hz_ / min_period_;
}

void PitchEstimator::setMaxPeriod(int minFrqHz) {
    max_period_ = sample_rate_hz_ / minFrqHz;
}

///////////////////////////////////////////////////////////////////////////////
// Pitch Estimation ///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

float PitchEstimator::estimateFrequency(const std::vector<float> &acf) const {
    auto period = estimatePeriod(acf);
    return static_cast<float>(sample_rate_hz_) / static_cast<float>(period);
}

int PitchEstimator::estimatePeriod(const std::vector<float> &acf) const {
    // Restrict the search window to the min and max pitch periods
    auto start = acf.begin() + min_period_;
    auto end = acf.begin()  + max_period_;

    // Identify the first local minimum and subsequent local maximum.
    // The distance between these values likely corresponds to the pitch period
    // of the segment
    auto local_min = std::min_element(start, end);
    auto period = std::distance(acf.begin(), std::max_element(local_min, end));

    if (period > max_period_) {
        return max_period_;
    } else if (period < min_period_) {
        return min_period_;
    } else {
        return period;
    }
}

};  // namespace tms_express
