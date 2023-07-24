// Copyright (C) 2023 Joseph Bellahcen <joeclb@icloud.com>

#include "Frame_Encoding/FrameEncoder.hpp"

#include <fstream>
#include <string>

#include "lib/json.hpp"

#include "Frame_Encoding/CodingTable.hpp"
#include "Frame_Encoding/Frame.hpp"

namespace tms_express {

///////////////////////////////////////////////////////////////////////////////
// Initializers ///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FrameEncoder::FrameEncoder(bool include_hex_prefix) {
    include_hex_prefix_ = include_hex_prefix;
    frames_ = std::vector<Frame>();
    binary_bitstream_ = std::vector<std::string>(1, "");
}

FrameEncoder::FrameEncoder(const std::vector<Frame> &frames,
    bool include_hex_prefix) {
    binary_bitstream_ = std::vector<std::string>(1, "");
    frames_ = std::vector<Frame>();
    include_hex_prefix_ = include_hex_prefix;

    append(frames);
}

///////////////////////////////////////////////////////////////////////////////
// Frame Appenders ////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FrameEncoder::append(Frame frame) {
    frames_.push_back(frame);
    auto bin = frame.toBinary();

    // The binary representation of a Frame is seldom cleanly divisible into
    // bytes. As such, the first few bits of a Frame may be packed into the
    // empty space of an existing vector element, or the last few bits may
    // partially ccupy a new vector element
    //
    // Check to see if the previous byte is incomplete (contains less than 8
    // characters), and fill it if so
    auto empty_bits_in_last_byte = 8 - binary_bitstream_.back().size();
    if (empty_bits_in_last_byte != 0) {
        binary_bitstream_.back() += bin.substr(0, empty_bits_in_last_byte);
        bin.erase(0, empty_bits_in_last_byte);
    }

    // Segment the rest of the binary frame into binary. The final byte will
    // likely be incomplete, but that will be addressed either in a subsequent
    // call to append() or during hex stream generation
    while (!bin.empty()) {
        auto byte = bin.substr(0, 8);
        binary_bitstream_.push_back(byte);

        bin.erase(0, 8);
    }
}

void FrameEncoder::append(const std::vector<Frame> &frames) {
    for (const auto &frame : frames) {
        append(frame);
    }
}

//////////////////////////////////////////////////////////////////////////////
// (De-)Serialization ////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

size_t FrameEncoder::importASCIIFromFile(const std::string &path) {
    // Flatten bitstream and remove delimiter
    std::ifstream file(path);
    std::string flat = std::string((std::istreambuf_iterator<char>(file)),
                            std::istreambuf_iterator<char>());

    return importASCIIFromString(flat);
}

size_t FrameEncoder::importASCIIFromString(std::string flat_bitstream) {
    // Copy reversed-hex bytes into binary buffer
    std::string buffer = reverseHexBytes(flat_bitstream);

    // Parse frames
    frames_.clear();
    const auto blank_frame = Frame(0, false, 0.0f, std::vector<float>(10, 0.f));

    while (!buffer.empty()) {
        // TODO(Joseph Bellahcen): Handle exception
        auto energy_idx = std::stoul(buffer.substr(0, 4), nullptr, 2);

        // Stop frame
        if (energy_idx == 0xf) {
            break;
        }

        // Silent frame
        if (energy_idx == 0x0) {
            append(blank_frame);
            buffer.erase(0, 4);
            continue;
        }

        auto is_repeat = (buffer[4] == '1');
        // TODO(Joseph Bellahcen): Handle exception
        auto pitch_idx = std::stoul(buffer.substr(5, 6), nullptr, 2);

        auto gain = coding_table::tms5220::rms.at(energy_idx);
        auto pitch = coding_table::tms5220::pitch.at(pitch_idx);

        if (is_repeat) {
            append(Frame(pitch, false, gain, std::vector<float>(10, 0.0f)));
            frames_.end()->setRepeat(true);
            buffer.erase(0, 11);
            continue;
        }

        float k1, k2, k3, k4, k5, k6, k7, k8, k9, k10 = 0;
        extractUnvoicedCoeffs(buffer, &k1, &k2, &k3, &k4);

        if (pitch == 0x0) {
            buffer.erase(0, 29);

        } else {
            extractVoicedCoeffs(buffer, &k5, &k6, &k7, &k8, &k9, &k10);

            buffer.erase(0, 50);
        }

        append(
            Frame(pitch,
            pitch != 0x0,
            gain,
            std::vector<float>{k1, k2, k3, k4, k5, k6, k7, k8, k9, k10}));
    }

    return frames_.size();
}

std::string FrameEncoder::toHex(bool shouldAppendStopFrame) {
    std::string hex_stream;

    if (shouldAppendStopFrame) {
        appendStopFrame();
    }

    // Pad final byte with zeros
    auto n_empty_bits_in_last_byte = 8 - binary_bitstream_.back().size();
    if (n_empty_bits_in_last_byte != 0) {
        binary_bitstream_.back() += std::string(n_empty_bits_in_last_byte, '0');
    }

    // Reverse each byte and convert to hex
    for (auto byte : binary_bitstream_) {
        std::reverse(byte.begin(), byte.end());
        hex_stream += binToHex(byte, include_hex_prefix_) + byte_delimiter;
    }

    // Remove final trailing comma
    hex_stream.erase(hex_stream.end() - 1);
    return hex_stream;
}

std::vector<std::byte> FrameEncoder::toBytes(bool append_stop_frame) {
    auto bytes = std::vector<std::byte>();

    if (append_stop_frame) {
        appendStopFrame();
    }

    // Pad final byte with zeros
    auto n_empty_bits_in_last_byte = 8 - binary_bitstream_.back().size();
    if (n_empty_bits_in_last_byte != 0) {
        binary_bitstream_.back() += std::string(n_empty_bits_in_last_byte, '0');
    }

    // Reverse each byte and convert to hex
    for (auto byte : binary_bitstream_) {
        std::reverse(byte.begin(), byte.end());
        auto data = std::byte(
            std::stoul(binToHex(byte, include_hex_prefix_), nullptr, 16));
        bytes.push_back(data);
    }

    return bytes;
}

std::string FrameEncoder::toJSON() const {
    nlohmann::json json;

    for (auto frame : frames_) {
        json.push_back(frame.toJSON());
    }

    return json.dump(4);
}

///////////////////////////////////////////////////////////////////////////////
// Accessors //////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

std::vector<Frame> FrameEncoder::getFrameTable() const {
    return frames_;
}

///////////////////////////////////////////////////////////////////////////////
// Static Helpers /////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

std::string FrameEncoder::binToHex(const std::string &bin_str,
    bool include_hex_prefix) {
    // TODO(Joseph Bellahcen): Handle exception
    int value = std::stoi(bin_str, nullptr, 2);

    char hex_byte[6];

    if (include_hex_prefix) {
        snprintf(hex_byte, sizeof(hex_byte), "0x%02x", value);
    } else {
        snprintf(hex_byte, sizeof(hex_byte), "%02x", value);
    }

    return {hex_byte};
}

std::string FrameEncoder::reverseHexBytes(std::string bitstream) {
    std::string buffer;

    bitstream.erase(std::remove(bitstream.begin(), bitstream.end(), ','),
        bitstream.end());

    // TODO(Joseph Bellahcen): Handle prefix/no prefix
    for (int i = 0; i < bitstream.size() - 1; i += 4) {
        auto substr = bitstream.substr(i, 4);
        std::reverse(substr.begin(), substr.end());

        // TODO(Joseph Bellahcen): Handle exception
        uint8_t byte = std::stoul(substr, nullptr, 16);
        auto bin = std::bitset<8>(byte);

        auto first_half = bin.to_string().substr(0, 4);
        auto second_half = bin.to_string().substr(4, 4);

        std::reverse(first_half.begin(), first_half.end());
        std::reverse(second_half.begin(), second_half.end());
        buffer += (first_half + second_half);
    }

    return buffer;
}

void FrameEncoder::extractUnvoicedCoeffs(const std::string &chunk, float *k1,
    float *k2, float *k3, float *k4) {
        auto k1_idx = std::stoul(chunk.substr(11, 5), nullptr, 2);
        auto k2_idx = std::stoul(chunk.substr(16, 5), nullptr, 2);
        auto k3_idx = std::stoul(chunk.substr(21, 4), nullptr, 2);
        auto k4_idx = std::stoul(chunk.substr(25, 4), nullptr, 2);

        // TODO(Joseph Bellahcen): Guard against nullptr dereference
        *k1 = coding_table::tms5220::k1.at(k1_idx);
        *k2 = coding_table::tms5220::k2.at(k2_idx);
        *k3 = coding_table::tms5220::k3.at(k3_idx);
        *k4 = coding_table::tms5220::k4.at(k4_idx);
}

void FrameEncoder::extractVoicedCoeffs(const std::string &chunk, float *k5,
    float *k6, float *k7, float *k8, float *k9, float *k10) {
    auto k5_idx = std::stoul(chunk.substr(29, 4), nullptr, 2);
    auto k6_idx = std::stoul(chunk.substr(33, 4), nullptr, 2);
    auto k7_idx = std::stoul(chunk.substr(37, 4), nullptr, 2);
    auto k8_idx = std::stoul(chunk.substr(41, 3), nullptr, 2);
    auto k9_idx = std::stoul(chunk.substr(44, 3), nullptr, 2);
    auto k10_idx = std::stoul(chunk.substr(47, 3), nullptr, 2);

    // TODO(Joseph Bellahcen): Guard against nullptr dereference
    *k5 = coding_table::tms5220::k5.at(k5_idx);
    *k6 = coding_table::tms5220::k6.at(k6_idx);
    *k7 = coding_table::tms5220::k7.at(k7_idx);
    *k8 = coding_table::tms5220::k8.at(k8_idx);
    *k9 = coding_table::tms5220::k9.at(k9_idx);
    *k10 = coding_table::tms5220::k10.at(k10_idx);
}

///////////////////////////////////////////////////////////////////////////////
// Helpers ////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FrameEncoder::appendStopFrame() {
    std::string bin = "1111";

    // Check to see if the previous byte is incomplete (contains less than 8
    // characters), and fill it if so
    auto empty_bits_in_last_byte = 8 - binary_bitstream_.back().size();
    if (empty_bits_in_last_byte != 0) {
        binary_bitstream_.back() += bin.substr(0, empty_bits_in_last_byte);
        bin.erase(0, empty_bits_in_last_byte);
    }

    // Segment the rest of the binary frame into bytes. The final byte will
    // likely be incomplete, but that will be addressed either in a subsequent
    // call to append() or during hex stream generation
    while (!bin.empty()) {
        auto byte = bin.substr(0, 8);
        binary_bitstream_.push_back(byte);

        bin.erase(0, 8);
    }
}

};  // namespace tms_express
