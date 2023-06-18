///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class: Synthesizer
//
// Description: The Synthesizer converts a Frame vector into PCM samples representing synthesized speech. The lattice
//              filter through which Frame parameters pass is modeled after the TMS5220 Voice Synthesis Processor
//
// Author: Joseph Bellahcen <joeclb@icloud.com>
//
// Acknowledgement: This class implements a pure-software version of the popular Arduino Talkie library, written by
//                  Peter Knight and Jonathan Gevaryahu, which emulates the behavior of the TMS5220. It also draws from
//                  a Lua implementation of the original C++ codebase, which utilized a floating-point coding table.
//                  The original source codes may be found at https://github.com/going-digital/Talkie and
//                  https://github.com/tocisz/talkie.love
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Audio/AudioBuffer.h"
#include "Frame_Encoding/Synthesizer.h"
#include "Frame_Encoding/Frame.h"
#include "Frame_Encoding/Tms5220CodingTable.h"

#include <cmath>
#include <string>
#include <vector>

using namespace Tms5220CodingTable;

/// Create a new synthesizer
///
/// \note   The synthesizer consists of three parts: the synthesis table, the random noise generator, and the lattice
///         filter. The synthesis table holds parameters of the current frame. These parameters pass through the lattice
///         filter repeatedly to produce audio. The random noise generator provides the basis for unvoiced sounds
///
/// \param sampleRateHz Sample rate of synthesized audio (in Hertz)
/// \param frameRateMs Frame rate of synthesized audio, or duration of each frame (in milliseconds)
Synthesizer::Synthesizer(int sampleRateHz, float frameRateMs) {
    sampleRate = sampleRateHz;
    windowWidth = frameRateMs;
    samplesPerFrame = int(float(sampleRateHz) * frameRateMs * 1e-3f);

    synthEnergy = synthPeriod = 0;
    synthK1 = synthK2 = synthK3 = synthK4 = synthK5 = synthK6 = synthK7 = synthK8 = synthK9 = synthK10 = 0;
    x0 = x1 = x2 = x3 = x4 = x5 = x6 = x7 = x8 = x9 = u0 = 0;
    synthRand = periodCounter = 0;
    synthesizedSamples = {};
}

/// Reset the synthesizer
///
/// \note Synthesizer::synthesize calls this automatically
void Synthesizer::reset() {
    synthEnergy = synthPeriod = 0;
    synthK1 = synthK2 = synthK3 = synthK4 = synthK5 = synthK6 = synthK7 = synthK8 = synthK9 = synthK10 = 0;
    x0 = x1 = x2 = x3 = x4 = x5 = x6 = x7 = x8 = x9 = u0 = 0;
    synthRand = periodCounter = 0;
    synthesizedSamples = {};
}

/// Reconstruct audio from frame data
///
/// \param frames Frames to synthesize
/// \return Synthesized PCM samples
std::vector<float> Synthesizer::synthesize(const std::vector<Frame>& frames) {
    reset();

    for (const auto &frame : frames) {
        if (updateSynthTable(frame)) {
            break;
        }

        for (int i = 0; i < samplesPerFrame; i++)
            synthesizedSamples.push_back(updateLatticeFilter());
    }

    return synthesizedSamples;
}

/// Export synthesized samples as an audio file
void Synthesizer::render(const std::string &path) {
    AudioBuffer(synthesizedSamples, sampleRate, windowWidth).render(path);
}

/// Update the synthesizer state based on incoming frame
///
/// \param frame Frame to load into synthesis table
/// \return Whether stop frame encountered, at which point synthesis should halt
bool Synthesizer::updateSynthTable(Frame frame) {
    auto quantizedGain = frame.quantizedGain();

    // Silent frame
    if (quantizedGain == 0) {
        synthEnergy = 0;

        // Stop frame
    } else if (quantizedGain == 0xf) {
        reset();
        return true;

    } else {
        synthEnergy = energy[quantizedGain];
        synthPeriod = pitch[frame.quantizedPitch()];

        if (!frame.isRepeat()) {
            auto coeffs = frame.quantizedCoeffs();

            // Voiced/unvoiced parameters
            synthK1 = k1[coeffs[0]];
            synthK2 = k2[coeffs[1]];
            synthK3 = k3[coeffs[2]];
            synthK4 = k4[coeffs[3]];

            // Voiced-only parameters
            if (std::fpclassify(synthPeriod) != FP_ZERO) {
                synthK5 = k5[coeffs[4]];
                synthK6 = k6[coeffs[5]];
                synthK7 = k7[coeffs[6]];
                synthK8 = k8[coeffs[7]];
                synthK9 = k9[coeffs[8]];
                synthK10 = k9[coeffs[9]];
            }
        }
    }

    return false;
}

/// Advance the random noise generator
///
/// \return The polarity (+true, -false) of the unvoiced sample to generate
bool Synthesizer::updateNoiseGenerator() {
    synthRand = (synthRand >> 1) ^ ((synthRand & 1) ? 0xB800 : 0);
    return (synthRand & 1);
}

/// Synthesize new sample and advance synthesizer state
///
/// \return Newly synthesized sample
float Synthesizer::updateLatticeFilter() {
    // Generate voiced sample
    if (std::fpclassify(synthPeriod) != FP_ZERO) {
        if (float(periodCounter) < synthPeriod) {
            periodCounter++;
        } else {
            periodCounter = 0;
        }

        if (periodCounter < chirpWidth) {
            u0 = ((chirp[periodCounter]) * synthEnergy);
        } else {
            u0 = 0;
        }

    // Generate unvoiced sample
    } else {
        u0 = (updateNoiseGenerator()) ? synthEnergy : -synthEnergy;
    }

    // Push new data through lattice filter
    if (std::fpclassify(synthPeriod) != FP_ZERO) {
        u0 -= (synthK10 * x9) + (synthK9 * x8);
        x9 = x8 + (synthK9 * u0);

        u0 -= synthK8 * x7;
        x8 = x7 + (synthK8 * u0);

        u0 -= synthK7 * x6;
        x7 = x6 + (synthK7 * u0);

        u0 -= synthK6 * x5;
        x6 = x5 + (synthK6 * u0);

        u0 -= synthK5 * x4;
        x5 = x4 + (synthK5 * u0);
    }

    u0 -= synthK4 * x3;
    x4 = x3 + (synthK4 * u0);

    u0 -= synthK3 * x2;
    x3 = x2 + (synthK3 * u0);

    u0 -= synthK2 * x1;
    x2 = x1 + (synthK2 * u0);

    u0 -= synthK1 * x0;
    x1 = x0 + (synthK1 * u0);

    // Normalize result
    x0 = std::max(std::min(u0, 1.0f), -1.0f);
    return x0;
}

std::vector<float> Synthesizer::samples() {
    return synthesizedSamples;
}
