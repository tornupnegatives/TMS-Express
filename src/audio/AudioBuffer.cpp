// Copyright (C) 2023 Joseph Bellahcen <joeclb@icloud.com>

#include "audio/AudioBuffer.hpp"

#include <memory>
#include <utility>
#include <vector>

#include <sndfile.hh>
#include <samplerate.h>

namespace tms_express {

///////////////////////////////////////////////////////////////////////////////
// Factory Functions //////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

std::shared_ptr<AudioBuffer> AudioBuffer::Create(const std::string &path,
    int sample_rate_hz, float window_width_ms) {
    //
    // Attempt to open an audio file via libsndfile, aborting initialization if
    // the given path does not exist, is invalid, or is not a suported format
    auto audio_file = SndfileHandle(path);

    if (audio_file.error()) {
        return nullptr;
    }

    // Import samples from the audio file, then resample and mix
    auto src_sample_rate_hz = audio_file.samplerate();
    auto n_frames = audio_file.frames();
    auto n_channels = audio_file.channels();
    auto buffer_size = n_frames * n_channels;

    std::vector<float> samples(buffer_size);
    audio_file.read(samples.data(), buffer_size);

    // Resample if needed, and mix to mono
    if (n_channels != 1) {
        samples = mixToMono(samples, n_channels);
    }

    if (src_sample_rate_hz != sample_rate_hz) {
        samples = resample(samples, src_sample_rate_hz, sample_rate_hz);
    }

    auto ptr = std::make_shared<AudioBuffer>(
                AudioBuffer(samples, sample_rate_hz, window_width_ms));

    return ptr;
}

///////////////////////////////////////////////////////////////////////////////
// Initializers ///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

AudioBuffer::AudioBuffer(std::vector<float> samples, int sample_rate_hz,
    float window_width_ms) {
    //
    n_segments_ = 0;
    n_samples_per_segment_ = 0;
    sample_rate_hz_ = sample_rate_hz;

    samples_ = samples;
    original_samples_ = samples_;

    setWindowWidthMs(window_width_ms);
}

AudioBuffer::AudioBuffer(int sample_rate_hz, float window_width_ms) {
    n_segments_ = 0;
    n_samples_per_segment_ = 0;
    sample_rate_hz_ = sample_rate_hz;

    samples_ = original_samples_ = {};

    setWindowWidthMs(window_width_ms);
}

///////////////////////////////////////////////////////////////////////////////
// Accessors //////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

std::vector<float> AudioBuffer::getSamples() const {
    return samples_;
}

void AudioBuffer::setSamples(const std::vector<float> &samples) {
    // If, for some reason, the passed vector is empty, simply clear the buffer
    if (samples.empty()) {
        n_segments_ = 0;
        n_samples_per_segment_ = 0;
        samples_.clear();

        return;
    }

    samples_ = samples;
    setWindowWidthMs(getWindowWidthMs());
}

float AudioBuffer::getWindowWidthMs() const {
    float numerator = static_cast<float>(n_samples_per_segment_);
    float denominator = static_cast<float>(sample_rate_hz_) * 1.0e-3;

    return numerator / denominator;
}

void AudioBuffer::setWindowWidthMs(float window_width_ms) {
    if (window_width_ms == 0) {
        n_samples_per_segment_ = 1;
        n_segments_ = samples_.size();
        return;
    }

    // Re-compute segment bounds
    n_samples_per_segment_ = static_cast<int>(
        static_cast<float>(sample_rate_hz_) * window_width_ms * 1e-3);
    n_segments_ = samples_.size() / n_samples_per_segment_;

    // Pad final segment with zeros
    std::vector<float>::size_type padded_size =
        n_samples_per_segment_ * n_segments_;

    if (samples_.size() < padded_size) {
        samples_.resize(padded_size, 0);

    } else if (samples_.size() > padded_size) {
        samples_.resize(padded_size + n_samples_per_segment_, 0);
    }
}

int AudioBuffer::getSampleRateHz() const {
    return sample_rate_hz_;
}

std::vector<float> AudioBuffer::getSegment(int i) const {
    if (i >= n_segments_ || empty()) {
        return {};
    }

    auto offset = n_samples_per_segment_;
    auto start = samples_.begin() + (i * offset);
    auto end = start + offset;

    return {start, end};
}

std::vector<std::vector<float>> AudioBuffer::getAllSegments() const {
    if (empty()) {
        return {};
    }

    auto segments = std::vector<std::vector<float>>();
    for (int i = 0; i < n_segments_; i++) {
        segments.push_back(getSegment(i));
    }

    return segments;
}

int AudioBuffer::getNSamplesPerSegment() const {
    return n_samples_per_segment_;
}

int AudioBuffer::getNSegments() const {
    return n_segments_;
}

///////////////////////////////////////////////////////////////////////////////
// Metadata ///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool AudioBuffer::empty() const {
    return samples_.empty();
}

///////////////////////////////////////////////////////////////////////////
// Utility ////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

AudioBuffer AudioBuffer::copy() const {
    return AudioBuffer(samples_, sample_rate_hz_, getWindowWidthMs());
}

bool AudioBuffer::render(const std::string &path) const {
    // Indicate error if buffer is empty
    if (empty()) {
        return false;
    }

    // Construct WAV audio file handle
    auto metadata = SF_INFO();
    metadata.channels = 1;
    metadata.frames = sf_count_t(samples_.size());
    metadata.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
    metadata.samplerate = sample_rate_hz_;

    // Open file
    // TODO(Joseph Bellahcen) Track number of samples written and return error
    // if number does not match original buffer size
    auto audio_file = sf_open(path.c_str(), SFM_WRITE, &metadata);
    sf_writef_float(audio_file, samples_.data(), sf_count_t(samples_.size()));
    sf_close(audio_file);

    return true;
}

void AudioBuffer::reset() {
    samples_ = original_samples_;
}

///////////////////////////////////////////////////////////////////////////////
// Static Initialization Utilities ////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

std::vector<float> AudioBuffer::mixToMono(std::vector<float> samples,
    int n_channels) {
    //
    int mono_size = samples.size() / n_channels;
    auto mono_samples = std::vector<float>(mono_size, 0);

    for (int frame = 0; frame < mono_size; frame++) {
        for (int channel = 0; channel < n_channels; channel++) {
            mono_samples[frame] += samples[frame * n_channels + channel];
        }

        mono_samples[frame] /= static_cast<float>(n_channels);
    }

    return mono_samples;
}

// Resample the audio buffer to the target sample rate
std::vector<float> AudioBuffer::resample(std::vector<float> samples,
    int src_sample_rate_hz, int target_sample_rate_hz) {
    //
    // Resampler parameters
    // NOTE:    If a future version of this codebase requires
    //          compatibility with stereo audio, compute the
    //          number of frames as: size / (channels * ratio)
    //          and the number of samples as: (frames * channels)
    double ratio = static_cast<double>(target_sample_rate_hz) /
        static_cast<double>(src_sample_rate_hz);

    auto n_frames = static_cast<int>(
        static_cast<double>(samples.size()) * ratio);

    auto resampled_buffer = std::vector<float>(n_frames);

    // Initialize resampler
    auto resampler = SRC_DATA();
    resampler.data_in = samples.data();
    resampler.input_frames = samples.size();
    resampler.data_out = resampled_buffer.data();
    resampler.output_frames = n_frames;
    resampler.src_ratio = ratio;

    // Store resampled audio
    // TODO(Joseph Bellahcen): Check for errors and return empty vector if occur
    src_simple(&resampler, SRC_SINC_BEST_QUALITY, 1);
    return resampled_buffer;
}

};  // namespace tms_express
