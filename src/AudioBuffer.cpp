//
// Created by Joseph Bellahcen on 4/6/22.
//

#include "AudioBuffer.h"
#include "debug.h"
#include <samplerate.h>
#include <sndfile.hh>

// Initialize an AudioBuffer from a file at the given path, resamples it to the
// target sample rate, and segments it into n windows
AudioBuffer::AudioBuffer(const char *path, int targetSampleRate, float windowSize) {
    // Import audio file
    SndfileHandle file = SndfileHandle(path);
    DEBUG("File:\t\t\t%s\n", path);
    DEBUG("Sample rate:\t%d\n", file.samplerate());
    DEBUG("Channels:\t\t%d\n", file.channels());

    // Gather metadata
    size = (int) file.frames() * file.channels();
    sampleRate = file.samplerate();
    channels = file.channels();

    // Read samples
    samples = (float *) malloc(sizeof(float) * size);
    sf_count_t readSamples = file.read(samples, size);
    DEBUG("Read %lld samples\n", readSamples);

    // Mixdown & resample
    mixdown();
    DEBUG("Mixed to %d samples\n", size);
    resample(targetSampleRate);
    DEBUG("Resampled to %d Hz (%d samples)\n", sampleRate, size);

    // Calculate number of segments
    samplesPerSegment = (int) (windowSize * 1e-3 * sampleRate);
    numSegments = size / samplesPerSegment;
    DEBUG("Slicing into %d segments (%d samples per segment)\n", numSegments, samplesPerSegment);

    // Pad final segment
    padFinalSegment();
    DEBUG("After padding: %d segments (%d samples)\n", numSegments, size);
}

AudioBuffer::~AudioBuffer() {
    free(samples);
}

float *AudioBuffer::segment(int i, int *size) {
    if (i < numSegments) {
        *size = samplesPerSegment;
        return samples + (samplesPerSegment * i);
    } else {
        *size = 0;
        return nullptr;
    }
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