//
// Created by Joseph Bellahcen on 4/11/22.
//

#include "LPC_Analysis/Autocorrelator.h"
#include <cstdlib>

Autocorrelator::Autocorrelator(int samplesPerSegment) {
    Autocorrelator::samplesPerSegment = samplesPerSegment;
}

// Calculate the autocorrelation r_xx of the samples
//
// To further aid in analysis, the autocorrelation
// is normalized by default
float *Autocorrelator::autocorrelation(float *segment, bool normalize) {
    auto xcorr = (float *) malloc(sizeof(float) * samplesPerSegment);

    for (int i = 0; i < samplesPerSegment; i++) {
        float sum = 0.0;

        for (int j = 0; j < samplesPerSegment - i; j++) {
            sum += segment[j] * segment[j + i];
        }

        xcorr[i] = sum;
    }

    if (normalize) {
        float scale = xcorr[0];
        for (int i = 0; i < samplesPerSegment; i++) {
            xcorr[i] /= scale;
        }
    }

    return  xcorr;
}
