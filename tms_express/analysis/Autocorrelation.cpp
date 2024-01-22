// Copyright 2023 Joseph Bellahcen <joeclb@icloud.com>

#include "analysis/Autocorrelation.hpp"

#include <vector>

namespace tms_express {

std::vector<float> Autocorrelation(const std::vector<float> &segment) {
    auto size = segment.size();
    auto acf = std::vector<float>(size);

    for (int i = 0; i < static_cast<int>(size); i++) {
        float sum = 0.0f;

        for (int j = 0; j < static_cast<int>(size) - i; j++) {
            sum += segment[j] * segment[j + i];
        }

        acf[i] = (sum / static_cast<float>(size));
    }

    return acf;
}

};  // namespace tms_express
