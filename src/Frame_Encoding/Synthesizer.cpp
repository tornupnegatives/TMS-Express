///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class: Synthesizer
//
// Description: The Synthesizer converts a Frame vector into PCM samples representing synthesized speech. It does so
//              by applying a reverse lattice filter with the Frame's reflector coefficients.
//
// Author: Joseph Bellahcen <joeclb@icloud.com>
//
// Acknowledgement: This code is adapted from the popular Arduino Talkie library, written by Peter Knight and
//                  Jonathan Gevaryahu. The original source code can be found at https://github.com/going-digital/Talkie
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Frame_Encoding/Synthesizer.h"
#include "Frame_Encoding/Frame.h"

#include <sndfile.hh>

#include <cstdint>
#include <string>
#include <vector>

#include <iostream>

Synthesizer::Synthesizer() {
    synthPeriod = 0;
    synthEnergy = 0;
    synthK1 = synthK2 = 0;
    synthK3 = synthK4 = synthK5 = synthK6 = synthK7 = synthK8 = synthK9 = synthK10 = 0;
    pcmSamples = {};
}

void Synthesizer::reset() {
    pcmSamples = {};

    synthPeriod = 0;
    synthEnergy = 0;
    synthK1 = synthK2 = 0;
    synthK3 = synthK4 = synthK5 = synthK6 = synthK7 = synthK8 = synthK9 = synthK10 = 0;
}

void Synthesizer::synthesize(const std::vector<Frame>& frames) {
    reset();

    for (auto frame : frames) {
        readFrame(frame);

        for (int i = 0; i < 8000 / 40; i++)
            pcmSamples.push_back(generateSample());
    }
}

void Synthesizer::render(std::string path) {
    std::cout << "Rendering " << pcmSamples.size() << " pcmSamples" << std::endl;

    auto sndfileHandle = SndfileHandle(path, SFM_WRITE, SF_FORMAT_WAV | SF_FORMAT_PCM_16, 1, 8000);
    sndfileHandle.write(pcmSamples.data(), pcmSamples.size());
}

void Synthesizer::readFrame(Frame frame) {
    auto energy = frame.quantizedGain();

    // Silent frame
    if (frame.isSilent()) {
        //std::cout << "SILENT" << std::endl;
        synthEnergy = 0;

        // Stop frame
    } else if (energy == 0xf) {
        //std::cout << "DONE" << std::endl;
        synthEnergy = 0;
        synthK1 = 0;
        synthK2 = 0;
        synthK3 = 0;
        synthK4 = 0;
        synthK5 = 0;
        synthK6 = 0;
        synthK7 = 0;
        synthK8 = 0;
        synthK9 = 0;
        synthK10 = 0;

    } else {
        synthEnergy = tmsEnergy[energy];
        synthPeriod = tmsPeriod[frame.quantizedPitch()];

        if (!frame.isRepeat()) {
            auto coeffs = frame.quantizedCoeffs();

            synthK1 = tmsK1[coeffs[0]];
            synthK2 = tmsK2[coeffs[1]];
            synthK3 = tmsK3[coeffs[2]];
            synthK4 = tmsK4[coeffs[3]];

            if (synthPeriod) {
                synthK5 = tmsK5[coeffs[4]];
                synthK6 = tmsK6[coeffs[5]];
                synthK7 = tmsK7[coeffs[6]];
                synthK8 = tmsK8[coeffs[7]];
                synthK9 = tmsK9[coeffs[8]];
                synthK10 = tmsK10[coeffs[9]];
            }
        }
    }
}

unsigned short Synthesizer::generateSample() {
    static uint8_t periodCounter;
    static int16_t x0,x1,x2,x3,x4,x5,x6,x7,x8,x9;
    int16_t u0,u1,u2,u3,u4,u5,u6,u7,u8,u9,u10;

    if (synthPeriod) {
        // Voiced source
        if (periodCounter < synthPeriod) {
            periodCounter++;
        } else {
            periodCounter = 0;
        }
        if (periodCounter < sizeof(chirp)) {
            u10 = ((chirp[periodCounter]) * (uint32_t) synthEnergy) >> 8;
        } else {
            u10 = 0;
        }
    } else {
        // Unvoiced source
        static uint16_t synthRand = 1;
        synthRand = (synthRand >> 1) ^ ((synthRand & 1) ? 0xB800 : 0);
        u10 = (synthRand & 1) ? synthEnergy : -synthEnergy;
    }
    // Lattice filter forward path
    u9 = u10 - (((int16_t)synthK10*x9) >> 7);
    u8 = u9 - (((int16_t)synthK9*x8) >> 7);
    u7 = u8 - (((int16_t)synthK8*x7) >> 7);
    u6 = u7 - (((int16_t)synthK7*x6) >> 7);
    u5 = u6 - (((int16_t)synthK6*x5) >> 7);
    u4 = u5 - (((int16_t)synthK5*x4) >> 7);
    u3 = u4 - (((int16_t)synthK4*x3) >> 7);
    u2 = u3 - (((int16_t)synthK3*x2) >> 7);
    u1 = u2 - (((int32_t)synthK2*x1) >> 15);
    u0 = u1 - (((int32_t)synthK1*x0) >> 15);

    // Output clamp
    if (u0 > 511) u0 = 511;
    if (u0 < -512) u0 = -512;

    // Lattice filter reverse path
    x9 = x8 + (((int16_t)synthK9*u8) >> 7);
    x8 = x7 + (((int16_t)synthK8*u7) >> 7);
    x7 = x6 + (((int16_t)synthK7*u6) >> 7);
    x6 = x5 + (((int16_t)synthK6*u5) >> 7);
    x5 = x4 + (((int16_t)synthK5*u4) >> 7);
    x4 = x3 + (((int16_t)synthK4*u3) >> 7);
    x3 = x2 + (((int16_t)synthK3*u2) >> 7);
    x2 = x1 + (((int32_t)synthK2*u1) >> 15);
    x1 = x0 + (((int32_t)synthK1*u0) >> 15);
    x0 = u0;

    return (u0 << 6);
}

std::vector<float> Synthesizer::normalizedSamples() {
    auto normalized = std::vector<float>(pcmSamples.size());

    for (int i = 0; i < pcmSamples.size(); i++) {
        normalized[i] = float(pcmSamples[i]) / INT16_MAX;
    }

    return normalized;
}
