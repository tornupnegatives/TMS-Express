//
// Created by Joseph Bellahcen on 4/6/22.
//

#include "AudioBuffer.h"
#include "debug.h"
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
    size = (int) file.frames() * file.channels();
    sampleRate = file.samplerate();
    channels = file.channels();

    // Read samples
    samples = (float *) malloc(sizeof(float) * size);
    sf_count_t readSamples = file.read(samples, size);

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

// Return a segment of audio for analysis
float *AudioBuffer::getSegment(int i, int *size) {
    if (i < numSegments) {
        *size = samplesPerSegment;
        return samples + (samplesPerSegment * i);
    } else {
        *size = 0;
        return nullptr;
    }
}

// Return the entire array of audio samples
float *AudioBuffer::getSamples(int *size) {
    if (samples != nullptr) {
        *size = AudioBuffer::size;
        return samples;
    } else {
        *size = 0;
        return nullptr;
    }
}

int AudioBuffer::getSampleRate() {
    return sampleRate;
}

int AudioBuffer::getNumSegments() {
    return numSegments;
}

int AudioBuffer::getSamplesPerSegment() {
    return samplesPerSegment;
}

int AudioBuffer::frames() {
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

    if (segmentedSize != size) {
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
