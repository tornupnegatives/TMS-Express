#include "Frame_Encoding/Frame.hpp"
#include <gtest/gtest.h>
#include <vector>

namespace tms_express {

Frame frameTestSubject() {
    return Frame(38, true, 56.850773, {-0.753234, 0.939525, -0.342255, -0.172317,
                                                                        0.108887, 0.679660, 0.056874, 0.433271,
                                                                        -0.220355, 0.17028});
}

// A silent frame should contain only an energy parameter
TEST(FrameTests, SilentFrame) {
    auto frame = frameTestSubject();
    frame.setGain(0.0f);

    EXPECT_TRUE(frame.isSilent());
    EXPECT_EQ(frame.toBinary(), "0000");
}

/*
TEST(FrameTests, RepeatFrame) {

}
 */

// An unvoiced frame has an energy parameter, zero pitch, a repeat bit, and four reflector coefficients
TEST(FrameTests, UnvoicedFrame) {
    auto frame = frameTestSubject();
    frame.setVoicing(false);

    auto bin = frame.toBinary();

    // energy[4] + repeat[1] + pitch[6] + K1[5] + K2[5] + K3[4] + K3[4] = 29 bits
    EXPECT_EQ(bin.size(), 29);

    // Energy = 38 ~= 52 @ [1]
    auto energyBin = bin.substr(0, 4);
    EXPECT_EQ(energyBin, "0001");

    // Repeat = 0
    auto repeatBin = bin.substr(4, 1);
    EXPECT_EQ(repeatBin, "0");

    // Pitch = 38 ~= 38 @ [23]
    // For unvoiced frames, pitch is ALWAYS b000000
    auto pitchBin = bin.substr(5, 6);
    EXPECT_EQ(pitchBin, "000000");

    // K1 = -0.753234 ~= -0.74058 @ [17]
    auto k1Bin = bin.substr(11, 5);
    EXPECT_EQ(k1Bin, "10001");

    // K2 = 0.939525 ~= 0.92988 @ [30]
    auto k2Bin = bin.substr(16, 5);
    EXPECT_EQ(k2Bin, "11110");

    // K3 = -0.342255 ~= -0.33333 @ [5]
    auto k3Bin = bin.substr(21, 4);
    EXPECT_EQ(k3Bin, "0101");

    // K4 = -0.172317 ~= 0.20579 @ [4]
    auto k4Bin = bin.substr(25, 4);
    EXPECT_EQ(k4Bin, "0100");
}

// A voiced frame will have a full set of parameters
TEST(FrameTests, VoicedFrame) {
    auto frame = frameTestSubject();
    auto bin = frame.toBinary();

    EXPECT_EQ(bin.size(), 50);

    // Energy = 38 ~= 52 @ [1]
    auto energyBin = bin.substr(0, 4);
    EXPECT_EQ(energyBin, "0001");

    // Repeat = 0
    auto repeatBin = bin.substr(4, 1);
    EXPECT_EQ(repeatBin, "0");

    // Pitch = 38 ~= 38 @ [24]
    auto pitchBin = bin.substr(5, 6);
    EXPECT_EQ(pitchBin, "011000");

    // K1 = -0.753234 ~= -0.74058 @ [17]
    auto k1Bin = bin.substr(11, 5);
    EXPECT_EQ(k1Bin, "10001");

    // K2 = 0.939525 ~= 0.92988 @ [30]
    auto k2Bin = bin.substr(16, 5);
    EXPECT_EQ(k2Bin, "11110");

    // K3 = -0.342255 ~= -0.33333 @ [5]
    auto k3Bin = bin.substr(21, 4);
    EXPECT_EQ(k3Bin, "0101");

    // K4 = -0.172317 ~= 0.20579 @ [4]
    auto k4Bin = bin.substr(25, 4);
    EXPECT_EQ(k4Bin, "0100");

    // K5 = 0.108887 ~= 0.08533 @ [8]
    auto k5Bin = bin.substr(29, 4);
    EXPECT_EQ(k5Bin, "1000");

    // K6 = 0.679660 ~= 0.71333 @ [14]
    auto k6Bin = bin.substr(33, 4);
    EXPECT_EQ(k6Bin, "1110");

    // K7 = 0.056874 ~= 0.05333 @ [7]
    auto k7Bin = bin.substr(37, 4);
    EXPECT_EQ(k7Bin, "0111");

    // K8 = 0.433271 ~= 0.42857 @ [5]
    auto k8Bin = bin.substr(41, 3);
    EXPECT_EQ(k8Bin, "101");

    // K9 = -0.220355 ~= -0.18571 @ [2]
    auto k9Bin = bin.substr(44, 3);
    EXPECT_EQ(k9Bin, "010");

    // K10 = 0.17028 ~= 0.17143 @ [4]
    auto k10Bin = bin.substr(47, 3);
    EXPECT_EQ(k10Bin, "100");
}

};  // namespace tms_express
