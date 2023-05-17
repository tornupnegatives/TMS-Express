///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class: FrameEncoder
//
// Description: The FrameEncoder generates a bitstream representing a Frame vector. This bitstream adheres to the LPC-10
//              specification, and data is segmented into reversed hex bytes to mimic the behavior of the TMS6100 Voice
//              Synthesis Memory device.
//
// Author: Joseph Bellahcen <joeclb@icloud.com>
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Frame_Encoding/FrameEncoder.h"
#include "Frame_Encoding/Frame.h"

#include "json.hpp"

#include <algorithm>
#include <cstdio>

/// Create a new Frame Encoder with an empty frame buffer
///
/// \param includeHexPrefix Whether or not to include '0x' before hex bytes
/// \param separator Character with which to separate hex bytes
FrameEncoder::FrameEncoder(bool includeHexPrefix, char separator) {
    shouldIncludeHexPrefix = includeHexPrefix;
    byteSeparator = separator;
    frames = std::vector<Frame>();
    bytes = std::vector<std::string>(1, "");
}

/// Create a new Frame Encoder and populate it with frames
///
/// \param initialFrames Frames with which to populate encoder
/// \param includeHexPrefix Whether or not to include '0x' before hex bytes
/// \param separator Character with which to separate hex bytes
FrameEncoder::FrameEncoder(const std::vector<Frame> &initialFrames, bool includeHexPrefix, char separator) {
    bytes = std::vector<std::string>(1, "");
    byteSeparator = separator;
    frames = std::vector<Frame>();
    shouldIncludeHexPrefix = includeHexPrefix;

    append(initialFrames);
}

/// Append binary representation of a frame to the end of the encoder buffer
///
/// \note   The binary representation of a Frame is seldom cleanly divisible into bytes. As such, the first few bits of
///         a Frame may be packed into the empty space of an existing vector element, or the last few bits may partially
///         occupy a new vector element
void FrameEncoder::append(Frame frame) {
    frames.push_back(frame);
    auto bin = frame.toBinary();

    // Check to see if the previous byte is incomplete (contains less than 8 characters), and fill it if so
    auto emptyBitsInLastByte = 8 - bytes.back().size();
    if (emptyBitsInLastByte != 0) {
        bytes.back() += bin.substr(0, emptyBitsInLastByte);
        bin.erase(0, emptyBitsInLastByte);
    }

    // Segment the rest of the binary frame into bytes. The final byte will likely be incomplete, but that will be
    // addressed either in a subsequent call to append() or during hex stream generation
    while (!bin.empty()) {
        auto byte = bin.substr(0, 8);
        bytes.push_back(byte);

        bin.erase(0, 8);
    }
}

/// Append binary representation of new frames to the end of the encoder buffer
///
/// \param newFrames Frames to be appended
void FrameEncoder::append(const std::vector<Frame> &newFrames) {
    for (const auto &frame: newFrames) {
        append(frame);
    }
}

/// Serialize the Frame data to a stream of hex bytes
///
/// \note   Appending a stop frame tells the TMS5220 to exit Speak External mode. It is not necessary for
///         software emulations of the TMS5220 or for bitstreams intended to be stored in the TMS6100 Voice Synthesis
///         Memory IC
//
/// \param shouldAppendStopFrame Whether or not to include an explicit stop frame at the end of the bitstream
std::string FrameEncoder::toHex(bool shouldAppendStopFrame) {
    std::string hexStream;

    if (shouldAppendStopFrame) {
        appendStopFrame();
    }

    // Pad final byte with zeros
    auto emptyBitsInLastByte = 8 - bytes.back().size();
    if (emptyBitsInLastByte != 0) {
        bytes.back() += std::string(emptyBitsInLastByte, '0');
    }

    // Reverse each byte and convert to hex
    for (auto byte : bytes) {
        std::reverse(byte.begin(), byte.end());
        hexStream += byteToHex(byte) + byteSeparator;
    }

    // Remove final trailing comma
    hexStream.erase(hexStream.end() - 1);

    return hexStream;
}

/// Serialize the Frame data to a JSON object
std::string FrameEncoder::toJSON() {
    nlohmann::json json;

    for (auto frame : frames) {
        json.push_back(frame.toJSON());
    }

    return json.dump(4);
}

/// Append a stop frame to the end of the bitstream
///
/// \note See \code FrameEncoder::toHex() \endcode for more information on stop frames
void FrameEncoder::appendStopFrame() {
    auto bin = std::string("1111");

    // Check to see if the previous byte is incomplete (contains less than 8 characters), and fill it if so
    auto emptyBitsInLastByte = 8 - bytes.back().size();
    if (emptyBitsInLastByte != 0) {
        bytes.back() += bin.substr(0, emptyBitsInLastByte);
        bin.erase(0, emptyBitsInLastByte);
    }

    // Segment the rest of the binary frame into bytes. The final byte will likely be incomplete, but that will be
    // addressed either in a subsequent call to append() or during hex stream generation
    while (!bin.empty()) {
        auto byte = bin.substr(0, 8);
        bytes.push_back(byte);

        bin.erase(0, 8);
    }
}

/// Convert binary string to its ASCII hex bytes
std::string FrameEncoder::byteToHex(const std::string &byte) const {
    int value = std::stoi(byte, nullptr, 2);

    char hexByte[6];

    if (shouldIncludeHexPrefix) {
        snprintf(hexByte, 5, "0x%02x", value);
    } else {
        snprintf(hexByte, 5, "%02x", value);
    }

    return {hexByte};
}
