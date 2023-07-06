// Copyright 2023 Joseph Bellahcen <joeclb@icloud.com>

#ifndef LPC_ANALYSIS_AUTOCORRELATION_H_
#define LPC_ANALYSIS_AUTOCORRELATION_H_

#include <vector>

namespace tms_express {

/// @brief Compute biased autocorrelation of segment
///
/// @param segment Segment from which to compute autocorrelation
/// @return Biased autocorrelation of segment
std::vector<float> Autocorrelation(const std::vector<float> &segment);

};  // namespace tms_express

#endif  // LPC_ANALYSIS_AUTOCORRELATION_H_
