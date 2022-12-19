///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class: AudioBuffer
//
// Description: The AudioBuffer stores mono audio samples and provides and interface for segmentation. Audio data
//              may be imported from any format supported by libsndfile and may be resampled
//
// Author: Joseph Bellahcen <joeclb@icloud.com>
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Audio/AudioBuffer.h"
#include <samplerate.h>
#include <sndfile.hh>
#include <stdexcept>
#include <vector>

// Extract 8kHz mono audio samples from the provided file
AudioBuffer::AudioBuffer(const std::string &path, float segmentDurationMs) {
    // Import audio file and get metadata
    auto sndfile = SndfileHandle(path);

    if (sndfile.error()) {
        throw std::runtime_error("Unsupported audio format");
    }

    sampleRateHz = sndfile.samplerate();
    sf_count_t nFrames = sndfile.frames();
    int nChannels = sndfile.channels();

    // Storing samples as integers improves the accuracy of speech-analysis algorithms
    // The values are normalized by the smallest value for 16-bit integers
    sf_count_t bufferSize = nFrames * nChannels;
    samples = std::vector<float>(bufferSize);
    for (int i = 0; i < bufferSize; i++) {
        int sample = 0;
        sndfile.read(&sample, 1);

        samples[i] = float(sample / normalizationFactor);
    }

    // Mixdown and resample
    if (nChannels > 1) {
        mixToMono(nChannels);
    }

    segmentDuration = segmentDurationMs;
    pad();
}

AudioBuffer::AudioBuffer(const AudioBuffer &buffer) {
    sampleRateHz = buffer.sampleRate();
    segmentDuration = buffer.segmentDuration;
    samples = std::vector<float>(buffer.samples);
}

///////////////////////////////////////////////////////////////////////////////
//                          Getters & Setters
///////////////////////////////////////////////////////////////////////////////

unsigned int AudioBuffer::sampleRate() const {
    return sampleRateHz;
}

unsigned int AudioBuffer::nSegments() const {
    return samples.size() / samplesPerSegment();
}

unsigned int AudioBuffer::samplesPerSegment() const {
    return int(float(sampleRate()) * segmentDuration * 1e-3);
}

std::vector<float> AudioBuffer::getSamples() {
    return samples;
}

void AudioBuffer::setSamples(const std::vector<float> &newSamples) {
    if (newSamples.size() != samples.size()) {
        throw std::length_error("Unsupported buffer write: new size must equal old size");
    }

    samples = newSamples;
}

std::vector<float> AudioBuffer::getSegment(int i) {
    if (i >= nSegments()) {
        throw std::range_error("Segment index out of bounds");
    }

    auto start = samples.begin() + (i * samplesPerSegment());
    auto end = start + samplesPerSegment();
    auto segment = std::vector<float>(start, end);
    return segment;
}

__attribute__((unused)) std::vector<std::vector<float>> AudioBuffer::getSegments() {
    auto segments = std::vector<std::vector<float>>();
    for (int i = 0; i < nSegments(); i++) {
        auto segment = getSegment(i);
        segments.push_back(segment);
    }

    return segments;
}

///////////////////////////////////////////////////////////////////////////////
//                          Utility Functions
///////////////////////////////////////////////////////////////////////////////

// Export 8kHz mono audio from the buffer to a file
__attribute__((unused)) void AudioBuffer::exportAudio(const std::string &path) {
    // Construct 8kHz mono audio file handle
    auto metadata = SF_INFO();

    metadata.channels = 1;
    metadata.frames = sf_count_t(samples.size());
    metadata.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
    metadata.samplerate = int(sampleRate());

    // Convert samples back to un-normalized integers
    auto  intSamples = std::vector<int>(metadata.frames);
    for (int i = 0; i < metadata.frames; i++)
        intSamples[i] = int(samples[i] * normalizationFactor);

    // Open file
    auto sndfile = sf_open(path.c_str(), SFM_WRITE, &metadata);
    sf_writef_int(sndfile, intSamples.data(), sf_count_t(samples.size()));
    sf_close(sndfile);

}

// Mix audio buffer to mono by summing each frame across channels and averaging the result
void AudioBuffer::mixToMono(int nChannels) {
    auto monoSize = samples.size() / nChannels;
    auto monoSamples = std::vector<float>(monoSize, 0);

    for (int frame = 0; frame < monoSize; frame++) {
        for (int channel = 0; channel < nChannels; channel++) {
            monoSamples[frame] += samples[frame * nChannels + channel];
        }

        monoSamples[frame] /= float(nChannels);
    }

    samples = monoSamples;
}

// Resample the audio buffer to the target sample rate
void AudioBuffer::resample(int targetSampleRateHz) {
    // Resampler parameters
    double ratio = double(targetSampleRateHz) / double(sampleRate());
    //int newFrames = (int) ((double) size / (double) channels * ratio);
    unsigned int nFrames = int(double(samples.size()) * ratio);
    // auto newSamples = (float *) malloc(sizeof(float) * frames() * channels);
    auto resampledBuffer = std::vector<float>(nFrames);

    // Initialize resampler
    auto resampler = SRC_DATA();
    resampler.data_in = samples.data();
    // resampler->input_frames = frames();
    resampler.input_frames = long(samples.size());
    resampler.data_out = resampledBuffer.data();
    resampler.output_frames = nFrames;
    resampler.src_ratio = ratio;

    // Resample and store result
    src_simple(&resampler, SRC_SINC_BEST_QUALITY, 1);
    sampleRateHz = targetSampleRateHz;
    samples = resampledBuffer;
}

void AudioBuffer::pad() {
    // Pad with zeros
    unsigned int paddedSize = samplesPerSegment() * nSegments();
    if (samples.size() < paddedSize) {
        samples.resize(paddedSize, 0);
    } else if (samples.size() > paddedSize) {
        samples.resize(paddedSize + samplesPerSegment(), 0);
    }
}
