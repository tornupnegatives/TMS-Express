///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class: AudioBuffer
//
// Description: The AudioBuffer stores mono audio samples and provides and interface for segmentation. Audio data
//              may be imported from any format supported by libsndfile and will be resampled during initialization
//
// Author: Joseph Bellahcen <joeclb@icloud.com>
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Audio/AudioBuffer.h"

#include <samplerate.h>
#include <sndfile.hh>

#include <stdexcept>
#include <utility>
#include <vector>

/// Create an audio buffer from an existing audio file
///
/// \param path Path to audio file
/// \param targetSampleRateHz Rate at which to sample audio (in Hertz)
/// \param windowWidthMs Segmentation window width (in milliseconds)
AudioBuffer::AudioBuffer(const std::string &path, int targetSampleRateHz, float windowWidthMs) {
    // Import audio file via libsndfile
    auto sndfile = SndfileHandle(path);

    // TODO: Catch in GUI
    if (sndfile.error()) {
        throw std::runtime_error("Unsupported audio format");
    }

    // Extract audio metadata
    sampleRateHz = sndfile.samplerate();
    auto nFrames = sndfile.frames();
    auto nChannels = sndfile.channels();

    // Store floating-point samples
    auto bufferSize = nFrames * nChannels;
    samples = std::vector<float>(bufferSize);
    sndfile.read(samples.data(), bufferSize);

    // Mix to mono
    if (nChannels != 1) {
        mixToMono(nChannels);
    }

    // Resample at target rate
    // TODO: Implement upsampling safeguards
    if (sndfile.samplerate() != targetSampleRateHz) {
        resample(targetSampleRateHz);
    }

    // Initialize parameters which depend on window width
    nSegments = 0;
    originalSamples = samples;
    samplesPerSegment = 0;

    setWindowWidth(windowWidthMs);
}

/// Create an audio buffer from raw samples
///
/// \param pcmSamples Floating point audio samples
/// \param sampleRate Sample rate of buffer (in Hertz)
/// \param windowWidthMs Segmentation window width (in milliseconds)
AudioBuffer::AudioBuffer(std::vector<float> pcmSamples, int sampleRate, float windowWidthMs) {
    sampleRateHz = sampleRate;
    samples = std::move(pcmSamples);

    nSegments = 0;
    originalSamples = samples;
    samplesPerSegment = 0;

    setWindowWidth(windowWidthMs);
}

/// Initialize an empty Audio Buffer
///
/// \param sampleRate Target sample rate of buffer (in Hertz)
/// \param windowWidthMs Segmentation window width (in milliseconds)
AudioBuffer::AudioBuffer(int sampleRate, float windowWidthMs) {
    nSegments = 0;
    originalSamples = samples = {};
    sampleRateHz = sampleRate;
    samplesPerSegment = 0;

    setWindowWidth(windowWidthMs);
}

/// Create a deep copy of an existing audio buffer
///
/// \param buffer Original AudioBuffer to copy
AudioBuffer::AudioBuffer(const AudioBuffer &buffer) {
    sampleRateHz = buffer.sampleRateHz;
    samplesPerSegment = buffer.samplesPerSegment;
    nSegments = buffer.nSegments;
    samples = std::vector<float>(buffer.samples);
}

///////////////////////////////////////////////////////////////////////////////
//                          Getters & Setters
///////////////////////////////////////////////////////////////////////////////

/// Return flat, unsegmented array of samples
std::vector<float> AudioBuffer::getSamples() {
    return samples;
}

/// Replace samples in buffer
///
/// \note Size of newSamples must equal size of buffer
///
/// \param newSamples Samples to be copied into buffer
void AudioBuffer::setSamples(const std::vector<float> &newSamples) {
    // If the Audio Buffer is empty, allow the user to set the samples with a
    // vector of any size. Otherwise, the new samples must be the same size as
    // the original samples
    if (!isEmpty() && (newSamples.size() != samples.size())) {
        throw std::length_error("Unsupported buffer write: new size must equal old size");
    }

    samples = newSamples;

    // If the Audio Buffer was empty, determine the segmentation bounds
    if (isEmpty()) {
        setWindowWidth(getWindowWidth());
    }
}

/// Return window width (in number of samples)
float AudioBuffer::getWindowWidth() const {
    return float(samplesPerSegment) / float(sampleRateHz * 1.0e-3);
}

/// Update the window width, modifying segment bounds
///
/// \param windowWidthMs Desired window width (in milliseconds)
void AudioBuffer::setWindowWidth(float windowWidthMs) {
    // Re-compute segment bounds
    samplesPerSegment = int(float(sampleRateHz) * windowWidthMs * 1e-3);
    nSegments = samples.size() / samplesPerSegment;

    // Effectively pad final segment with zeros
    unsigned int paddedSize = samplesPerSegment * nSegments;
    if (samples.size() < paddedSize) {
        samples.resize(paddedSize, 0);
    } else if (samples.size() > paddedSize) {
        samples.resize(paddedSize + samplesPerSegment, 0);
    }
}

///////////////////////////////////////////////////////////////////////////////
//                             Const Getters
///////////////////////////////////////////////////////////////////////////////

/// Return sampling rate of audio
int AudioBuffer::sampleRate() const {
    return sampleRateHz;
}

/// Return ith audio segment
std::vector<float> AudioBuffer::segment(int i) {
    if (i >= nSegments) {
        throw std::range_error("Segment index out of bounds");
    }

    if (isEmpty()) {
        throw std::runtime_error("Cannot segment empty buffer");
    }

    auto offset = int(samplesPerSegment);
    auto start = samples.begin() + (i * offset);
    auto end = start + offset;

    return {start, end};
}

/// Return 2-dimensional array of all segments
std::vector<std::vector<float>> AudioBuffer::segments() {
    auto _segments = std::vector<std::vector<float>>();
    for (int i = 0; i < nSegments; i++) {
        _segments.push_back(segment(i));
    }

    return _segments;
}

/// Return number of audio samples in each segment
size_t AudioBuffer::segmentSize() const {
    return samplesPerSegment;
}

/// Return number of total audio segments
size_t AudioBuffer::size() const {
    return nSegments;
}

/// Return whether or not the Audio Buffer contains samples
bool AudioBuffer::isEmpty() const {
    return samples.empty();
}

///////////////////////////////////////////////////////////////////////////////
//                       Public Utility Functions
///////////////////////////////////////////////////////////////////////////////

/// Export buffer contents to WAV audio file
///
/// \param path Path to new audio file
void AudioBuffer::render(const std::string &path) {
    // Throw error if buffer is empty
    if (isEmpty()) {
        throw std::runtime_error("Cannot render empty buffer");
    }
    
    // Construct 8kHz mono audio file handle
    auto metadata = SF_INFO();

    metadata.channels = 1;
    metadata.frames = sf_count_t(samples.size());
    metadata.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
    metadata.samplerate = int(sampleRateHz);

    // Open file
    auto sndfile = sf_open(path.c_str(), SFM_WRITE, &metadata);
    sf_writef_float(sndfile, samples.data(), sf_count_t(samples.size()));
    sf_close(sndfile);
}

/// Restore buffer to its initialization state
///
/// \note This function will NOT reset the window width
void AudioBuffer::reset() {
    samples = originalSamples;
}

// Mix audio buffer to mono by summing each frame across channels and averaging the result
void AudioBuffer::mixToMono(int nOriginalChannels) {
    auto monoSize = samples.size() / nOriginalChannels;
    auto monoSamples = std::vector<float>(monoSize, 0);

    for (int frame = 0; frame < monoSize; frame++) {
        for (int channel = 0; channel < nOriginalChannels; channel++) {
            monoSamples[frame] += samples[frame * nOriginalChannels + channel];
        }

        monoSamples[frame] /= float(nOriginalChannels);
    }

    samples = monoSamples;
}

// Resample the audio buffer to the target sample rate
void AudioBuffer::resample(int targetSampleRateHz) {
    // Resampler parameters
    // NOTE:    If a future version of this codebase requires
    //          compatibility with stereo audio, compute the
    //          number of frames as: size / (channels * ratio)
    //          and the number of samples as: (frames * channels)
    double ratio = double(targetSampleRateHz) / double(sampleRateHz);
    int nFrames = int(double(samples.size()) * ratio);
    auto resampledBuffer = std::vector<float>(nFrames);

    // Initialize resampler
    auto resampler = SRC_DATA();
    resampler.data_in = samples.data();
    resampler.input_frames = long(samples.size());
    resampler.data_out = resampledBuffer.data();
    resampler.output_frames = nFrames;
    resampler.src_ratio = ratio;

    // Store resampled audio
    src_simple(&resampler, SRC_SINC_BEST_QUALITY, 1);
    sampleRateHz = targetSampleRateHz;
    samples = resampledBuffer;
}
