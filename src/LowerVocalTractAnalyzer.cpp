//
// Created by Joseph Bellahcen on 4/16/22.
//

#include <algorithm>
#include <cstdlib>
#include "LowerVocalTractAnalyzer.h"

LowerVocalTractAnalyzer::LowerVocalTractAnalyzer(int numSegments, int samplesPerSegment, float unvoicedThreshold) {
    LowerVocalTractAnalyzer::numSegments = numSegments;
    LowerVocalTractAnalyzer::segmentSize = samplesPerSegment;
    LowerVocalTractAnalyzer::unvoicedThreshold = unvoicedThreshold;

    LowerVocalTractAnalyzer::pitches = (int *) malloc(sizeof(int) * numSegments);
    LowerVocalTractAnalyzer::voicings = (voicing *) malloc(sizeof(voicing) * numSegments);
}

LowerVocalTractAnalyzer::~LowerVocalTractAnalyzer() {
    if (pitches != nullptr)
        free(pitches);

    if (voicings != nullptr)
        free(voicings);
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
void LowerVocalTractAnalyzer::estimatePitch(int i, float *xcorr) {
    int minIdx = std::distance(xcorr, std::min_element(xcorr, xcorr + segmentSize));
    int period = std::distance(xcorr, std::max_element(xcorr + minIdx, xcorr + segmentSize));

    LowerVocalTractAnalyzer::pitches[i] = period;
}

// Determine whether the given segment is voiced (vowel) or unvoiced (consonant)
//
//
void LowerVocalTractAnalyzer::detectVoicing(int i, float *xcorr) {

}

int *LowerVocalTractAnalyzer::getPitches() {
    return pitches;
}
