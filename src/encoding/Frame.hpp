// Copyright (C) 2023 Joseph Bellahcen <joeclb@icloud.com>

#ifndef TMS_EXPRESS_FRAME_ENCODING_FRAME_HPP_
#define TMS_EXPRESS_FRAME_ENCODING_FRAME_HPP_

#include <string>
#include <vector>

#include <nlohmann/json.hpp>

namespace tms_express {

/// @brief Characterizes speech data
/// @details One Frame holds information about a window of speech data,
///             typically corresponding to 22.5-30 ms of audio
class Frame {
 public:
    ///////////////////////////////////////////////////////////////////////////
    // Initializers ///////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Stores a new Frame
    /// @param pitch_period Pitch period, in samples
    /// @param is_voiced true if Frame represents voiced (vowel) sample,
    ///                     false for unvoiced (consonant)
    /// @param gain_db Gain, in decibels
    /// @param coeffs LPC reflector coefficients
    Frame(int pitch_period, bool is_voiced, float gain_db,
        std::vector<float> coeffs);

    ///////////////////////////////////////////////////////////////////////////
    // Accessors //////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Accesses LPC reflector coefficients
    /// @return Vector of coefficients, with length proportional to LPC
    ///         model order. Although voiced and unvoiced Frames differ in the
    ///         number of coefficients encoded into a bitstream, the coeffs
    ///         vector will always contain the full set
    std::vector<float> getCoeffs() const;

    /// @brief Replaces LPC reflector coefficients
    /// @param coeffs Vector of coefficients
    // TODO(Joseph Bellahcen): Enforce model order
    void setCoeffs(std::vector<float> coeffs);

    /// @brief Accesses the gain
    /// @return Gain, in decibels
    float getGain() const;

    /// @brief Sets the gain precisely
    /// @param gain_db New gain, in decibels
    void setGain(float gain_db);

    /// @brief Sets the gain coarsely via TMS5220 Coding Table
    /// @param idx Index into TMS5220 Coding Table's RMS vector
    /// @note If index out of bounds, it will be floored or ceiling to the
    ///         appropriate bound, resulting in a Frame with either minimum
    ///         or maximum gain
    void setGain(int idx);

    /// @brief Accesses the pitch period
    /// @return Pitch period, in samples
    int getPitch() const;

    /// @brief Sets the pitch period
    /// @param pitch New pitch period, in samples
    void setPitch(int pitch);

    /// @brief Checks if Frame is repeat of adjacent Frame
    /// @return true if Frame marked as repeat, false otherwise
    /// @note This property is determined by the Frame Post-Processor
    [[deprecated]] bool getRepeat() const;

    /// @brief Mark frame as repeat
    /// @param is_repeat true if Frame is repeat, false otherwise
    void setRepeat(bool is_repeat);

    /// @brief Checks voicing of Frame
    /// @return true if Frame is voiced (vowel) or unvoiced (consonant)
    [[deprecated]] bool getVoicing() const;

    /// @brief Sets voicing of Frame
    /// @param isVoiced true if Frame is voiced (vowel) or unvoiced (consonant)
    void setVoicing(bool isVoiced);

    ///////////////////////////////////////////////////////////////////////////
    // Quantized Getters //////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Quantizes LPC reflector coefficients via TMS5220 Coding Table
    /// @return Vector of indicies of closest LPC reflector coefficient value
    ///         in TMS5220 Coding Table
    std::vector<int> quantizedCoeffs() const;

    /// @brief Quantizes gain via TMS5220 Coding Table
    /// @return Index of closest gain value in TMS5220 Coding Table
    int quantizedGain() const;

    /// @brief Quantizes pitch via TMS5220 Coding Table
    /// @return Index of closest pitch period value in TMS5220 Coding Table
    int quantizedPitch() const;

    /// @brief Quantizes LPC voicing via TMS5220 Coding Table
    /// @return Index of closest gain value in TMS5220 Coding Table
    [[deprecated]] int quantizedVoicing() const;

    ///////////////////////////////////////////////////////////////////////////
    // Metadata ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Reports whether Frame is repeat (identical to neighbor)
    /// @return true if Frame is repeat, false otherwise
    bool isRepeat() const;

    /// @brief Reports whether Frame contains audio
    /// @return true if Frame is silent, false otherwise
    bool isSilent() const;

    /// @brief Reports whether frame is voiced (vowel) or unvoiced (consonant)
    /// @return true if Frame is voiced, false if unvoiced
    bool isVoiced() const;

    ///////////////////////////////////////////////////////////////////////////
    // Serializers ////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Converts Frame to binary representation, per the TMS5220's LPC
    ///         encoding specification
    /// @return Encoded Frame, as binary string
    std::string toBinary();

    /// @brief Converts Frame to JSON object which closely mirrors internal
    ///         implementation
    /// @return Encoded Frame, as JSON object
    nlohmann::json toJSON();

 private:
    ///////////////////////////////////////////////////////////////////////////
    // Static Helpers /////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Finds value in TMS5220 Coding Table whose value is closest to
    ///         argument, and provides its index
    /// @param value Value to look for
    /// @param table TMS5220 Coding Table entry
    /// @return Index of closest value to argument in TMS5220 Coding Table
    static int closestIndex(float value, std::vector<float> table);

    /// @brief Converts integer value to binary string
    /// @param value Value to convert
    /// @param width Number of bits to use for encoding
    /// @return Binary string corresponding to value, MSB first
    static std::string valueToBinary(int value, int width);

    ///////////////////////////////////////////////////////////////////////////
    // Members ////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Gain, in decibels
    float gain_db_;

    /// @brief Pitch period, in samples
    int pitch_period_;

    /// @brief LPC reflector coefficients
    std::vector<float> coeffs_;

    /// @brief true if Frame is repeat (identical to neighbor), false otherwise
    bool is_repeat_;

    /// @brief true if Frame is voiced (vowel), false if unvoiced (consonant)
    bool is_voiced_;
};

};  // namespace tms_express

#endif  // TMS_EXPRESS_FRAME_ENCODING_FRAME_HPP_
