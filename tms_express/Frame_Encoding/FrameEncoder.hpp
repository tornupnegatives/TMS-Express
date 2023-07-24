// Copyright (C) 2023 Joseph Bellahcen <joeclb@icloud.com>

#ifndef TMS_EXPRESS_FRAME_ENCODING_FRAMEENCODER_HPP_
#define TMS_EXPRESS_FRAME_ENCODING_FRAMEENCODER_HPP_

#include <string>
#include <vector>

#include "Frame_Encoding/Frame.hpp"

namespace tms_express {

/// @brief Generates bitstreams which adhere to TMS5220 LPC-10 specification
/// @details The Frame Encoder represents Frames as reversed hex bytes,
///             mimicking the behavior of the TMS6100 Voice Synthesis Memory
class FrameEncoder {
 public:
    ///////////////////////////////////////////////////////////////////////////
    // Initializers ///////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Creates a new Frame Encoder with empty Frame buffer
    /// @param include_hex_prefix true to include '0x' in front of hex bytes,
    ///                             false otherwise
    explicit FrameEncoder(bool include_hex_prefix = false);

    /// @brief Creates a new Frame Encoder, starting with given Frame buffer
    /// @param frames Vector of Frames to encode
    /// @param include_hex_prefix true to include '0x' in front of hex bytes,
    ///                             false otherwise
    explicit FrameEncoder(const std::vector<Frame> &frames,
        bool include_hex_prefix = false);

    ///////////////////////////////////////////////////////////////////////////
    // Frame Appenders ////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Appends single Frame to end of Frame buffer
    /// @param frame Frame to append
    void append(Frame frame);

    /// @brief Appends multiple Frames to end of Frame buffer
    /// @param frames Vector of frames to append
    void append(const std::vector<Frame> &frames);

    ///////////////////////////////////////////////////////////////////////////
    // (De-)Serialization /////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Imports ASCII bitstream from disk
    /// @param path Path to ASCII bitstream file
    /// @return Number of Frames imported from file
    size_t importASCIIFromFile(const std::string &path);

    /// @brief Imports ASCII bitstream from string
    /// @param flat_bitstream String of comma-delimited ASCII hex bytes
    /// @return Number of Frames imported from string
    size_t importASCIIFromString(std::string flat_bitstream);

    /// @brief Serializes Frames to ASCII bitstream
    /// @param append_stop_frame true to append explicit stop frame to end of
    ///                             bitstream, false otherwise
    /// @return ASCII bitstream, as string
    /// @note Appending a stop frame tells the TMS5220 to exit Speak External
    ///         mode. It is not strictly required for emulations of the device,
    ///         nor for bitstreams intended to be stored on a TMS6100 Voice
    ///         Synthesis Memory chip
    std::string toHex(bool append_stop_frame = true);

    /// @brief Serializes Frames to vector of raw bytes
    /// @param append_stop_frame true to append explicit stop frame to end of
    ///                             bitstream, false otherwise
    /// @return Vector of bytes corresponding to bitstream
    std::vector<std::byte> toBytes(bool append_stop_frame = true);

    /// @brief Converts Frames buffer to JSON array of Frame JSON objects
    /// @return JSON object array, as a string
    std::string toJSON() const;

    ///////////////////////////////////////////////////////////////////////////
    // Accessors //////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    std::vector<Frame> getFrameTable() const;

 private:
    ///////////////////////////////////////////////////////////////////////////
    // Static Helpers /////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Converts binary string to ASCII hex byte
    /// @param bin_str Binary string corresponding to byte
    /// @param include_hex_prefix true to include '0x' prefix, false otherwise
    /// @return ASCII hex representation of binary string
    static std::string binToHex(const std::string &bin_str,
        bool include_hex_prefix);

    /// @brief Reverses hex bytes in bitstream, effectively converting between
    ///         host representation and TMS6100 Voice Synthesis Memory format
    /// @param bitstream Bitstream string
    /// @return Bitstream with reversed hex bytes
    static std::string reverseHexBytes(std::string bitstream);

    /// @brief Extracts unvoiced LPC reflector coefficients (K1-K4) from binary
    ///         representation of Frame
    /// @param chunk Chunk holding binary representation of single Frame
    /// @param k1 Pointer to store K1 LPC reflector coefficient
    /// @param k2 Pointer to store K2 LPC reflector coefficient
    /// @param k3 Pointer to store K3 LPC reflector coefficient
    /// @param k4 Pointer to store K4 LPC reflector coefficient
    static void extractUnvoicedCoeffs(const std::string &chunk, float *k1,
        float *k2, float *k3, float *k4);

    /// @brief Extracts voiced LPC reflector coefficients (K5-K10) from binary
    ///         representation of Frame
    /// @param chunk Chunk holding binary representation of single Frame
    /// @param k5 Pointer to store K5 LPC reflector coefficient
    /// @param k6 Pointer to store K6 LPC reflector coefficient
    /// @param k7 Pointer to store K7 LPC reflector coefficient
    /// @param k8 Pointer to store K8 LPC reflector coefficient
    static void extractVoicedCoeffs(const std::string &chunk, float *k5,
        float *k6, float *k7, float *k8, float *k9, float *k10);

    ///////////////////////////////////////////////////////////////////////////
    // Helpers ////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    void appendStopFrame();

    ///////////////////////////////////////////////////////////////////////////
    // Members ////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    std::vector<std::string> binary_bitstream_;
    static const char byte_delimiter = ',';
    std::vector<Frame> frames_;
    bool include_hex_prefix_;
};

};  // namespace tms_express

#endif  // TMS_EXPRESS_FRAME_ENCODING_FRAMEENCODER_HPP_
