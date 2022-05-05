//
// Created by Joseph Bellahcen on 5/1/22.
//

#include "Frame_Encoding/FrameEncoder.h"

#include <bitset>
#include <cstdlib>

FrameEncoder::FrameEncoder(Frame **frames, int count) {
    FrameEncoder::frames = frames;
    FrameEncoder::count = count;

    FrameEncoder::binStream = std::string();
    FrameEncoder::hexStream = std::string();
}

FrameEncoder::~FrameEncoder() {
    // Free frames
    for (int i = 0; i < count; i++) {
        if (frames[i] != nullptr) {
            delete frames[i];
        }
    }

    if (frames != nullptr) {
        free(frames);
    }
}

void FrameEncoder::serialize(char *path) {
    // Convert all frames into a single bitstream
    for (int i = 0; i < count; i++) {
        auto frame = frames[i];
        frameToBinary(frame);
    }

    // A stop frame signals to the TMS5220 that
    // the talk sequence is finished
    appendStopFrame();

    // Convert the binary stream to hex, one byte at a time
    for (int i = 0; i < binStream.size(); i+= 8) {
        auto byte = binStream.substr(i, 8);;
        binToHex(byte.c_str());
    }

    // Remove trailing comma
    hexStream.erase(hexStream.size() - 1);
    printf("%s\n", hexStream.c_str());

    FILE *lpcOut = fopen(path, "w");
    if (lpcOut != nullptr) {
        fputs(hexStream.c_str(), lpcOut);
        fclose(lpcOut);
    }
}

void FrameEncoder::frameToBinary(Frame *frame) {
    // At minimum, a frame will contain an energy parameter
    int energy = frame->getQuantizedEnergy();
    auto energyBin = std::bitset<energySize>(energy).to_string();
    binStream.append(energyBin);

    // A zero energy frame contains no further parameters
    if (energy == 0) {
        return;
    }

    // TODO: Detect repeat frames
    // A repeat frame will also contain the repeat and pitch parameters
    bool repeat = false;
    binStream.append(repeat ? "1" : "0");

    // An unvoiced frame has zero pitch
    bool voiced = frame->getQuantizedVoicing();
    int pitch =  voiced ? frame->getQuantizedPitch() : 0;

    auto pitchBin = std::bitset<pitchSize>(pitch).to_string();
    binStream.append(pitchBin);

    if (repeat) {
        return;
    }

    // Both voiced and unvoiced frames contain reflector coefficients or
    // "k parameters." Unvoiced frames contain four, while voiced frames contain ten
    int *coeffs = frame->getQuantizedCoefficients();
    const int numParams = voiced ? 10 : 4;

    // Not all k parameters are the same width
    for (int i = 0; i < numParams; i++) {
        int coeff = coeffs[i];
        std::string coeffBin;

        if (i < 2) {
            coeffBin = std::bitset<k1_k2Size>(coeff).to_string();

        } else if (i < 7) {
            coeffBin = std::bitset<k3_k7Size>(coeff).to_string();

        } else {
            coeffBin = std::bitset<k8_k10Size>(coeff).to_string();
        }

        binStream.append(coeffBin);
    }

    free(coeffs);
}

void FrameEncoder::binToHex(const char *bin) {
    std::string binStr = std::string(bin);
    reverse(binStr.begin(), binStr.end());
    int byte = std::stoi(binStr, nullptr, 2);

    char hexByte[4];
    sprintf(hexByte, "%02x,", byte);
    hexStream.append(hexByte);
}

// Pack a stop frame and add it to the filestream
void FrameEncoder::appendStopFrame() {
    const int stopFrame = 0x0f;
    auto energyBin = std::bitset<energySize>(stopFrame).to_string();
    binStream.append(energyBin);
}
