// Copyright (C) 2023 Joseph Bellahcen <joeclb@icloud.com>
// Reference: Arduino Talkie (https://github.com/going-digital/Talkie)
// Reference: Talkie.Love (https://github.com/tocisz/talkie.love)

#include "encoding/Synthesizer.hpp"

#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

#include "audio/AudioBuffer.hpp"
#include "encoding/CodingTable.hpp"
#include "encoding/Frame.hpp"

namespace tms_express {

///////////////////////////////////////////////////////////////////////////////
// Initializers ///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Synthesizer::Synthesizer(int sample_rate_hz, float frame_rate_ms) {
    sample_rate_hz_ = sample_rate_hz;
    window_width_ms_ = frame_rate_ms;
    n_samples_per_frame_ = sample_rate_hz * frame_rate_ms * 1e-3f;

    energy_ = period_ = 0;

    k1_ = k2_ = k3_ = k4_ =
    k5_ = k6_ = k7_ = k8_ =
    k9_ = k10_ = 0;

    x0_ = x1_ = x2_ = x3_ = x4_ = x5_ = x6_ = x7_ = x8_ = x9_ = u0_ = 0;
    rand_noise_ = period_count_ = 0;
    samples_ = {};
}

///////////////////////////////////////////////////////////////////////////////
// Synthesis Interfaces ///////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

std::vector<float> Synthesizer::synthesize(const std::vector<Frame>& frames) {
    reset();

    for (const auto &frame : frames) {
        if (updateSynthTable(frame)) {
            break;
        }

        for (int i = 0; i < n_samples_per_frame_; i++)
            samples_.push_back(updateLatticeFilter());
    }

    return samples_;
}

///////////////////////////////////////////////////////////////////////////////
// Accessors //////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

std::vector<float> Synthesizer::getSamples() const {
    return samples_;
}

///////////////////////////////////////////////////////////////////////////////
// Static Utilities ///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/// Export synthesized samples as an audio file
void Synthesizer::render(const std::vector<float> &samples,
    const std::string& path, int sample_rate_hz, float frame_rate_ms) {
    //
    AudioBuffer(samples, sample_rate_hz, frame_rate_ms).render(path);
}

///////////////////////////////////////////////////////////////////////////////
// Synthesis Functions ////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool Synthesizer::updateNoiseGenerator() {
    rand_noise_ = (rand_noise_ >> 1) ^ ((rand_noise_ & 1) ? 0xB800 : 0);
    return (rand_noise_ & 1);
}

bool Synthesizer::updateSynthTable(Frame frame) {
    auto quantized_gain = frame.quantizedGain();

    // Silent frame
    if (quantized_gain == 0) {
        energy_ = 0;

    // Stop frame
    } else if (quantized_gain == 0xf) {
        reset();
        return true;

    } else {
        energy_ = coding_table::tms5220::energy[quantized_gain];
        period_ = coding_table::tms5220::pitch[frame.quantizedPitch()];

        if (!frame.isRepeat()) {
            auto coeffs = frame.quantizedCoeffs();

            // Voiced/unvoiced parameters
            k1_ = coding_table::tms5220::k1[coeffs[0]];
            k2_ = coding_table::tms5220::k2[coeffs[1]];
            k3_ = coding_table::tms5220::k3[coeffs[2]];
            k4_ = coding_table::tms5220::k4[coeffs[3]];

            // Voiced-only parameters
            if (std::fpclassify(period_) != FP_ZERO) {
                k5_ = coding_table::tms5220::k5[coeffs[4]];
                k6_ = coding_table::tms5220::k6[coeffs[5]];
                k7_ = coding_table::tms5220::k7[coeffs[6]];
                k8_ = coding_table::tms5220::k8[coeffs[7]];
                k9_ = coding_table::tms5220::k9[coeffs[8]];
                k10_ = coding_table::tms5220::k10[coeffs[9]];
            }
        }
    }

    return false;
}

float Synthesizer::updateLatticeFilter() {
    // Generate voiced sample
    if (std::fpclassify(period_) != FP_ZERO) {
        if (static_cast<float>(period_count_) < period_) {
            period_count_++;
        } else {
            period_count_ = 0;
        }

        auto size = static_cast<int>(coding_table::tms5220::chirp.size());

        if (period_count_ < size) {
            u0_ = ((coding_table::tms5220::chirp[period_count_]) * energy_);
        } else {
            u0_ = 0;
        }

    // Generate unvoiced sample
    } else {
        u0_ = (updateNoiseGenerator()) ? energy_ : -energy_;
    }

    // Push new data through lattice filter
    if (std::fpclassify(period_) != FP_ZERO) {
        u0_ -= (k10_ * x9_) + (k9_ * x8_);
        x9_ = x8_ + (k9_ * u0_);

        u0_ -= k8_ * x7_;
        x8_ = x7_ + (k8_ * u0_);

        u0_ -= k7_ * x6_;
        x7_ = x6_ + (k7_ * u0_);

        u0_ -= k6_ * x5_;
        x6_ = x5_ + (k6_ * u0_);

        u0_ -= k5_ * x4_;
        x5_ = x4_ + (k5_ * u0_);
    }

    u0_ -= k4_ * x3_;
    x4_ = x3_ + (k4_ * u0_);

    u0_ -= k3_ * x2_;
    x3_ = x2_ + (k3_ * u0_);

    u0_ -= k2_ * x1_;
    x2_ = x1_ + (k2_ * u0_);

    u0_ -= k1_ * x0_;
    x1_ = x0_ + (k1_ * u0_);

    // Normalize result
    x0_ = std::max(std::min(u0_, 1.0f), -1.0f);
    return x0_;
}

///////////////////////////////////////////////////////////////////////////
// Utility Functions //////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

void Synthesizer::reset() {
    energy_ = period_ = 0;
    k1_ = k2_ = k3_ = k4_ = k5_ = k6_ = k7_ = k8_ = k9_ = k10_ = 0;
    x0_ = x1_ = x2_ = x3_ = x4_ = x5_ = x6_ = x7_ = x8_ = x9_ = u0_ = 0;
    rand_noise_ = period_count_ = 0;
    samples_ = {};
}

};  // namespace tms_express
