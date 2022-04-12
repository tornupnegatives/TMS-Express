//
// Created by Joseph Bellahcen on 4/11/22.
//

#include "Autocorrelator.h"
#include <cstdlib>

Autocorrelator::Autocorrelator() {
    xcorr = nullptr;
    normalizedXcorr = nullptr;
    size = 0;
}

Autocorrelator::~Autocorrelator() {
    if (xcorr != nullptr)
        free(xcorr);

    if (normalizedXcorr != nullptr)
        free(normalizedXcorr);
}

// Calculates the autocorrelation r_xx of the samples
//
// When looking through a small enough window, a segment
// of speech data is nearly periodic. As such, the
// autocorrelation will also be periodic. This property is useful
// for both performing pitch estimation and computing the LPC coefficients
//
// The result is stored as a class member rather than returned
// such that the Autocorrelator can free it automatically
void Autocorrelator::autocorrelation(float *samples, int size) {
    // Discard previous computations
    if (xcorr != nullptr)
        free(xcorr);

    if (normalizedXcorr != nullptr)
        free(normalizedXcorr);

    xcorr = (float *) malloc(sizeof(float) * size);
    normalizedXcorr = (float *) malloc(sizeof(float) * size);

    // Compute autocorrelation
    for (int i = 0; i < size; i++) {
        float sum = 0.0;

        for (int j = 0; j < size - i; j++) {
            sum += samples[j] * samples[j + i];
        }

        xcorr[i] = sum;
    }

    // Store normalized result
    for (int i = 0; i < size; i++)
        normalizedXcorr[i] = xcorr[i] / xcorr[0];

    Autocorrelator::size = size;
}

float *Autocorrelator::result(int *size) {
    if (xcorr != nullptr) {
        *size = Autocorrelator::size;
        return xcorr;
    } else {
        *size = 0;
        return nullptr;
    }
}

float *Autocorrelator::normalizedResult(int *size) {
    if (normalizedXcorr != nullptr) {
        *size = Autocorrelator::size;
        return normalizedXcorr;
    } else {
        *size = 0;
        return nullptr;
    }
}
