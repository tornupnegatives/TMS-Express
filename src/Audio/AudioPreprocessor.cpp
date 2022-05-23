//
// Created by Joseph Bellahcen on 4/14/22.
//

#include "Audio/AudioPreprocessor.h"
#include <cmath>

AudioPreprocessor::AudioPreprocessor(AudioBuffer *audioBuffer) {
    AudioPreprocessor::samplesPerSegment = audioBuffer->getSamplesPerSegment();
    AudioPreprocessor::filter_dt = 1.0f / (float) audioBuffer->getSampleRate();
}

// Apply time-domain pre-emphasis filter to audio buffer
//
// Pre-emphasis balances the spectrum by amplifying lower frequencies
//
// y(t) = x(t) - ax(t-1)
// A typical value for |a| = 15/16 = 0.9375
void AudioPreprocessor::preEmphasize(float *segment, float alpha) {
    float previous = segment[0];

    for (int i = 1; i < samplesPerSegment; i++) {
        float temp = segment[i];
        segment[i] -= alpha * previous;
        previous = temp;
    }
}

// Apply low-pass filter to audio buffer
//
// For speech analysis, decreasing the amplitude of certain
// frequencies may lead to more accurate results. The optimum
// frequency band is between 300-3400 Hz
//
// In the time domain, a high/lowpass filter can be implemented
// as an exponential moving average
//
//  y[i] = y[i-1] + a * (x[i] - y[i-1])
//  a is the smoothing constant and is defined as:
//  a = dt / (RC + dt)
//  RC = 1 / (2*pi*f_cutoff)
//  dt = 1 / f_sampling
void AudioPreprocessor::lowpassFilter(float *segment, float cutoff) {
    float rc = 1.0f / (2.0f * (float) M_PI * cutoff);
    float a = filter_dt / (rc + filter_dt);

    for (int i = 1; i < samplesPerSegment; i++)
        segment[i] = segment[i - 1] + (a * (segment[i] - segment[i - 1]));
}

// Apply high-pass filter to audio buffer
//
// For speech analysis, decreasing the amplitude of certain
// frequencies may lead to more accurate results. The optimum
// frequency band is between 300-3400 Hz
//
// In the time domain, a high/lowpass filter can be implemented
// as an exponential moving average
//
//  y[i] = a × (y[i−1] + x[i] − x[i−1])
//  a is the smoothing constant and is defined as:
//  a = RC / (RC + dt)
//  RC = 1 / (2*pi*f_cutoff)
//  dt = 1 / f_sampling
void AudioPreprocessor::highpassFilter(float *segment, float cutoff) {
    float rc = 1.0f / (2.0f * (float) M_PI * cutoff);
    float a = rc / (rc + filter_dt);
    float previousSample = segment[1];

    for (int i = 2; i < samplesPerSegment; i++) {
        float temp = segment[i];
        segment[i] = a * (segment[i - 1] + segment[i] - previousSample);
        previousSample = temp;
    }
}

// Apply Hamming window to audio segment
//
// Segmenting the audio buffer removes information
// about the transition between segments. A windowing
// function can be used to mitigate this issue by
// "smearing" the spectrum slightly. For LPC analysis,
// the type of window makes little difference, but Hamming
// is preferred
//
// A time-domain Hamming window is implemented as
// w[i] = a - (1 - a) * cos(2*pi*i/N), 0 <= i < N
// where a = 0.54
//
// The window is applied to the signal via multiplication
void AudioPreprocessor::hammingWindow(float *segment) {
    for (int i = 0; i < samplesPerSegment; i++) {
        float theta = 2.0f * (float) M_PI * (float) i / (float) samplesPerSegment;
        float window = 0.54f - 0.46f * cosf(theta);

        segment[i] *= window;
    }
}
