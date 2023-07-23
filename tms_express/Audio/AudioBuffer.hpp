// Copyright (C) 2023 Joseph Bellahcen <joeclb@icloud.com>

#ifndef TMS_EXPRESS_AUDIO_AUDIOBUFFER_HPP_
#define TMS_EXPRESS_AUDIO_AUDIOBUFFER_HPP_

#include <memory>
#include <string>
#include <vector>

namespace tms_express {

/// @brief Stores mono audio samples and provides interface for
///         segment-based analysis
class AudioBuffer {
 public:
    ///////////////////////////////////////////////////////////////////////////
    // Factory Functions //////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Creates new Audio Buffer from audio file
    /// @param path Path to audio file
    /// @param sample_rate_hz Rate at which to sample/resample audio, in Hertz
    /// @param window_width_ms Segmentation window with, in milliseconds
    /// @return Pointer to a valid Audio Buffer if path points to valid file,
    ///         nullptr otherwise
    static std::shared_ptr<AudioBuffer> Create(const std::string &path,
        int sample_rate_hz = 8000, float window_width_ms = 25.0f);

    ///////////////////////////////////////////////////////////////////////////
    // Initializers ///////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Initializes new Audio Buffer from given samples
    /// @param samples Floating-point PCM samples
    /// @param sample_rate_hz Sampling rate used to generate samples, in Hertz
    /// @param window_width_ms Segmentation window with, in milliseconds
    explicit AudioBuffer(std::vector<float> samples, int sample_rate_hz,
        float window_width_ms);

    /// @brief Initializes a new empty Audio Buffer
    /// @param sample_rate_hz Sample rate, in Hertz, of data which Audio Buffer
    ///                         may eventually hold
    /// @param window_width_ms Segmentation window with, in milliseconds
    explicit AudioBuffer(int sample_rate_hz = 8000,
        float window_width_ms = 25.0f);

    ///////////////////////////////////////////////////////////////////////////
    // Accessors //////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Accesses unsegmented array of samples
    /// @return Samples vector
    std::vector<float> getSamples() const;

    /// @brief Replaces Audio Buffer samples with given vector
    /// @param samples New samples vector
    void setSamples(const std::vector<float> &samples);

    /// @brief Accesses segmentation window width
    /// @return Segmentation widnow width, in milliseconds
    float getWindowWidthMs() const;

    /// @brief Recomputes analysis segment bounds from given window width
    /// @param window_width_ms New segmentation window width, in milliseconds
    void setWindowWidthMs(float window_width_ms);

    /// @brief Accesses audio sampling rate
    /// @return Sampling rate, in Hertz
    int getSampleRateHz() const;

    /// @brief Accesses ith segment of Audio Buffer
    /// @param i Index of the desired segment
    /// @return The ith segment if index in range, empty vector if index out of
    ///         range or Audio Buffer empty
    std::vector<float> getSegment(int i) const;

    /// @brief Accesses all segments, as a 2D vector
    /// @return Vector of segments, or empty vector if Audio Buffer empty
    std::vector<std::vector<float>> getAllSegments() const;

    /// @brief Accesses number of samples each segment
    /// @return Samples per segment
    int getNSamplesPerSegment() const;

    /// @brief Accesses number of segments in Audio Buffer
    /// @return Segments in Audio Buffer
    int getNSegments() const;

    ///////////////////////////////////////////////////////////////////////////
    // Metadata ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Reports whether Audio Buffer contains samples
    /// @return false if Audio Buffer contains no samples, true otherwise
    bool empty() const;

    ///////////////////////////////////////////////////////////////////////////
    // Utility ////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief  Creates an independent copy of the Audio Buffer
    /// @return New Audio Buffer which is separate from but identical to its
    ///         parent at the time of creation
    AudioBuffer copy() const;

    /// @brief Exports Audio Buffer to audio file for playback
    /// @param path Path to new audio file
    /// @return true if render successful, false otherwise
    bool render(const std::string &path) const;

    /// @brief Restore Audio Buffer samples to initialization state
    [[deprecated]] void reset();

 private:
    ///////////////////////////////////////////////////////////////////////////
    // Static Initialization Utilities ////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Mixes multi-channel audio to 1D mono samples
    /// @param samples Original multi-channel samples
    /// @param n_channels Original number of channels
    /// @return Mono samples, as a 1D vector
    static std::vector<float> mixToMono(std::vector<float> samples,
        int n_channels);

    /// @brief Resamples samples to the target sample rate
    /// @param samples Original samples
    /// @param src_sample_rate_hz Original sample rate, in Hertz
    /// @param target_sample_rate_hz Target sample rate, in Hertz
    /// @return Resampled vectors at the target sample rate
    static std::vector<float> resample(std::vector<float> samples,
        int src_sample_rate_hz, int target_sample_rate_hz);

    ///////////////////////////////////////////////////////////////////////////
    // Members ////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Sampling rate used to generate or import samples, in Hertz
    int sample_rate_hz_;

    /// @brief Number of segments in Audio Buffer, determined by segmentation
    ///         window width
    int n_segments_;

    /// @brief Number of samples in each segment, determined by segmentation
    ///         window width
    int n_samples_per_segment_;

    /// @brief Flat (unsegmented) buffer of samples
    std::vector<float> samples_;

    /// @brief Original samples set during initialization
    // TODO(Joseph Bellahcen): Remove
    std::vector<float> original_samples_;
};

};  //  namespace tms_express

#endif  //  TMS_EXPRESS_AUDIO_AUDIOBUFFER_HPP_
