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
#include <algorithm>
#include <cstdio>
#include <nlohmann/json.hpp>

FrameEncoder::FrameEncoder(bool hexPrefix, char separator) {
    includeHexPrefix = hexPrefix;
    byteSeparator = separator;
    frames = std::vector<Frame>();
    bytes = std::vector<std::string>(1, "");
}

FrameEncoder::FrameEncoder(const std::vector<Frame> &initFrames, bool hexPrefix, char separator) {
    includeHexPrefix = hexPrefix;
    byteSeparator = separator;
    frames = std::vector<Frame>();
    bytes = std::vector<std::string>(1, "");

    appendFrames(initFrames);
}

// Extract the binary representation of a Frame, segment it into bytes, and store the data
//
// The binary representation of a Frame is seldom cleanly divisible into bytes. As such, the first few bits of a Frame
// may be packed into the empty space of an existing vector element, or the last few bits may partially occupy a new
// vector element
void FrameEncoder::appendFrame(Frame frame) {
    frames.push_back(frame);
    auto bin = frame.toBinary();

    // Check to see if the previous byte is incomplete (contains less than 8 characters), and fill it if so
    auto emptyBitsInLastByte = 8 - bytes.back().size();
    if (emptyBitsInLastByte != 0) {
        bytes.back() += bin.substr(0, emptyBitsInLastByte);
        bin.erase(0, emptyBitsInLastByte);
    }

    // Segment the rest of the binary frame into bytes. The final byte will likely be incomplete, but that will be
    // addressed either in a subsequent call to appendFrame() or during hex stream generation
    while (!bin.empty()) {
        auto byte = bin.substr(0, 8);
        bytes.push_back(byte);

        bin.erase(0, 8);
    }
}

// Extract the binary representation of an entire vector of Frames, and slice it into bytes
void FrameEncoder::appendFrames(const std::vector<Frame> &initFrames) {
    for (const auto &frame: initFrames) {
        appendFrame(frame);
    }
}

// Serialize the Frame data to a stream of hex bytes
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

// Serialize the Frame data to a JSON object
std::string FrameEncoder::toJSON() {
    nlohmann::json json;

    for (auto frame : frames) {
        json.push_back(frame.toJSON());
    }

    return json.dump(4);
}

// End the hex stream with a stop frame, which signifies to the TMS5220 that the Speak External command has completed
// and the device should halt execution
void FrameEncoder::appendStopFrame() {
    auto bin = std::string("1111");

    // Check to see if the previous byte is incomplete (contains less than 8 characters), and fill it if so
    auto emptyBitsInLastByte = 8 - bytes.back().size();
    if (emptyBitsInLastByte != 0) {
        bytes.back() += bin.substr(0, emptyBitsInLastByte);
        bin.erase(0, emptyBitsInLastByte);
    }

    // Segment the rest of the binary frame into bytes. The final byte will likely be incomplete, but that will be
    // addressed either in a subsequent call to appendFrame() or during hex stream generation
    while (!bin.empty()) {
        auto byte = bin.substr(0, 8);
        bytes.push_back(byte);

        bin.erase(0, 8);
    }
}

// Convert binary string to its hexadecimal representation
std::string FrameEncoder::byteToHex(const std::string &byte) const {
    int value = std::stoi(byte, nullptr, 2);

    char hexByte[6];

    if (includeHexPrefix) {
        snprintf(hexByte, 5, "0x%02x", value);
    } else {
        snprintf(hexByte, 5, "%02x", value);
    }

    return {hexByte};
}
