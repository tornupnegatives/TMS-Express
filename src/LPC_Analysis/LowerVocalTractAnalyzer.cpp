//
// Created by Joseph Bellahcen on 4/16/22.
//

#include "LPC_Analysis/LowerVocalTractAnalyzer.h"

#include <algorithm>
#include <cmath>

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
    // Restrict search window to autocorrelation lags which are within the target pitch periods
    float *xcorrStart = xcorr + minPitchPeriod;
    float *xcorrEnd = xcorr + minPitchPeriod + maxPitchPeriod;

    // Identify the first local minimum and subsequent local maximum
    // The distance between these values likely corresponds to the pitch of the segment
    auto firstLocalMin = std::min_element(xcorrStart, xcorrEnd);
    int period = (int) std::distance(xcorr, std::max_element(firstLocalMin, xcorrEnd));

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
Voicing LowerVocalTractAnalyzer::detectVoicing(float *segment, float energy, float *xcorr, float gain, int pitchPeriod) {
    float magnitude = 0.0f;
    for (int i = 0; i < samplesPerSegment; i++) {
        magnitude += abs(segment[i]);
    }

    // TODO: Gold-Rabiner pitch detector FIRST, then Use Gold-Rabiner voicing score
    // TODO: check if K1 > unvoicedThreshold (per LPC by Peter Richards)
    float k = xcorr[1] / xcorr[0];

    if (pitchPeriod > maxPitchPeriod)
        return UNVOICED;

    float avg = (0.6f * magnitude) + (0.4f * k);

    if (avg> unvoicedThreshold) {
        return VOICED;
    } else {
        return UNVOICED;
    }
}
