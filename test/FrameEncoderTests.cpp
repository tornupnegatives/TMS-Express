#include "Frame_Encoding/Frame.h"
#include "Frame_Encoding/FrameEncoder.h"

#include <gtest/gtest.h>

#include <cstddef>
#include <vector>

namespace tms_express {

// The hex stream should, at minimum, contain a StopFrame
TEST(FrameEncoderTests, StopFrame) {
    auto frameEncoder = FrameEncoder();

    auto hex = frameEncoder.toHex();
    EXPECT_EQ(hex, "0f");
}

TEST(FrameEncoderTests, AsciiStopFrame) {
    auto frameEncoder = FrameEncoder();

    frameEncoder.parseAsciiBitstream("0x0f");

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

    frameEncoder.append(silentFrame);

    auto hex = frameEncoder.toHex();
    EXPECT_EQ(hex, "f0");
}

TEST(FrameEncoderTests, AsciiSilentFrame) {
    auto frameEncoder = FrameEncoder();
    frameEncoder.parseAsciiBitstream("0xf0");

    auto hex = frameEncoder.toHex();
    EXPECT_EQ(hex, "f0");
}

// A voiced Frame will produce 50 data bits. The output will also contain 4 stop bits. The Frame is "flattened" into a
// binary string representing its contents, which is then sliced into binary. Each byte is then reversed, as the TMS5100
// Voice Synthesis Memory units which normally feed data into the TMS5220 send byte-wise data LSB first
TEST(FrameEncoderTests, VoicedFrame) {
    auto voicedFrame = Frame(38, true, 56.850773,
                             {-0.753234, 0.939525, -0.342255, -0.172317, 0.108887, 0.679660, 0.056874, 0.433271,
                                    -0.220355, 0.17028});

    auto frameEncoder = FrameEncoder();
    frameEncoder.append(voicedFrame);

    auto bin = frameEncoder.toHex();
    EXPECT_EQ(bin, "c8,88,4f,25,ce,ab,3c");
}

TEST(FrameEncoderTests, AsciiVoicedFrame) {
    auto frameEncoder = FrameEncoder();
    frameEncoder.parseAsciiBitstream("0xc8,0x88,0x4f,0x25,0xce,0xab,0x3c");

    auto bin = frameEncoder.toHex();
    EXPECT_EQ(bin, "c8,88,4f,25,ce,ab,3c");
}

// An unvoiced Frame will produce 29 data bits. The output will also contain 4 stop bits.
TEST(FrameEncoderTests, UnvoicedFrame) {
    auto unVoicedFrame = Frame(38, false, 56.850773,
                             {-0.753234, 0.939525, -0.342255, -0.172317, 0.108887, 0.679660, 0.056874, 0.433271,
                              -0.220355, 0.17028});

    auto frameEncoder = FrameEncoder();
    frameEncoder.append(unVoicedFrame);

    auto bin = frameEncoder.toHex();
    EXPECT_EQ(bin, "08,88,4f,e5,01");
}

TEST(FrameEncoderTests, AsciiUnvoicedFrame) {
    auto frameEncoder = FrameEncoder();
    frameEncoder.parseAsciiBitstream("0x08,0x88,0x4f,0xe5,0x01");

    auto bin = frameEncoder.toHex();
    EXPECT_EQ(bin, "08,88,4f,e5,01");
}

TEST(FrameEncoderTests, MixtureOfFrames) {
    auto frames = std::vector<Frame>(
            {
                Frame(0, false, 0, {-0.753234, 0.939525, -0.342255, -0.172317, 0.108887, 0.679660, 0.056874, 0.433271,
                                    -0.220355, 0.17028}),

                Frame(38, true, 142.06, {-0.653234, 0.139525, 0.342255, -0.172317, 0.108887, 0.679660, 0.056874, 0.433271,
                                         -0.220355, 0.17028}),

                Frame(38, true, 142.06, {-0.653234, 0.139525, 0.342255, -0.172317, 0.108887, 0.679660, 0.056874, 0.433271,
                                         -0.220355, 0.17028}),

                Frame(38, false, 56.850773, {-0.753234, 0.939525, -0.342255, -0.172317, 0.108887, 0.679660, 0.056874, 0.433271,
                                             -0.220355, 0.17028})
            }
            );


    auto frameEncoder = FrameEncoder(frames);

    auto bin = frameEncoder.toHex();
    EXPECT_EQ(bin, "c0,8c,a4,5b,e2,bc,0a,33,92,6e,89,f3,2a,08,88,4f,e5,01");

}

TEST(FrameEncoderTests, AsciiMixtureOfFrames) {
    auto frameEncoder = FrameEncoder();
    frameEncoder.parseAsciiBitstream("0xc0,0x8c,0xa4,0x5b,0xe2,0xbc,0x0a,0x33,0x92,0x6e,0x89,0xf3,0x2a,0x08,0x88,0x4f,0xe5,0x01");

    auto bin = frameEncoder.toHex();
    EXPECT_EQ(bin, "c0,8c,a4,5b,e2,bc,0a,33,92,6e,89,f3,2a,08,88,4f,e5,01");
}

};  // namespace tms_express
