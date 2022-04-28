//
// Created by Joseph Bellahcen on 4/14/22.
//

#include "Audio/AudioPreprocessor.h"
#include <cmath>

AudioPreprocessor::AudioPreprocessor(AudioBuffer *audioBuffer) {
    AudioPreprocessor::buffer = audioBuffer;
    AudioPreprocessor::size = buffer->getSize();
    AudioPreprocessor::samples = buffer->getSamples();
    AudioPreprocessor::filter_dt = 1.0f / (float) audioBuffer->getSampleRate();
}

// Apply time-domain pre-emphasis filter to audio buffer
//
// Pre-emphasis balances the spectrum by amplifying lower frequencies
//
// y(t) = x(t) - ax(t-1)
// A typical value for |a| = 15/16 = 0.9375
void AudioPreprocessor::preEmphasize(float alpha) {
    float previous = samples[0];

    for (int i = 1; i < size; i++) {
        float temp = samples[i];
        samples[i] -= alpha * previous;
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
void AudioPreprocessor::lowpassFilter(float cutoff) {
    float rc = 1.0f / (2.0f * (float) M_PI * cutoff);
    float a = filter_dt / (rc + filter_dt);

    for (int i = 1; i < size; i++)
        samples[i] = samples[i - 1] + (a * (samples[i] - samples[i - 1]));
}

// Apply high-pass filter to audio buffer
//
// For speech analysis, decreasing the amplitude of certain
// frequencies may lead to more accurate results. The optimum
// frequency band is between 300-3400 Hz
//
// In the time domain, a high/lowpass filter can be implemeted
// as an exponential moving average
//
//  y[i] = a × (y[i−1] + x[i] − x[i−1])
//  a is the smoothing constant and is defined as:
//  a = RC / (RC + dt)
//  RC = 1 / (2*pi*f_cutoff)
//  dt = 1 / f_sampling
void AudioPreprocessor::highpassFilter(float cutoff) {
    float rc = 1.0f / (2.0f * (float) M_PI * cutoff);
    float a = rc / (rc + filter_dt);
    float previousSample = samples[0];

    for (int i = 2; i < size; i++) {
        float temp = samples[i];
        samples[i] = a * (samples[i - 1] + samples[i] - previousSample);
        previousSample = temp;
    }
}

// Apply Hamming window to AudioBuffer segments
//
// See AudioPreprocessor::hammingWindow(float *segment, int size)
void AudioPreprocessor::hammingWindow() {
    for (int i = 0; i < buffer->getNumSegments(); i++) {
        float *segment = buffer->getSegment(i);
        int samplesPerSegment = buffer->getSamplesPerSegment();
        hammingWindow(segment, samplesPerSegment);
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
void AudioPreprocessor::hammingWindow(float *segment, int samplesPerSegment) {
    for (int i = 0; i < samplesPerSegment; i++) {
        float theta = 2.0f * (float) M_PI * (float) i / (float) samplesPerSegment;
        float window = 0.54f - 0.46f * cosf(theta);

        segment[i] *= window;
    }
}
