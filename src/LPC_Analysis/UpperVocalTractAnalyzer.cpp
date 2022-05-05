//
// Created by Joseph Bellahcen on 4/16/22.
//

#include <cstdlib>
#include <cmath>
#include <cstdio>
#include "LPC_Analysis/UpperVocalTractAnalyzer.h"

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
float *UpperVocalTractAnalyzer::lpcCoefficients(float *xcorr, float *loss) {
    auto k = (float *) malloc(sizeof(float) * (order + 1));
    float *r = xcorr;

    // Error
    float e[order + 1];

    // Coefficient matrix
    // TODO: Reduce to 2D array
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

    *loss = e[order];
    return k;
}

// Predict the log-energy of the LPC-encoded signal
//
// In the context of LPC, the gain may be expressed
// as the root-means-squared deviation
//
// The gain is expressed on the decibel scale
float UpperVocalTractAnalyzer::gain(float *segment, float loss) {
    // Compute the energy of the original signal
    float signalEnergy = 0.0f;
    for (int j = 0; j < samplesPerSegment; j++) {
        signalEnergy += segment[j] * segment[j];
    }

    signalEnergy /= samplesPerSegment;

    // Compute RMS deviation
    //
    // The variance is a means-squared quantity, but
    // the explicit computation of the sqaure root
    // may be omitted due to the properties of the
    // log function
    //
    // The constant 10e-10 prevents calculating log(0)
    float variance = signalEnergy * loss;
    float gain = 10.f * abs(log10(variance + 10e-10));

    return gain;
}
