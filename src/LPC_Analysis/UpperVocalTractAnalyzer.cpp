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
// During LPC encoding, the signal loses some of its
// energy. The residual energy which remains can be
// easily found by taking the original energy and
// multiplying it by the loss proportion
//
// The energy can be expressed on the decibel scale
// for ease of quantization
float UpperVocalTractAnalyzer::gain(float *segment, float loss) {
    // Compute the energy of the original signal
    float signalEnergy = 0.0f;
    for (int j = 0; j < samplesPerSegment; j++) {
        signalEnergy += segment[j] * segment[j];
    }

    // Per the nature of compression, some information
    // about the original signal (energy) will be lost.
    // What remains is known as the residual energy
    float residualEnergy = signalEnergy * loss;

    // Convert the energy to power
    float power = residualEnergy / samplesPerSegment;

    // Express power on the decibel scale
    float gain = 10.f * abs(log10(power));

    return gain;
}