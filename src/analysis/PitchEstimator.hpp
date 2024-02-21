// Copyright 2023 Joseph Bellahcen <joeclb@icloud.com>

#ifndef TMS_EXPRESS_LPC_ANALYSIS_PITCHESTIMATOR_HPP_
#define TMS_EXPRESS_LPC_ANALYSIS_PITCHESTIMATOR_HPP_

#include <vector>

#include "audio/AudioBuffer.hpp"

namespace tms_express {

/// @brief Estimates pitch of sample using its autocorrelation
class PitchEstimator {
 public:
    ///////////////////////////////////////////////////////////////////////////
    // Initializers ///////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Creates a new autocorrelation-based Pitch Estimator
    /// @param sample_rate_hz Sample rate of data to analyze
    /// @param min_frq_hz Minimum frequency to detect, in Hertz
    /// @param max_frq_hz Maximum frequency to detect, in Hertz
    explicit PitchEstimator(int sample_rate_hz, int min_frq_hz = 50,
        int max_frq_hz = 500);

    ///////////////////////////////////////////////////////////////////////////
    // Accessors //////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Accesses the max pitch period, in samples
    /// @return Max pitch period, in samples
    int getMaxPeriod() const;

    /// @brief Sets the max pitch period
    /// @param min_frq_hz Min pitch frequency, in Hertz
    void setMaxPeriod(int min_frq_hz);

    /// @brief Accesses the max pitch frequency, in Hertz
    /// @return Max pitch frequency, in Hertz
    int getMaxFrq() const;

    /// @brief Accesses the min pitch period, in samples
    /// @return Min pitch period, in samples
    int getMinPeriod() const;

    /// @brief Sets the min pitch period
    /// @param min_frq_hz Max pitch frequency, in Hertz
    void setMinPeriod(int max_frq_hz);

    /// @brief Accesses the min pitch frequency, in Hertz
    /// @return Min pitch frequency, in Hertz
    int getMinFrq() const;

    ///////////////////////////////////////////////////////////////////////////
    // Pitch Estimation ///////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Estimate the frequency of sample given autocorrelation
    /// @param acf Autocorrelation of sample
    /// @return Estimated pitch frequency, in Hertz
    float estimateFrequency(const std::vector<float> &acf) const;

    /// @brief Estimate the period of sample given autocorrelation
    /// @param acf Autocorrelation of sample
    /// @return Estimated pitch period, in samples
    int estimatePeriod(const std::vector<float> &acf) const;

 private:
    ///////////////////////////////////////////////////////////////////////////
    // Members ////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    int max_period_;

    int min_period_;

    int sample_rate_hz_;
};

};  // namespace tms_express

#endif  // TMS_EXPRESS_LPC_ANALYSIS_PITCHESTIMATOR_HPP_
