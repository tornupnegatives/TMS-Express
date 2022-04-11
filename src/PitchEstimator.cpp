//
// Created by Joseph Bellahcen on 4/6/22.
//
#include <array>
#include <cstdlib>
#include "PitchEstimator.h"

PitchEstimator::PitchEstimator(int sampleRate) {
    PitchEstimator::sampleRate = sampleRate;
}

int PitchEstimator::estimatePeriod(float *segment, int size) {
    // Step 1. Compute autocorrelation for entire signal
    float *xcorr = autocorrelation(segment, size, 0);

    // Step 2. Find the peak of the autocorrelation
    //
    // Since the maximum value of the function will be at xcorr[0],
    // the first minimum must be found first. Then, the NEXT maximum
    // is found.
    //
    // The distance between the first and second maxima corresponds to
    // the pitch period of the segment
    int minIdx = std::distance(xcorr, std::min_element(xcorr, xcorr + size));
    int period = std::distance(xcorr, std::max_element(xcorr + minIdx, xcorr + size));

    free(xcorr);

    // Step 3. Convert period to frequency
    // float frequency = (float) sampleRate / (float) maxIdx;

    return period;
}

// Calculates the autocorrelation r_xx of the samples
//
// Because speech data is nearly periodic, the autocorrelation
// can be used to estimate its pitch
float *PitchEstimator::autocorrelation(float *samples, int size, int lag) {
    auto *xcorr = (float *) malloc(sizeof(float) * size);

    for (int i = 0; i < size; i++) {
        float sum = 0.0;

        for (int j = 0; j < size - i; j++) {
            sum += samples[j] * samples[j + i];
        }

        xcorr[i] = sum;
    }

    return xcorr;
}