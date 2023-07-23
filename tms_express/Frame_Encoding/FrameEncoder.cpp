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
#include "Frame_Encoding/Frame.hpp"
#include "Frame_Encoding/Tms5220CodingTable.h"

#include "json.hpp"

#include <algorithm>
#include <bitset>
#include <cstddef>
#include <cstdio>
#include <fstream>
#include <string>

namespace tms_express {

/// Create a new Frame Encoder with an empty frame buffer
///
/// \param includeHexPrefix Whether or not to include '0x' before hex bytes
/// \param separator Character with which to separate hex bytes
FrameEncoder::FrameEncoder(bool includeHexPrefix, char separator) {
    shouldIncludeHexPrefix = includeHexPrefix;
    byteSeparator = separator;
    frames = std::vector<Frame>();
    binary = std::vector<std::string>(1, "");
}

/// Create a new Frame Encoder and populate it with frames
///
/// \param initialFrames Frames with which to populate encoder
/// \param includeHexPrefix Whether or not to include '0x' before hex bytes
/// \param separator Character with which to separate hex bytes
FrameEncoder::FrameEncoder(const std::vector<Frame> &initialFrames, bool includeHexPrefix, char separator) {
    binary = std::vector<std::string>(1, "");
    byteSeparator = separator;
    frames = std::vector<Frame>();
    shouldIncludeHexPrefix = includeHexPrefix;

    append(initialFrames);
}

///////////////////////////////////////////////////////////////////////////////
//                          Append Functions
///////////////////////////////////////////////////////////////////////////////

/// Append binary representation of a frame to the end of the encoder buffer
///
/// \note   The binary representation of a Frame is seldom cleanly divisible into bytes. As such, the first few bits of
///         a Frame may be packed into the empty space of an existing vector element, or the last few bits may partially
///         occupy a new vector element
void FrameEncoder::append(Frame frame) {
    frames.push_back(frame);
    auto bin = frame.toBinary();

    // Check to see if the previous byte is incomplete (contains less than 8 characters), and fill it if so
    auto emptyBitsInLastByte = 8 - binary.back().size();
    if (emptyBitsInLastByte != 0) {
        binary.back() += bin.substr(0, emptyBitsInLastByte);
        bin.erase(0, emptyBitsInLastByte);
    }

    // Segment the rest of the binary frame into binary. The final byte will likely be incomplete, but that will be
    // addressed either in a subsequent call to append() or during hex stream generation
    while (!bin.empty()) {
        auto byte = bin.substr(0, 8);
        binary.push_back(byte);

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

///////////////////////////////////////////////////////////////////////////////
//                          Import Functions
///////////////////////////////////////////////////////////////////////////////

/// Import ASCII bitstream (CSV) from disk
///
/// \param path Path to comma-delimited ASCII bytes
/// \return Number of frames imported
size_t FrameEncoder::importFromAscii(const std::string &path) {
    // Flatten bitstream and remove delimiter
    std::ifstream file(path);
    std::string flatBitstream = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    return parseAsciiBitstream(flatBitstream);
}

///////////////////////////////////////////////////////////////////////////////
//                              Serialization
///////////////////////////////////////////////////////////////////////////////

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
    auto emptyBitsInLastByte = 8 - binary.back().size();
    if (emptyBitsInLastByte != 0) {
        binary.back() += std::string(emptyBitsInLastByte, '0');
    }

    // Reverse each byte and convert to hex
    for (auto byte: binary) {
        std::reverse(byte.begin(), byte.end());
        hexStream += byteToHex(byte) + byteSeparator;
    }

    // Remove final trailing comma
    hexStream.erase(hexStream.end() - 1);

    return hexStream;
}

/// Serialize frame data to a vector of raw bytes
///
/// \return Frame table data represented as bytes
std::vector<std::byte> FrameEncoder::toBin(bool shouldAppendStopFrame) {
    auto bytes = std::vector<std::byte>();

    if (shouldAppendStopFrame) {
        appendStopFrame();
    }

    // Pad final byte with zeros
    auto emptyBitsInLastByte = 8 - binary.back().size();
    if (emptyBitsInLastByte != 0) {
        binary.back() += std::string(emptyBitsInLastByte, '0');
    }

    // Reverse each byte and convert to hex
    for (auto byte: binary) {
        std::reverse(byte.begin(), byte.end());
        auto data = std::byte(std::stoul(byteToHex(byte), nullptr, 16));
        bytes.push_back(data);
    }

    return bytes;
}

/// Serialize the Frame data to a JSON object
std::string FrameEncoder::toJSON() {
    nlohmann::json json;

    for (auto frame: frames) {
        json.push_back(frame.toJSON());
    }

    return json.dump(4);
}

/// Pass the frame table vector
std::vector<Frame> FrameEncoder::frameTable() {
    return frames;
}

///////////////////////////////////////////////////////////////////////////////
//                              Helpers
///////////////////////////////////////////////////////////////////////////////

/// Append a stop frame to the end of the bitstream
///
/// \note See \code FrameEncoder::toHex() \endcode for more information on stop frames
void FrameEncoder::appendStopFrame() {
    auto bin = std::string("1111");

    // Check to see if the previous byte is incomplete (contains less than 8 characters), and fill it if so
    auto emptyBitsInLastByte = 8 - binary.back().size();
    if (emptyBitsInLastByte != 0) {
        binary.back() += bin.substr(0, emptyBitsInLastByte);
        bin.erase(0, emptyBitsInLastByte);
    }

    // Segment the rest of the binary frame into bytes. The final byte will likely be incomplete, but that will be
    // addressed either in a subsequent call to append() or during hex stream generation
    while (!bin.empty()) {
        auto byte = bin.substr(0, 8);
        binary.push_back(byte);

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

size_t FrameEncoder::parseAsciiBitstream(std::string flatBitstream) {
    // Copy reversed-hex bytes into binary buffer
    std::string buffer;
    flatBitstream.erase(std::remove(flatBitstream.begin(), flatBitstream.end(), ','), flatBitstream.end());

    for (int i = 0; i < flatBitstream.size() - 1; i += 4) {
        auto substr = flatBitstream.substr(i, 4);
        std::reverse(substr.begin(), substr.end());
        uint8_t byte = std::stoul(substr, nullptr, 16);
        auto bin = std::bitset<8>(byte);

        auto firstHalf = bin.to_string().substr(0, 4);
        auto secondhalf = bin.to_string().substr(4, 4);

        std::reverse(firstHalf.begin(), firstHalf.end());
        std::reverse(secondhalf.begin(), secondhalf.end());
        buffer += (firstHalf + secondhalf);
    }

    // Parse frames
    frames.clear();
    const auto blankFrame = Frame(0, false, 0.0f, std::vector<float>(10, 0.0f));

    while (!buffer.empty()) {
        auto energyIdx = std::stoul(buffer.substr(0, 4), nullptr, 2);

        // Stop frame
        if (energyIdx == 0xf) {
            break;
        }

        // Silent frame
        if (energyIdx == 0x0) {
            append(blankFrame);
            buffer.erase(0, 4);
            continue;
        }

        auto isRepeat = (buffer[4] == '1');
        auto pitchIdx = std::stoul(buffer.substr(5, 6), nullptr, 2);

        auto gain = Tms5220CodingTable::rms.at(energyIdx);
        auto pitch = int(Tms5220CodingTable::pitch.at(pitchIdx));

        if (isRepeat) {
            append(Frame(pitch, false, gain, std::vector<float>(10, 0.0f)));
            frames.end()->setRepeat(true);
            buffer.erase(0, 11);
            continue;
        }

        auto kIdx1 = std::stoul(buffer.substr(11, 5), nullptr, 2);
        auto kIdx2 = std::stoul(buffer.substr(16, 5), nullptr, 2);
        auto kIdx3 = std::stoul(buffer.substr(21, 4), nullptr, 2);
        auto kIdx4 = std::stoul(buffer.substr(25, 4), nullptr, 2);

        auto k1 = Tms5220CodingTable::k1.at(kIdx1);
        auto k2 = Tms5220CodingTable::k2.at(kIdx2);
        auto k3 = Tms5220CodingTable::k3.at(kIdx3);
        auto k4 = Tms5220CodingTable::k4.at(kIdx4);

        auto k5 = 0.0f;
        auto k6 = 0.0f;
        auto k7 = 0.0f;
        auto k8 = 0.0f;
        auto k9 = 0.0f;
        auto k10 = 0.0f;

        if (pitch == 0x0) {
            buffer.erase(0, 29);

        } else {
            auto kIdx5 = std::stoul(buffer.substr(29, 4), nullptr, 2);
            auto kIdx6 = std::stoul(buffer.substr(33, 4), nullptr, 2);
            auto kIdx7 = std::stoul(buffer.substr(37, 4), nullptr, 2);
            auto kIdx8 = std::stoul(buffer.substr(41, 3), nullptr, 2);
            auto kIdx9 = std::stoul(buffer.substr(44, 3), nullptr, 2);
            auto kIdx10 = std::stoul(buffer.substr(47, 3), nullptr, 2);

            k5 = Tms5220CodingTable::k5.at(kIdx5);
            k6 = Tms5220CodingTable::k6.at(kIdx6);
            k7 = Tms5220CodingTable::k7.at(kIdx7);
            k8 = Tms5220CodingTable::k8.at(kIdx8);
            k9 = Tms5220CodingTable::k9.at(kIdx9);
            k10 = Tms5220CodingTable::k10.at(kIdx10);

            buffer.erase(0, 50);
        }

        append(Frame(pitch, pitch != 0x0, gain, std::vector<float>{k1, k2, k3, k4, k5, k6, k7, k8, k9, k10}));
    }

    return frames.size();
}

};  // namespace tms_express
