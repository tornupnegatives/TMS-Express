// Copyright (C) 2023 Joseph Bellahcen <joeclb@icloud.com>

#ifndef TMS_EXPRESS_FRAME_ENCODING_FRAMEPOSTPROCESSOR_HPP_
#define TMS_EXPRESS_FRAME_ENCODING_FRAMEPOSTPROCESSOR_HPP_

#include <vector>

#include "Frame_Encoding/Frame.hpp"

namespace tms_express {

/// @brief Applies post-processing to Frame table to improve synthesis quality
class FramePostprocessor {
 public:
    ///////////////////////////////////////////////////////////////////////////
    // Initializers ///////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Creates new Frame Postprocessor to operate on given Frame table
    /// @param frames Pointer to Frame table
    /// @param max_voiced_gain_db Max voiced (vowel) gain, in decibels
    /// @param max_unvoiced_gain_db Max unvoiced (consonant) gain, in decibels
    explicit FramePostprocessor(std::vector<Frame> *frames,
      float max_voiced_gain_db = 37.5, float max_unvoiced_gain_db = 37.5);

    ///////////////////////////////////////////////////////////////////////////
    // Accessors //////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Accesses the max unvoiced (consonant) gain
    /// @return Max unvoiced gain, in decibels
    float getMaxUnvoicedGainDB() const;

    /// @brief Sets the max unvoiced (consonant) gain
    /// @param gain_db Max unvoiced gain, in decibels
    void setMaxUnvoicedGainDB(float gain_db);

    /// @brief Accesses the max voiced (vowel) gain
    /// @return Max voiced gain, in decibels
    float getMaxVoicedGainDB() const;

    /// @brief Sets the max voiced (vowel) gain
    /// @param gain_db Max voiced gain, in decibels
    void setMaxVoicedGainDB(float gain_db);

    ///////////////////////////////////////////////////////////////////////////
    // Frame Table Manipulators ///////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Marks Frames which are similar to their neighbors as repeat
    /// @return Number of repeat Frames detected
    /// @details Because the LPC model of the human vocal tract asserts that
    ///             speech signals vary relatively slowly, it may be observed
    ///             that consecutive Frames are often similar. Such Frames may
    ///             be marked as repeats, such that only the original is fully
    ///             encoded. This effectively compresses the bitstream
    int detectRepeatFrames();

    /// @brief Applies gain normalization to all Frames
    /// @note Gain normalization reduces DC offset and creates natural volume
    void normalizeGain();

    /// @brief Shifts gain by integer offset into TMS5220 Coding Table
    /// @param offset Offset into TMS5200 Coding Table entry
    /// @note Offset is subject to floor/ceiling to prevent unstable bitstreams
    void shiftGain(int offset);

    /// @brief Shifts pitch by integer offset into TMS5220 Coding Table
    /// @param offset Offset into TMS5200 Coding Table entry
    /// @note Offset is subject to floor/ceiling to prevent unstable bitstreams
    void shiftPitch(int offset);

    /// @brief Sets the pitch of all Frames to value from TMS5220 Coding Table
    /// @param index Index into TMS5220 Coding Table entry
    void overridePitch(int index);

    ///////////////////////////////////////////////////////////////////////////
    // Utility ////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Restores Frame table to initialization state
    /// @note Does not reset voiced or unvoiced gain limits
    [[deprecated]] void reset();

 private:
    ///////////////////////////////////////////////////////////////////////////
    // Helpers ////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Applies normalization to either voiced or unvoiced Frames
    /// @param target_voiced true to normalized voiced Frames, false
    ///                         for unvoiced Frames
    void normalizeGain(bool target_voiced);

    ///////////////////////////////////////////////////////////////////////////
    // Members ////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    std::vector<Frame> original_frame_table_;
    std::vector<Frame> *frame_table_;
    float max_unvoiced_gain_db_;
    float max_voiced_gain_db_;

};

};  // namespace tms_express

#endif  // TMS_EXPRESS_FRAME_ENCODING_FRAMEPOSTPROCESSOR_HPP_
