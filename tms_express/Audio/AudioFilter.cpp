///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class: AudioFilter
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

#include "Audio/AudioFilter.hpp"

#include <cmath>
#include <vector>

namespace tms_express {

void AudioFilter::applyHammingWindow(AudioBuffer &buffer) {
    for (auto &segment : buffer.getAllSegments()) {
        applyHammingWindow(segment);
    }
}

void AudioFilter::applyHammingWindow(std::vector<float> &segment) {
    auto size = segment.size();

    for (int i = 0; i < size; i++) {
        float theta = 2.0f * M_PI * i / size;
        // TODO(Joseph Bellahcen): Make smearing coefficient alpha configurable
        float window = 0.54f - 0.46f * cosf(theta);
        segment[i] *= window;
    }
}

void AudioFilter::applyHighpass(AudioBuffer &buffer, int cutoff_hz) {
    computeCoeffs(HPF, cutoff_hz);
    applyBiquad(buffer);
}

void AudioFilter::applyLowpass(AudioBuffer &buffer, int cutoff_hz) {
    computeCoeffs(LPF, cutoff_hz);
    applyBiquad(buffer);
}

void AudioFilter::applyPreEmphasis(AudioBuffer &buffer, float alpha) {
    // Initialize filtered buffer
    auto samples = buffer.getSamples();
    auto filteredSamples = std::vector<float>();

    float previous = samples[0];
    filteredSamples.push_back(previous);

    // Apply filter
    // y(t) = x(t) - a * x(t-1)
    for (int i = 1; i < samples.size(); i++) {
        auto new_sample = samples[i] - alpha * samples[i - 1];
        filteredSamples.push_back(new_sample);
    }

    // Store filtered samples
    buffer.setSamples(filteredSamples);
}

void AudioFilter::applyBiquad(AudioBuffer &buffer) {
    // Rename coefficients for readability
    float k0 = coeffs[0];
    float k1 = coeffs[1];
    float k2 = coeffs[2];
    float k3 = coeffs[3];
    float k4 = coeffs[4];
    float normalizationCoeff = coeffs[5];

    // Initialize filtered buffer
    std::vector<float> samples = buffer.getSamples();
    float x1 = 0, x2 = 0;
    float y1 = 0, y2 = 0;

    // Apply filter
    for (float &sample : samples) {
        float result = (k0 * sample) + (k1 * x1) +
                        (k2 * x2) - (k3 * y1)- (k4 * y2);
        result /= normalizationCoeff;

        x2 = x1;
        x1 = sample;
        y2 = y1;
        y1 = result;

        sample = result;
    }

    // Store filtered samples
    buffer.setSamples(samples);
}

void AudioFilter::computeCoeffs(AudioFilter::FilterMode mode, int cutoff_hz) {
    // Filter-agnostic parameters
    float omega = 2.0f * M_PI * cutoff_hz / 8000.0f;
    float cs = cosf(omega);
    float sn = sinf(omega);
    float alpha = sn / (2.0f * 0.707f);

    // Intermediate coefficients
    float aCoeff[3];
    float bCoeff[3];

    switch (mode) {
        case HPF:
            aCoeff[0] = 1.0f + alpha;
            aCoeff[1] = -2.0f * cs;
            aCoeff[2] = 1.0f - alpha;

            bCoeff[0] = (1.0f + cs) / 2.0f;
            bCoeff[1] = -(1.0f + cs);
            bCoeff[2] = bCoeff[0];
            break;

        case LPF:
            aCoeff[0] = 1.0f + alpha;
            aCoeff[1] = -2.0f * cs;
            aCoeff[2] = 1.0f - alpha;

            bCoeff[0] = (1.0f - cs) / 2.0f;
            bCoeff[1] = 1.0f - cs;
            bCoeff[2] = bCoeff[0];
            break;
    }

    // Filter coefficients
    coeffs[0] = bCoeff[0];
    coeffs[1] = bCoeff[1];
    coeffs[2] = bCoeff[2];
    coeffs[3] = aCoeff[1];
    coeffs[4] = aCoeff[2];

    // Normalization coefficient
    coeffs[5] = aCoeff[0];
}

};  // namespace tms_express
