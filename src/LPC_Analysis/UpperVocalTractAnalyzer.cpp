//
// Created by Joseph Bellahcen on 4/16/22.
//

#include "LPC_Analysis/UpperVocalTractAnalyzer.h"

#include <cmath>
#include <cstdlib>

UpperVocalTractAnalyzer::UpperVocalTractAnalyzer(int samplesPerSegment, int order) {
    UpperVocalTractAnalyzer::samplesPerSegment = samplesPerSegment;
    UpperVocalTractAnalyzer::order = order;
}

// Compute the LPC coefficients of the speech signal
//
// The loss parameter represents the proportion of energy removed from
// the signal during compression, known as the residual error. It is
// useful for estimating the gain of the predicted signal
//
// The behavior of the oral cavity during speech may be approximated
// by an M-th order all-pole filter. At the heart of this filter are
// its reflector coefficients, which minimize the energy of the output
// signal
//
// There are many equivalent algorithms for solving the system,
// which involves a pxp Toeplitz matrix A, the coefficient vector b,
// and the autocorrelation coefficients c. The algorithm implemented
// here is the Levinson-Durbin recursion
//
// Source: Levinson–Durbin Algorithm by Paolo Castiglioni (ISBN 0-470-84907-X)
float *UpperVocalTractAnalyzer::lpcCoefficients(float *xcorr, float *error) {
    // Rename autocorrelation for convenience
    float *r = xcorr;

    // Reflector coefficients
    auto k = (float *) malloc(sizeof(float) * (order + 1));

    // Error
    float e[order + 1];

    // Coefficient matrix
    float b[order + 1][order + 1];

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

    *error = e[order];
    return k;
}

// Compute the prediction gain of the segment
//
// Speech compression via linear prediction results in a loss of some
// signal energy as error. This residual may be used to predict the gain
// of the segment by expressing it as a ratio of the original energy.
// However, the original energy is a somewhat meaningless metric, so
// instead the ratio can be taken with respect to an established reference.
// This value is then expressed on the decibel scale via the logarithm
//
// Source: http://www.sengpielaudio.com/calculator-soundlevel.htm
float UpperVocalTractAnalyzer::gain(float predictionError) {
    float gain = 10.0f * log10f(predictionError / 1e-12f);
    return gain;
}
