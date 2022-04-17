//
// Created by Joseph Bellahcen on 4/11/22.
//

#include "Autocorrelator.h"
#include <cstdlib>

Autocorrelator::Autocorrelator(int samplesPerSegment) {
    Autocorrelator::size = samplesPerSegment;
}

// Calculate the autocorrelation r_xx of the samples
//
// To further aid in analysis, the autocorrelation
// is normalized by default
float *Autocorrelator::autocorrelation(float *segment, bool normalize) {
    auto xcorr = (float *) malloc(sizeof(float) * size);

    for (int i = 0; i < size; i++) {
        float sum = 0.0;

        for (int j = 0; j < size - i; j++) {
            sum += segment[j] * segment[j + i];
        }

        xcorr[i] = sum;
    }

    if (normalize) {
        float scale = xcorr[0];
        for (int i = 0; i < size; i++) {
            xcorr[i] /= scale;
        }
    }

    return  xcorr;
}
