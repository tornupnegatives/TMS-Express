//
// Created by Joseph Bellahcen on 4/16/22.
//

#include "LPC_Analysis/LowerVocalTractAnalyzer.h"

#include <algorithm>

LowerVocalTractAnalyzer::LowerVocalTractAnalyzer(int numSegments, int samplesPerSegment, float unvoicedThreshold) {
    LowerVocalTractAnalyzer::samplesPerSegment = samplesPerSegment;
    LowerVocalTractAnalyzer::unvoicedThreshold = unvoicedThreshold;
}

// Estimate the pitch period of the segment from its autocorrelation
//
// Because a small enough segment of speech is roughly periodic,
// the autocorrelation will also be periodic. This makes it a
// useful estimator of pitch
//
// The global maximum of the autocorrelation will occur at the
// very first sample. An easy way to find the period between
// this peak and the next local maximum is to find the first
// local minimum, offset the samples to begin there,
// and then find the second local maximum. Then the distance
// between these two addresses can be computed via pointer arithmetic
int LowerVocalTractAnalyzer::estimatePitch(float *xcorr) {
    int minIdx = std::distance(xcorr, std::min_element(xcorr, xcorr + samplesPerSegment));
    int period = std::distance(xcorr, std::max_element(xcorr + minIdx, xcorr + samplesPerSegment));

    return period;
}

// Determine whether the segment is voiced (vowel) or unvoiced (consontant)
//
// This function depends on pitch data and must be run after
// calling estimatePitch()
//
// One way to determine the voicing of the sample is to look at the ratio
// between the initial value and first maximum of the autocorrelation.
// If the sample is nearly periodic, the ratio will be large and
// the segment is likely voiced. Otherwise, if the segment more
// closely resembles white noise, it is likely unvoiced
LowerVocalTractAnalyzer::voicing LowerVocalTractAnalyzer::detectVoicing(int pitch, float xcorr_0) {
    // TODO: Implement min/max pitch ranges
    float  ratio = (float) pitch / xcorr_0;

    if (ratio >= unvoicedThreshold) {
        return VOICED;
    } else {
        return UNVOICED;
    }
}
