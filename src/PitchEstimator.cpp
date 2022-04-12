//
// Created by Joseph Bellahcen on 4/6/22.
//
#include <array>
#include "PitchEstimator.h"

// Estimate the pitch period given the autocorrelation of the speech signal
int PitchEstimator::estimatePeriod(float *xcorr, int size) {
    // Since the maximum value of the function will be at xcorr[0],
    // the first minimum must be found first. Then, the NEXT maximum
    // is found.
    //
    // The distance between the first and second maxima corresponds to
    // the pitch period of the segment
    int minIdx = std::distance(xcorr, std::min_element(xcorr, xcorr + size));
    int period = std::distance(xcorr, std::max_element(xcorr + minIdx, xcorr + size));

    return period;
}
