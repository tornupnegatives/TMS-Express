//
// Created by Joseph Bellahcen on 4/6/22.
//

#include "Audio/AudioBuffer.h"

#include <samplerate.h>
#include <sndfile.hh>

// Initialize an AudioBuffer from a file
//
// Also mixes to mono, downsamples to the
// target sample rate, and zero-pads for
// segmentation
AudioBuffer::AudioBuffer(const char *path, int targetSampleRate, float windowSize) {
    // Import audio file
    SndfileHandle file = SndfileHandle(path);

    // Gather metadata
    AudioBuffer::size = (int) file.frames() * file.channels();
    AudioBuffer::sampleRate = file.samplerate();
    AudioBuffer::channels = file.channels();

    // Read samples
    samples = (float *) malloc(sizeof(float) * size);
    __attribute__((unused)) sf_count_t readSamples = file.read(samples, size);

    // Mixdown & resample
    mixdown();
    resample(targetSampleRate);

    // Calculate number of segments
    samplesPerSegment = (int) (windowSize * 1e-3 * sampleRate);
    numSegments = size / samplesPerSegment;

    // Pad final segment
    padFinalSegment();
}

AudioBuffer::~AudioBuffer() {
    if (samples != nullptr) {
        free(samples);
    }
}

int AudioBuffer::getSize() const {
    return size;
}

int AudioBuffer::getSampleRate() const {
    return sampleRate;
}

int AudioBuffer::getSamplesPerSegment() const {
    return samplesPerSegment;
}

int AudioBuffer::getNumSegments() const {
    return numSegments;
}

// Return the entire array of audio samples
float *AudioBuffer::getSamples() {
    if (samples != nullptr) {
        return samples;
    } else {
        return nullptr;
    }
}

// Return a segment of audio for analysis
float *AudioBuffer::getSegment(int i) {
    if (i < numSegments) {
        return samples + (samplesPerSegment * i);
    } else {
        return nullptr;
    }
}

int AudioBuffer::frames() const {
    return size / channels;
}

// Mix AudioBuffer to mono
void AudioBuffer::mixdown() {
    auto mono = (float *) calloc(frames(), sizeof(float ));

    for (int frame = 0; frame < frames(); frame++) {
        for (int channel = 0; channel < channels; channel++) {
            mono[frame] += samples[frame * channels + channel];
        }

        mono[frame] /= (float) channels;
    }

    free(samples);
    size = frames();
    channels = 1;
    samples = mono;
}

void AudioBuffer::resample(int targetSampleRate) {
    // Prepare resampler
    double ratio = (double) targetSampleRate / (double) sampleRate;
    int newFrames = (int) ((double) size / (double) channels * ratio);
    auto newSamples = (float *) malloc(sizeof(float) * frames() * channels);

    // Initialize resampler
    auto resampler = (SRC_DATA *) malloc(sizeof(SRC_DATA));
    resampler->data_in = samples;
    resampler->input_frames = frames();
    resampler->data_out = newSamples;
    resampler->output_frames = newFrames;
    resampler->src_ratio = ratio;

    // Resample
    src_simple(resampler, SRC_SINC_BEST_QUALITY, channels);

    // Update class members
    free(samples);
    size = newFrames * channels;
    sampleRate = targetSampleRate;
    samples = newSamples;
    free(resampler);
}

// Pad the final segment with zeros for ease of calculation
void AudioBuffer::padFinalSegment() {
    int segmentedSize = samplesPerSegment * numSegments;

    if (size < segmentedSize) {
        // Allocate room for an extra segment
        auto paddedSamples = (float *) calloc(segmentedSize, sizeof(float));
        memcpy(paddedSamples, samples, sizeof(float) * size);

        // Update metadata
        free(samples);
        numSegments++;
        size = segmentedSize;
        samples = paddedSamples;
    }
}
