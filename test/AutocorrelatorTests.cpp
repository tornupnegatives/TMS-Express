// Copyright 2023 Joseph Bellahcen <joeclb@icloud.com>

#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include <vector>

#include "LPC_Analysis/Autocorrelation.hpp"

namespace tms_express {

/// @brief Produces test subject, which is the autocorrelation of a decaying
///         cosine signal with amplitude 20 and period 50
/// @return Test subject
std::vector<float> acfTestSubject() {
    auto signal = std::vector<float>();
    for (int i = 0; i < 200; i ++) {
        float sample = 20.0f *
            cosf(2.0f * static_cast<float>(M_PI) *
            static_cast<float>(i) / 50.0f) *
            expf(-0.02f * static_cast<float>(i));

        signal.push_back(sample);
    }

    auto acf = tms_express::Autocorrelation(signal);
    return acf;
}

TEST(AutocorrelatorTests, AutocorrelationIsMaxAtIndexZero) {
    auto acf = acfTestSubject();

    auto max_element = std::max_element(acf.begin(), acf.end());
    auto max_idx = std::distance(acf.begin(), max_element);
    EXPECT_EQ(max_idx, 0);
}

TEST(AutocorrelatorTests, AutocorrelationHasLocalMaxAtOriginalSignalPeriod) {
    auto acf = acfTestSubject();

    auto max_element = std::max_element(acf.begin(), acf.end());
    auto max_idx = std::distance(acf.begin(), max_element);

    auto min_element = std::min_element(acf.begin(), acf.end());
    auto next_max_element = std::max_element(min_element, acf.end());
    auto period_idx = std::distance(acf.begin(), next_max_element);
    EXPECT_NEAR(period_idx, 50, 2);
}

};  // namespace tms_express
