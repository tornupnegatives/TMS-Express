//
// Created by Joseph Bellahcen on 4/16/22.
//

#include "LPC_Analysis/LowerVocalTractAnalyzer.h"

#include <algorithm>

LowerVocalTractAnalyzer::LowerVocalTractAnalyzer(int samplesPerSegment, int sampleRate, int minPitchHz, int maxPitchHz, float unvoicedThreshold) {
    LowerVocalTractAnalyzer::samplesPerSegment = samplesPerSegment;
    LowerVocalTractAnalyzer::minPitchPeriod = sampleRate / maxPitchHz;
    LowerVocalTractAnalyzer::maxPitchPeriod = sampleRate/ minPitchHz;
    LowerVocalTractAnalyzer::unvoicedThreshold = unvoicedThreshold;
}

// Estimate the pitch period of the segment from its autocorrelation
//
// Because a small enough segment of speech is roughly periodic,
// the autocorrelation will also be periodic. This makes it a
// useful estimator of pitch
int LowerVocalTractAnalyzer::estimatePitch(float *xcorr) {
    // Restrict search window to autocorrelation lags which
    // are within the target pitch periods
    float *xcorrStart = xcorr + minPitchPeriod;
    float *xcorrEnd = xcorr + minPitchPeriod + maxPitchPeriod;

    // Identify the maximum element in the pitch window and find its distance
    // from the start of the array
    int period = std::distance(xcorr, std::max_element(xcorrStart, xcorrEnd));

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
LowerVocalTractAnalyzer::voicing LowerVocalTractAnalyzer::detectVoicing(int pitch, float *xcorr) {
    float  ratio = xcorr[pitch] / xcorr[0];

    // TODO: Also check against min pitch period

    if (ratio >= unvoicedThreshold) {
        return VOICED;
    } else {
        return UNVOICED;
    }
}
