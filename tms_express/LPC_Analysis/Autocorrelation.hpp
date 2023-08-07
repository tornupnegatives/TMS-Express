// Copyright 2023 Joseph Bellahcen <joeclb@icloud.com>

#ifndef TMS_EXPRESS_LPC_ANALYSIS_AUTOCORRELATION_HPP_
#define TMS_EXPRESS_LPC_ANALYSIS_AUTOCORRELATION_HPP_

#include <vector>

namespace tms_express {

/// @brief Computes biased autocorrelation of segment
///
/// @param segment Segment from which to compute autocorrelation
/// @return Biased autocorrelation of segment
std::vector<float> Autocorrelation(const std::vector<float> &segment);

};  // namespace tms_express

#endif  // TMS_EXPRESS_LPC_ANALYSIS_AUTOCORRELATION_HPP_
