///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class: LinearPredictor
//
// Description: The LinearPredictor performs upper vocal tract analysis on a speech segment, determining both the
//              coefficients of an M-th order linear system which define the vocal tract and the prediction gain
//
// Author: Joseph Bellahcen <joeclb@icloud.com>
//
// Acknowledgement: The Levinson-Durbin recursion algorithm used to find the LPC coefficients is adapted from the paper
//                  "Levinson–Durbin Algorithm" (Castiglioni)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "LPC_Analysis/LinearPredictor.h"
#include <cmath>
#include <vector>

/// Create a new LPC solver of the given order
///
/// \param modelOrder Order of the LPC model for which to solve (typically 10)
LinearPredictor::LinearPredictor(int modelOrder) {
    order = modelOrder;
    error = 0.0f;
}

/// Compute the LPC reflector coefficients of the speech signal
///
/// \note   The behavior fo the oral cavity during a segment of speech may be approximated by an M-th order all-pole
///         filter. The reflector coefficients of the filter minimize the energy of the output signal. There are many
///         equivalent algorithms for solving the linear system, which consists of an MxM Toeplitz matrix A, the
///         coefficient vector b, and the autocorrelation coefficients c. The algorithm implemented below is the
///         Levinson-Durbin recursion
std::vector<float> LinearPredictor::reflectorCoefficients(const std::vector<float>& acf) {
    // Model parameters
    //
    // r: autocorrelation
    // k: reflector coefficients
    // e: error
    // b: coefficient matrix
    auto r = acf;
    auto k = std::vector<float>(order + 1);
    auto e = std::vector<float>(order + 1);
    auto b = std::vector<std::vector<float>>(order + 1, std::vector<float>(order + 1));

    e[0] = r[0];
    k[0] = 0.0f;

    for (int m = 1; m <= order; m++) {
        float sum = r[m];
        for (int i = 1; i < m; i++) {
            sum += b[m - 1][i] * r[m - i];
        }

        b[m][m] = k[m] = -sum / e[m - 1];
        e[m] = e[m - 1] * (1 - b[m][m] * b[m][m]);

        for (int i = 1; i < m; i++) {
            b[m][i] = b[m - 1][i] + b[m][m] * b[m - 1][m - i];
        }
    }

    // The error member is the squared gain factor of the prediction
    error = e[order - 1];
    auto reflectors = std::vector<float>(k.begin() + 1, k.end());
    return reflectors;
}

/// Compute the prediction gain of the segment
///
/// \note   The gain of the signal may be expressed as the ratio of the original signal energy and the residual error,
///         which is the final error coefficient. This error is scaled by a reference intensity and then expressed on
///         the decibel scale
///
/// \source http://www.sengpielaudio.com/calculator-soundlevel.htm
float LinearPredictor::gain() const {
    // 10 * log10(x) == 20 * log10(sqrt(x))
    float gain = 10.0f * log10f(error / 1e-12f);
    return abs(gain);
}
