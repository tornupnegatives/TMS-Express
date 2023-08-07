// Copyright 2023 Joseph Bellahcen <joeclb@icloud.com>

#include <gtest/gtest.h>

#include <vector>

#include "Frame_Encoding/Frame.hpp"

namespace tms_express {

Frame frameTestSubject() {
    return Frame(38, true, 56.850773,
        {-0.753234, 0.939525, -0.342255, -0.172317, 0.108887, 0.679660,
            0.056874, 0.433271, -0.220355, 0.17028});
}

TEST(FrameTests, SilentFrameContainsOnlyEnergyParameter) {
    auto frame = frameTestSubject();
    frame.setGain(0.0f);

    EXPECT_TRUE(frame.isSilent());
    EXPECT_EQ(frame.toBinary(), "0000");
}

TEST(FrameTests, UnvoicedFrameHasEnergyNoPitchRepeatBitAndCoeffs) {
    auto frame = frameTestSubject();
    frame.setVoicing(false);
    auto bin = frame.toBinary();

    // energy[4] + repeat[1] + pitch[6] +
    // K1[5] + K2[5] + K3[4] + K3[4] = 29 bits
    EXPECT_EQ(bin.size(), 29);

    // Energy = 38 ~= 52 @ [1]
    auto energy_bin = bin.substr(0, 4);
    EXPECT_EQ(energy_bin, "0001");

    // Repeat = 0
    auto repeat_bin = bin.substr(4, 1);
    EXPECT_EQ(repeat_bin, "0");

    // Pitch = 38 ~= 38 @ [23]
    // For unvoiced frames, pitch is ALWAYS b000000
    auto pitch_bin = bin.substr(5, 6);
    EXPECT_EQ(pitch_bin, "000000");

    // K1 = -0.753234 ~= -0.74058 @ [17]
    auto k1_bin = bin.substr(11, 5);
    EXPECT_EQ(k1_bin, "10001");

    // K2 = 0.939525 ~= 0.92988 @ [30]
    auto k2_bin = bin.substr(16, 5);
    EXPECT_EQ(k2_bin, "11110");

    // K3 = -0.342255 ~= -0.33333 @ [5]
    auto k3_bin = bin.substr(21, 4);
    EXPECT_EQ(k3_bin, "0101");

    // K4 = -0.172317 ~= 0.20579 @ [4]
    auto k4_bin = bin.substr(25, 4);
    EXPECT_EQ(k4_bin, "0100");
}

TEST(FrameTests, VoicedFrameHasFullParameterSet) {
    auto frame = frameTestSubject();
    auto bin = frame.toBinary();

    EXPECT_EQ(bin.size(), 50);

    // Energy = 38 ~= 52 @ [1]
    auto energy_bin = bin.substr(0, 4);
    EXPECT_EQ(energy_bin, "0001");

    // Repeat = 0
    auto repeat_bin = bin.substr(4, 1);
    EXPECT_EQ(repeat_bin, "0");

    // Pitch = 38 ~= 38 @ [24]
    auto pitch_bin = bin.substr(5, 6);
    EXPECT_EQ(pitch_bin, "011000");

    // K1 = -0.753234 ~= -0.74058 @ [17]
    auto k1_bin = bin.substr(11, 5);
    EXPECT_EQ(k1_bin, "10001");

    // K2 = 0.939525 ~= 0.92988 @ [30]
    auto k2_bin = bin.substr(16, 5);
    EXPECT_EQ(k2_bin, "11110");

    // K3 = -0.342255 ~= -0.33333 @ [5]
    auto k3_bin = bin.substr(21, 4);
    EXPECT_EQ(k3_bin, "0101");

    // K4 = -0.172317 ~= 0.20579 @ [4]
    auto k4_bin = bin.substr(25, 4);
    EXPECT_EQ(k4_bin, "0100");

    // K5 = 0.108887 ~= 0.08533 @ [8]
    auto k5_bin = bin.substr(29, 4);
    EXPECT_EQ(k5_bin, "1000");

    // K6 = 0.679660 ~= 0.71333 @ [14]
    auto k6_bin = bin.substr(33, 4);
    EXPECT_EQ(k6_bin, "1110");

    // K7 = 0.056874 ~= 0.05333 @ [7]
    auto k7_bin = bin.substr(37, 4);
    EXPECT_EQ(k7_bin, "0111");

    // K8 = 0.433271 ~= 0.42857 @ [5]
    auto k8_bin = bin.substr(41, 3);
    EXPECT_EQ(k8_bin, "101");

    // K9 = -0.220355 ~= -0.18571 @ [2]
    auto k9_bin = bin.substr(44, 3);
    EXPECT_EQ(k9_bin, "010");

    // K10 = 0.17028 ~= 0.17143 @ [4]
    auto k10_bin = bin.substr(47, 3);
    EXPECT_EQ(k10_bin, "100");
}

};  // namespace tms_express
