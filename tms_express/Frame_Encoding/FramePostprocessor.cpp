// Copyright (C) 2023 Joseph Bellahcen <joeclb@icloud.com>

#include "Frame_Encoding/FramePostprocessor.hpp"

#include <vector>

#include "Frame_Encoding/CodingTable.hpp"
#include "Frame_Encoding/Frame.hpp"

namespace tms_express {

///////////////////////////////////////////////////////////////////////////////
// Initializers ///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FramePostprocessor::FramePostprocessor(std::vector<Frame> *frames,
    float max_voiced_gain_db, float max_unvoiced_gain_db) {
    //
    original_frame_table_ = std::vector<Frame>(frames->begin(), frames->end());
    frame_table_ = frames;
    max_unvoiced_gain_db_ = max_unvoiced_gain_db;
    max_voiced_gain_db_ = max_voiced_gain_db;
}

///////////////////////////////////////////////////////////////////////////////
// Accessors //////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

float FramePostprocessor::getMaxUnvoicedGainDB() const {
    return max_unvoiced_gain_db_;
}

void FramePostprocessor::setMaxUnvoicedGainDB(float gain_db) {
    max_unvoiced_gain_db_ = gain_db;
}

float FramePostprocessor::getMaxVoicedGainDB() const {
    return max_voiced_gain_db_;
}

void FramePostprocessor::setMaxVoicedGainDB(float gain_db) {
    max_voiced_gain_db_ = gain_db;
}

///////////////////////////////////////////////////////////////////////////////
// Frame Table Manipulators ///////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

int FramePostprocessor::detectRepeatFrames() {
    int n_repeat_frames = 0;

    for (int i = 1; i < static_cast<int>(frame_table_->size()); i++) {
        Frame previous_frame = frame_table_->at(i - 1);
        Frame &current_frame = frame_table_->at(i);

        if (current_frame.isSilent() || previous_frame.isSilent()) {
            continue;
        }

        // The first reflector coefficient is typically effective at
        // characterizing a Frame, and a useful indicator of similarity
        int previous_coeff = previous_frame.quantizedCoeffs()[0];
        auto current_coeff = current_frame.quantizedCoeffs()[0];

        if (abs(current_coeff - previous_coeff) == 1) {
            current_frame.setRepeat(true);
            n_repeat_frames++;
        }
    }

    return n_repeat_frames;
}

void FramePostprocessor::normalizeGain() {
    normalizeGain(true);
    normalizeGain(false);
}

void FramePostprocessor::shiftGain(int offset) {
    // If zero offset, do nothing
    if (!offset) {
        return;
    }

    for (Frame &frame : *frame_table_) {
        int quantized_gain = frame.quantizedGain();
        int change = quantized_gain + offset;

        // If the shifted gain would exceed the maximum representable gain of
        // the coding table, let it "hit the ceiling." Overuse of the largest
        // gain parameter may destabilize the synthesized signal
        if (change >= static_cast<int>(coding_table::tms5220::rms.size())) {
            frame.setGain(*coding_table::tms5220::rms.end());

        } else if (change < 0) {
            frame.setGain(0);

        } else {
            frame.setGain(coding_table::tms5220::rms.at(change));
        }
    }
}

void FramePostprocessor::shiftPitch(int offset) {
    if (!offset) {
        return;
    }

    for (Frame &frame : *frame_table_) {
        int quantized_pitch = frame.quantizedPitch();
        int change = quantized_pitch + offset;

        // If the Frame is silent, do nothing
        if (frame.isSilent()) {
            continue;
        }

        if (change >= static_cast<int>(coding_table::tms5220::pitch.size())) {
            frame.setPitch(*coding_table::tms5220::pitch.end());

        } else if (change < 0) {
            frame.setPitch(0);

        } else {
            frame.setPitch(coding_table::tms5220::pitch.at(change));
        }
    }
}

void FramePostprocessor::overridePitch(int index) {
    for (Frame &frame : *frame_table_) {
        if (!frame.isSilent()) {
            auto size = static_cast<int>(coding_table::tms5220::pitch.size());

            if (index >= size) {
                frame.setPitch(*coding_table::tms5220::pitch.end());

            } else {
                frame.setPitch(coding_table::tms5220::pitch.at(index));
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Utility ////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FramePostprocessor::reset() {
    for (int i = 0; i < static_cast<int>(frame_table_->size()); i++) {
        auto &frame = frame_table_->at(i);
        auto originalFrame = original_frame_table_.at(i);

        frame = originalFrame;
    }
}

///////////////////////////////////////////////////////////////////////////////
// Helpers ////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FramePostprocessor::normalizeGain(bool target_voiced) {
    // Compute the max gain value for a Frame category
    float max_gain = 0.0f;
    for (const Frame &frame : *frame_table_) {
        bool is_voiced = frame.isVoiced();
        float gain = frame.getGain();

        if (is_voiced == target_voiced && gain > max_gain) {
            max_gain = gain;
        }
    }

    // Apply scaling factor to improve naturalness of perceived volume
    float scale = (target_voiced ? max_voiced_gain_db_ : max_unvoiced_gain_db_);
    scale /=  max_gain;

    for (Frame &frame : *frame_table_) {
        bool is_voiced = frame.isVoiced();
        float gain = frame.getGain();

        if (is_voiced == target_voiced) {
            float scaled_gain = gain * scale;
            frame.setGain(scaled_gain);
        }
    }
}

};  // namespace tms_express
