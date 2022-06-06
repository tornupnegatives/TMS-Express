#include "Frame_Encoding/Frame.h"
#include "Frame_Encoding/FrameEncoder.h"
#include <gtest/gtest.h>
#include <vector>

// The hex stream should, at minimum, contain a StopFrame
TEST(FrameEncoderTests, StopFrame) {
    auto frameEncoder = FrameEncoder();
    auto hex = frameEncoder.toHex();

    EXPECT_EQ(hex, "0f");

}

// A silent frame will result in four zero bits
//
// Expected: silent frame, stop frame
//           0000          1111
//           00001111 -> 11110000
//           f0
TEST(FrameEncoderTests, SilentFrame) {
    auto frameEncoder = FrameEncoder();
    auto silentFrame = Frame(0, false, 0.0f, std::vector<float>());

    frameEncoder.appendFrame(silentFrame);
    auto hex = frameEncoder.toHex();

    EXPECT_EQ(hex, "f0");
}

// A voiced Frame will produce 50 data bits. The output will also contain 4 stop bits. The Frame is "flattened" into a
// binary string representing its contents, which is then sliced into bytes. Each byte is then reversed, as the TMS5100
// Voice Synthesis Memory units which normally feed data into the TMS5220 send byte-wise data LSB first
TEST(FrameEncoderTests, VoicedFrame) {
    auto voicedFrame = Frame(38, true, 56.850773,
                             {-0.753234, 0.939525, -0.342255, -0.172317, 0.108887, 0.679660, 0.056874, 0.433271,
                                    -0.220355, 0.17028});

    auto frameEncoder = FrameEncoder();
    frameEncoder.appendFrame(voicedFrame);

    auto bin = frameEncoder.toHex();
    EXPECT_EQ(bin, "c8,88,4f,25,ce,ab,3c");
}

// An unvoiced Frame will produce 29 data bits. The output will also contain 4 stop bits.
TEST(FrameEncoderTests, UnvoicedFrame) {
    auto voicedFrame = Frame(38, true, 56.850773,
                             {-0.753234, 0.939525, -0.342255, -0.172317, 0.108887, 0.679660, 0.056874, 0.433271,
                              -0.220355, 0.17028});

    auto frameEncoder = FrameEncoder();
    frameEncoder.appendFrame(voicedFrame);

    auto bin = frameEncoder.toHex();
    EXPECT_EQ(bin, "c8,88,4f,25,ce,ab,3c");
}

TEST(FrameEncoderTests, MixtureOfFrames) {
    auto unvoicedFrame = Frame(38, true, 56.850773,
                               {-0.753234, 0.939525, -0.342255, -0.172317, 0.108887, 0.679660, 0.056874, 0.433271,
                                -0.220355, 0.17028});
    unvoicedFrame.setVoicing(false);

    auto frameEncoder = FrameEncoder();
    frameEncoder.appendFrame(unvoicedFrame);

    auto bin = frameEncoder.toHex();
    EXPECT_EQ(bin, "08,88,4f,e5,01");

}