// Copyright 2023 Joseph Bellahcen <joeclb@icloud.com>

#ifndef TMS_EXPRESS_LPC_ANALYSIS_LINEARPREDICTOR_HPP_
#define TMS_EXPRESS_LPC_ANALYSIS_LINEARPREDICTOR_HPP_

#include <vector>

namespace tms_express {

/// @brief Performs upper-vocal-tract analysis, yielding LPC reflector
///         coefficients and prediction error
class LinearPredictor {
 public:
    ///////////////////////////////////////////////////////////////////////////
    // Initializers ///////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Creates new Linear Predictor with given model order
    /// @param modelOrder Model order, corresponding to number of filter poles
    explicit LinearPredictor(int modelOrder = 10);

    ///////////////////////////////////////////////////////////////////////////
    // Linear Prediction //////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Computes LPC reflector coefficients of given autocorrelation
    /// @param acf Autocorrelation corresponding to a segment of speech data
    /// @return Vector of n_pole LPC reflector coefficients
    std::vector<float> computeCoeffs(const std::vector<float> &acf);

    /// @brief Computes gain from prediction error
    /// @return Prediction gain, in decibels
    /// @warning This function must not be called before
    ///             LinearPredictor::computeCoeffs()
    float gain() const;

 private:
    ///////////////////////////////////////////////////////////////////////////
    // Members ////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Model order, corresponding to the number of poles in the LPC
    ///         lattice filter
    int order_;

    /// @brief Prediction error, which is used to model the gain of the signal
    float error_;
};

};  // namespace tms_express

#endif  // TMS_EXPRESS_LPC_ANALYSIS_LINEARPREDICTOR_HPP_
