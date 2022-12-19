///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class: AudioFilter
//
// Description: The Filterer applies filters to audio data to reduce the impact of unnecessary frequency components
//              on speech signal analysis. The class implements Butterworth highpass and lowpass filters, as well as
//              a pre-emphasis filter
//
// Author: Joseph Bellahcen <joeclb@icloud.com>
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Audio/AudioFilter.h"
#include "Iir.h"
#include <cmath>
#include <vector>

AudioFilter::AudioFilter(int sampleRateHz, int highpassCutoffHz, int lowpassCutoffHz, float preemphasisAlpha) {
    sampleRate = sampleRateHz;
    highpassCutoff = highpassCutoffHz;
    lowpassCutoff = lowpassCutoffHz;
    alpha = preemphasisAlpha;

    hpf.setup(8000, highpassCutoffHz-1);
    lpf.setup(8000, lowpassCutoffHz-1);

}

std::vector<float> AudioFilter::applyHighpass(std::vector<float> segment) {
    auto filtered = std::vector<float>(segment.size());

    for (int i = 0; i < segment.size(); i++) {
        auto sample = hpf.filter(segment[i]);
        filtered[i] = sample;
    }

    return filtered;
}

std::vector<float> AudioFilter::applyLowpass(std::vector<float> segment) {
    auto filtered = std::vector<float>(segment.size());

    for (int i = 0; i < segment.size(); i++) {
        auto sample = lpf.filter(segment[i]);
        filtered[i] = sample;
    }

    return filtered;
}

// Apply a time-domain pre-emphasis filter to the samples
//
// During LPC analysis, a pre-emphasis filter balances the signal spectrum by augmenting high-frequencies. This is
// undesirable for pitch estimation, but greatly improves the accuracy of linear prediction
std::vector<float> AudioFilter::applyPreemphasis(std::vector<float> segment) const {
    // Initialize filtered buffer
    auto size = segment.size();
    std::vector<float> filtered = std::vector<float>(size);

    // y(t) = x(t) - a * x(t-1)
    // A typical value for |a| = 15/16 = 0.9375
    for (int i = 1; i < size; i++) {
        float preEmphasized = segment[i] - alpha * segment[i - 1];
        filtered[i] = preEmphasized;
    }

    return filtered;
}

// Apply Hamming window to audio segment
//
// Because speech segments in isolation cannot convey information about the transition between each other, a windowing
// function is necessary to smooth their boundaries. This is accomplished by "smearing" the spectrum of the segments
// by a known alpha value. For the Hamming Window, which is preferred for speech signal analysis, a=0.54
std::vector<float> AudioFilter::applyHammingWindow(std::vector<float> segment) const {
    auto segmentSize = segment.size();
    auto filtered = std::vector<float>(segmentSize);

    // The Hamming window is 1.5x larger than the source signal, such that adjacent segments overlap slightly
    auto windowSize = (float(segmentSize) * 1.5f) - 1;

    for (int i = 0; i < segmentSize; i++) {
        float theta = 2.0f * pi * float(i) / windowSize;
        float window = 0.54f - 0.46f * cosf(theta);

        filtered[i] = segment[i] * window;
    }

    return filtered;
}
