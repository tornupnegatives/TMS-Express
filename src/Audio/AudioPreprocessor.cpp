///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class: Filter
//
// Description: The Filterer applies filters to audio data to reduce the impact of unnecessary frequency components
//              on speech signal analysis. The class implements biquadratic highpass and lowpass filters, as well as
//              a pre-emphasis filter. All filters operate in the time-domain
//
// Author: Joseph Bellahcen <joeclb@icloud.com>
//
// Acknowledgement: The bi-quadratic filter algorithms come from Robert Bristow-Johnson <robert@audioheads.com>.
//                  Additional information about digital biquad filters may be found at
//                  https://webaudio.github.io/Audio-EQ-Cookbook/audio-eq-cookbook.html
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Audio/AudioPreprocessor.h"
#include <cmath>
#include <vector>

AudioPreprocessor::AudioPreprocessor() {
    lastFilterMode = FILTER_NONE;
    lastCutoffHz = 0;
    coeffs = {0, 0, 0, 0, 0};
}

// Apply a time-domain biquad highpass/lowpass filter to the samples
//
// As pitch is a low-frequency property of speech, applying a lowpass filter before pitch detection will lead to more
// accurate predictions
void AudioPreprocessor::applyBiquad(AudioBuffer &buffer, unsigned int cutoffHz, FilterBiquadMode mode) {
    // Get coefficients and store them individually for convenience
    setCoefficients(mode, cutoffHz);
    float k0 = coeffs[0];
    float k1 = coeffs[1];
    float k2 = coeffs[2];
    float k3 = coeffs[3];
    float k4 = coeffs[4];

    // Initialize filtered buffer
    std::vector<float> samples = buffer.getSamples();
    float x1 = 0, x2 = 0;
    float y1 = 0, y2 = 0;

    // Filter buffer
    for (float &sample : samples) {
        float result = (k0 * sample) + (k1 * x1) + (k2 * x2) - (k3 * y1) - (k4 * y2);

        x2 = x1;
        x1 = sample;
        y2 = y1;
        y1 = result;

        sample = result;
    }

    // Store filtered samples
    buffer.setSamples(samples);
}

// Apply a time-domain pre-emphasis filter to the samples
//
// During LPC analysis, a pre-emphasis filter balances the signal spectrum by augmenting high-frequencies. This is
// undesirable for pitch estimation, but greatly improves the accuracy of linear prediction
void AudioPreprocessor::applyPreemphasis(AudioBuffer &buffer, float alpha) {
    // Initialize filtered buffer
    std::vector<float> samples = buffer.getSamples();
    std::vector<float> filteredSamples = std::vector<float>();

    float previous = samples[0];
    filteredSamples.push_back(previous);

    // y(t) = x(t) - a * x(t-1)
    // A typical value for |a| = 15/16 = 0.9375
    for (int i = 1; i < samples.size(); i++) {
        float preEmphasized = samples[i] - alpha * samples[i - 1];
        filteredSamples.push_back(preEmphasized);
    }

    lastFilterMode = FILTER_NONE;
    buffer.setSamples(filteredSamples);
}

// Apply Hamming window to audio segment
//
// Because speech segments in isolation cannot convey information about the transition between each other, a windowing
// function is necessary to smooth their boundaries. This is accomplished by "smearing" the spectrum of the segments
// by a known alpha value. For the Hamming Window, which is preferred for speech signal analysis, a=0.54
void AudioPreprocessor::applyHammingWindow(std::vector<float> &segment) {
    auto size = segment.size();

    for (int i = 0; i < size; i++) {
        float theta = 2.0f * float(M_PI) * float(i) / float(size);
        float window = 0.54f - 0.46f * cosf(theta);

        segment[i] *= window;
    }

    lastFilterMode = FILTER_NONE;
}

// Determine the biquadratic filter coefficients
void AudioPreprocessor::setCoefficients(FilterBiquadMode mode, unsigned int cutoffHz) {
    // Do not recompute coefficients if the cutoff frequency and mode are unchanged
    if (mode == lastFilterMode && cutoffHz == lastCutoffHz) {
        return;
    }

    // Filter-agnostic parameters
    // Bandwidth: 3dB (in definition of alpha)
    float omega = 2.0f * float(M_PI) * float(cutoffHz) / 8000.0f;
    float cs = cosf(omega);
    float sn = sinf(omega);
    float alpha = sn * sinhf(M_LN2 /2 * 3 * omega /sn);

    // Intermediate coefficients
    float aCoeff[3];
    float bCoeff[3];

    switch (mode) {
        case FILTER_HIGHPASS:
            aCoeff[0] = 1.0f + alpha;
            aCoeff[1] = -2.0f * cs;
            aCoeff[2] = 1.0f - alpha;

            bCoeff[0] = (1.0f + cs) / 2.0f;
            bCoeff[1] = -(1.0f + cs);
            bCoeff[2] = bCoeff[0];
            break;

        case FILTER_LOWPASS:
            aCoeff[0] = 1.0f + alpha;
            aCoeff[1] = -2.0f * cs;
            aCoeff[2] = 1.0f - alpha;

            bCoeff[0] = (1.0f - cs) / 2.0f;
            bCoeff[1] = 1.0f - cs;
            bCoeff[2] = bCoeff[0];
            break;

        default:
            for (int i = 0; i < 3; i++) {
                aCoeff[i] = bCoeff[i] = 0.0f;
            }

            break;
    }

    // Store normalized coefficients
    coeffs[0] = bCoeff[0] / aCoeff[0];
    coeffs[1] = bCoeff[1] / aCoeff[0];
    coeffs[2] = bCoeff[2] / aCoeff[0];
    coeffs[3] = aCoeff[1] / aCoeff[0];
    coeffs[4] = aCoeff[2] / aCoeff[0];

    // Record mode and cutoff
    lastFilterMode = mode;
    lastCutoffHz = cutoffHz;
}