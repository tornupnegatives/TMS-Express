//
// Created by Joseph Bellahcen on 5/1/22.
//

#ifndef TMS_EXPRESS_FRAMEBITSTREAM_H
#define TMS_EXPRESS_FRAMEBITSTREAM_H

#include "Frame_Encoding/Frame.h"

#include <bitset>

class FrameEncoder {
public:
    FrameEncoder(Frame **frames, int count);
    ~FrameEncoder();

    void serialize(char *path);

private:
    Frame **frames;
    int count;
    std::string binStream;
    std::string hexStream;

    static constexpr int energySize = 4;
    static constexpr int pitchSize = 6;
    static constexpr int k1_k2Size = 5;
    static constexpr int k3_k7Size = 4;
    static constexpr int k8_k10Size = 3;

    void frameToBinary(Frame *frame);
    void binToHex(const char *bin);
    void appendStopFrame();
};

#endif //TMS_EXPRESS_FRAMEBITSTREAM_H
