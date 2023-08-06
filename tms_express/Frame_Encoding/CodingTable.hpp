// Copyright (C) 2023 Joseph Bellahcen <joeclb@icloud.com>
// Reference: TMS 5220 VOICE SYNTHESIS PROCESSOR DATA MANUAL
//              (http://sprow.co.uk/bbc/hardware/speech/tms5220.pdf)
// Reference: Arduino Talkie (https://github.com/going-digital/Talkie)

#ifndef TMS_EXPRESS_FRAME_ENCODING_CODINGTABLE_HPP_
#define TMS_EXPRESS_FRAME_ENCODING_CODINGTABLE_HPP_

#include <array>
#include <vector>

namespace tms_express::coding_table::tms5220 {

///////////////////////////////////////////////////////////////////////////////
// Metadata //////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

static const int kNCoeffs = 10;

static const int kGainBitWidth = 4;
static const int kPitchBitWidth = 6;
static const int kVoicingBitWidth = 1;
static const int kCoeffBitWidths[] = {5, 5, 4, 4, 4, 4, 4, 3, 3, 3};

///////////////////////////////////////////////////////////////////////////////
// RMS (Gain) Table ///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

static const std::array<float, 16> rms = {0, 52, 87, 123, 174, 246, 348, 491,
    694, 981, 1385, 1957, 2764, 3904, 5514, 7789};

///////////////////////////////////////////////////////////////////////////////
// Pitch Period Table /////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

static const std::array<float, 64> pitch = {0, 15, 16, 17, 18, 19, 20, 21, 22,
    23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41,
    42, 44, 46, 48, 50, 52, 53, 56, 58, 60, 62, 65, 68, 70, 72, 76, 78, 80, 84,
    86, 91, 94, 98, 101, 105, 109, 114, 118, 122, 127, 132, 137, 142, 148, 153,
    159};

///////////////////////////////////////////////////////////////////////////////
// LPC Reflector Coefficients /////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

static const std::array<float, 32> k1 = {-0.97850f, -0.97270f, -0.97070f,
    -0.96680f, -0.96290f, -0.95900f, -0.95310f, -0.94140f, -0.93360f, -0.92580f,
    -0.91600f, -0.90620f, -0.89650f, -0.88280f, -0.86910f, -0.85350f, -0.80420f,
    -0.74058f, -0.66019f, -0.56116f, -0.44296f, -0.30706f, -0.15735f, -0.00005f,
    0.15725f, 0.30696f, 0.44288f, 0.56109f, 0.66013f, 0.74054f, 0.80416f,
    0.85350f};

static const std::array<float, 32> k2 = {-0.64000f, -0.58999f, -0.53500f,
    -0.47507f, -0.41039f, -0.34129f, -0.26830f, -0.19209f, -0.11350f, -0.03345f,
    0.04702f, 0.12690f, 0.20515f, 0.28087f, 0.35325f, 0.42163f, 0.48553f,
    0.54464f, 0.59878f, 0.64796f, 0.69227f, 0.73190f, 0.76714f, 0.79828f,
    0.82567f, 0.84965f, 0.87057f, 0.88875f, 0.90451f, 0.91813f, 0.92988f,
    0.98830f};

static const std::array<float, 16> k3 = {-0.86000f, -0.75467f, -0.64933f,
    -0.54400f, -0.43867f, -0.33333f, -0.22800f, -0.12267f, -0.01733, 0.08800f,
    0.19333f, 0.29867f, 0.40400f, 0.50933f, 0.61467f, 0.72000f};

static const std::array<float, 16> k4 = {-0.64000f, -0.53145f, -0.42289f,
    -0.31434f, -0.20579f, -0.09723f, 0.01132f, 0.11987f, 0.22843f, 0.33698f,
    0.44553f, 0.55409f, 0.66264f, 0.77119f, 0.87975f, 0.98830f};

static const std::array<float, 16> k5 = {-0.64000f, -0.54933f, -0.45867f,
    -0.36800f, -0.27733f, -0.18667f, -0.09600f, -0.00533f, 0.08533f, 0.17600f,
    0.26667f, 0.35733f, 0.44800f, 0.53867f, 0.62933f, 0.72000f};

static const std::array<float, 16> k6 = {-0.50000f, -0.41333f, -0.32667f,
    -0.24000f, -0.15333f, -0.06667f, 0.02000f, 0.10667f, 0.19333f, 0.28000f,
    0.36667f, 0.45333f, 0.54000f, 0.62667f, 0.71333f, 0.80000f};

static const std::array<float, 16> k7 = {-0.60000f, -0.50667f, -0.41333f,
    -0.32000f, -0.22667f, -0.13333f, -0.04000f, 0.05333f, 0.14667f, 0.24000f,
    0.33333f, 0.42667f, 0.52000f, 0.61333f, 0.70667f, 0.80000f};

static const std::array<float, 8> k8 = {-0.50000f, -0.31429f, -0.12857f,
    0.05714f, 0.24286f, 0.42857f, 0.61429f, 0.80000f};

static const std::array<float, 8> k9 = {-0.50000f, -0.34286f, -0.18571f,
    0.02857f, 0.12857f, 0.28571f, 0.44286f, 0.60000f};

static const std::array<float, 8> k10 = {-0.40000f, -0.25714f, -0.11429f,
    0.02857f, 0.17143f, 0.31429f, 0.45714f, 0.60000f};

///////////////////////////////////////////////////////////////////////////////
// Synthesis Tables ///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

static const std::array<float, 41> chirp = {0, 0.328125, -0.34375, 0.390625,
    -0.609375, 0.140625, 0.2890625, 0.15625, 0.015625, -0.2421875, -0.4609375,
    0.015625, 0.7421875, 0.703125, 0.0390625, 0.1171875, 0.296875, -0.03125,
    -0.7109375, -0.7109375, -0.328125, -0.2734375, -0.28125, -0.03125,
    0.2890625, 0.3359375, 0.265625, 0.2578125, 0.1171875, -0.0078125, -0.0625,
    -0.140625, -0.1484375, -0.1328125, -0.0703125, -0.078125, -0.046875, 0,
    0.0234375, 0.015625, 0.0078125};

static const std::array<float, 16> energy = {0, 0.00390625, 0.005859375,
    0.0078125, 0.009765625, 0.013671875, 0.01953125, 0.029296875, 0.0390625,
    0.0625, 0.080078125, 0.111328125, 0.158203125, 0.22265625, 0.314453125, 0};

///////////////////////////////////////////////////////////////////////////////
// LPC Coefficient Table Getter ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/// @brief Gets the ith LPC reflector coefficient table
/// @param i Index of coefficient table
/// @return ith coefficient table if i in range, empty vector otherwise
static const std::vector<float> getCoeffTable(int i) {
    switch (i) {
        case 0: return {k1.begin(), k1.end()};
        case 1: return {k2.begin(), k2.end()};
        case 2: return {k3.begin(), k3.end()};
        case 3: return {k4.begin(), k4.end()};
        case 4: return {k5.begin(), k5.end()};
        case 5: return {k6.begin(), k6.end()};
        case 6: return {k7.begin(), k7.end()};
        case 7: return {k8.begin(), k8.end()};
        case 8: return {k9.begin(), k9.end()};
        case 9: return {k10.begin(), k10.end()};
        default: return {};
    }
}

};  // namespace tms_express::coding_table::tms5220

#endif  // TMS_EXPRESS_FRAME_ENCODING_CODINGTABLE_HPP_