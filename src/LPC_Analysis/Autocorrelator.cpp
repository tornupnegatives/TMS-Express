//
// Created by Joseph Bellahcen on 4/11/22.
//

#include "LPC_Analysis/Autocorrelator.h"
#include <cstdlib>

Autocorrelator::Autocorrelator(int samplesPerSegment) {
    Autocorrelator::samplesPerSegment = samplesPerSegment;
}

// Calculate the autocorrelation r_xx of the samples
float *Autocorrelator::autocorrelation(float *segment) {
    auto xcorr = (float *) malloc(sizeof(float) * samplesPerSegment);

    for (int i = 0; i < samplesPerSegment; i++) {
        float sum = 0.0;

        for (int j = 0; j < samplesPerSegment - i; j++) {
            sum += segment[j] * segment[j + i];
        }

        xcorr[i] = sum;
    }

    return  xcorr;
}
