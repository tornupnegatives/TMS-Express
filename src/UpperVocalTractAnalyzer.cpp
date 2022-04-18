//
// Created by Joseph Bellahcen on 4/16/22.
//

#include <cstdlib>
#include <cmath>
#include "UpperVocalTractAnalyzer.h"

UpperVocalTractAnalyzer::UpperVocalTractAnalyzer(int numSegments, int samplesPerSegment, int order) {
    UpperVocalTractAnalyzer::numSegments = numSegments;
    UpperVocalTractAnalyzer::segmentSize = samplesPerSegment;
    UpperVocalTractAnalyzer::order = order;

    UpperVocalTractAnalyzer::reflectorCoefficients = (float **) malloc(sizeof(float *) * numSegments);
    for (int i = 0; i < numSegments; i++)
        UpperVocalTractAnalyzer::reflectorCoefficients[i] = (float *) malloc(sizeof(float ) * (order + 1));

    UpperVocalTractAnalyzer::energies = (float *) malloc(sizeof(float) * numSegments);
}

UpperVocalTractAnalyzer::~UpperVocalTractAnalyzer() {
    for (int i = 0; i < numSegments; i++) {
        if (reflectorCoefficients[i] != nullptr)
            free(reflectorCoefficients[i]);
    }

    if (reflectorCoefficients != nullptr) {
        free(reflectorCoefficients);
    }

    if (energies != nullptr) {
        free(energies);
    }
}

// Compute the LPC coefficients of the speech signal
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
void UpperVocalTractAnalyzer::lpcAnalysis(int i, float *xcorr) {
    // Rename reflector coefficient and autocorrelation pointers
    // for convenience
    float *k = reflectorCoefficients[i];
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

        b[m][m] = k[m] = -sum/e[m - 1];
        e[m] = e[m - 1] * (1 - b[m][m] * b[m][m]);

        for (int i = 1; i < m; i++) {
            b[m][i] = b[m - 1][i] + b[m][m] * b[m - 1][m - i];
        }
    }
}

void UpperVocalTractAnalyzer::estimateEnergy(int i, float *segment) {
    float sum = 0.0f;
    for (int j = 0; j < segmentSize; j++) {
        sum += segment[j] * segment[j];
    }

    energies[i] = sqrtf(sum / segmentSize);
}

float *UpperVocalTractAnalyzer::getReflectorCoefficients(int i) {
    return reflectorCoefficients[i];
}

float UpperVocalTractAnalyzer::getEnergy(int i) {
    return energies[i];
}
