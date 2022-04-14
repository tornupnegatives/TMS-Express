//
// Created by Joseph Bellahcen on 4/11/22.
//

#include "CoefficientSolver.h"
#include <cstdlib>

CoefficientSolver::CoefficientSolver(int order) {
    CoefficientSolver::order = order;
    coefficients = (float *) malloc(sizeof(float) * (order + 1));
}

CoefficientSolver::~CoefficientSolver() {
    if (coefficients != nullptr) {
        free(coefficients);
    }
}

// Calculates the reflection coefficients
//
// The reflection coefficients describe a zero-pole
// filter which can be used to model the human vocal tract
//
// The Durbin algorithm is adapted from:
// https://www.dsprelated.com/showthread/comp.dsp/134663-2.php
void CoefficientSolver::solve(float *r) {
    float a[order + 1];
    float a_tmp[order + 1];
    float alpha, epsilon;

    coefficients[0] = 0.0;
    a[0] = 1.0;
    alpha = r[0];

    for (int i = 1; i <= order; i++) {
        epsilon = r[i];

        for (int j = 1; j < i; j++) {
            epsilon += a[j] * r[i - j];
        }

        a[i] = coefficients[i] = -epsilon / alpha;
        alpha = alpha * (1.0 - coefficients[i] * coefficients[i]);

        for (int j = 1; j < i; j++) {
            a_tmp[j] = a[j] + coefficients[i] * a[i-j];
        }

        for (int j = 1; j < i; j++) {
            a[j] = a_tmp[j];
        }
    }
}

float *CoefficientSolver::getReflectionCoefficients(int *size) {
    if (coefficients != nullptr) {
        *size = order + 1;
        return  coefficients;
    } else {
        *size = 0;
        return nullptr;
    }
}