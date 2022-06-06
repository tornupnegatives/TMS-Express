//
// Created by Joseph Bellahcen on 6/1/22.
//

#include "LPC_Analysis/Autocorrelator.h"
#include <cmath>
#include <gtest/gtest.h>
#include <vector>

// Compute the autocorrelation of a decaying cosine signal with amplitude 20 and period 50
std::vector<float> acfTestSubject() {
    auto signal = std::vector<float>();
    for (int i = 0; i < 200; i ++) {
        float sample = 20.0f * cosf(2.0f * float(M_PI) * float(i) / 50.0f) * expf(-0.02f * float(i));
        signal.push_back(sample);
    }

    auto acf = Autocorrelator::process(signal);
    return acf;
}

// The autocorrelation should be maximized at time zero, or pitch detection will fail
TEST(AutocorrelatorTests, AutocorrelationIsMaxAtIndexZero) {
    auto acf = acfTestSubject();

    auto maxElement = std::max_element(acf.begin(), acf.end());
    auto maxIdx = std::distance(acf.begin(), maxElement);
    EXPECT_EQ(maxIdx, 0);
}

// For a periodic signal, the autocorrelation should have its second-largest local max at the period. This property
// is the basis of many pitch estimation algorithms
TEST(AutocorrelatorTests, AutocorrelationHasLocalMaxAtOriginalSignalPeriod) {
    auto acf = acfTestSubject();

    auto maxElement = std::max_element(acf.begin(), acf.end());
    auto maxIdx = std::distance(acf.begin(), maxElement);

    auto minElement = std::min_element(acf.begin(), acf.end());
    auto nextMaxElement = std::max_element(minElement, acf.end());
    auto periodIdx = std::distance(acf.begin(), nextMaxElement);
    EXPECT_NEAR(periodIdx, 50, 2);
}