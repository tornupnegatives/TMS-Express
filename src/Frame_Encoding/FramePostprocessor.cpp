///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class: FramePostprocessor
//
// Description: After LPC analysis and Frame packing, postprocessing may improve the quality and realism of synthesized
//              speech. The FramePostprocessor facilitates such modifications.
//
// Author: Joseph Bellahcen <joeclb@icloud.com>
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Frame_Encoding/Frame.h"
#include "Frame_Encoding/FramePostprocessor.h"
#include "Frame_Encoding/Tms5220CodingTable.h"

#include <vector>

/// Create a new Frame Postprocessor
///
/// \param frames Frames to modify
/// \param maxVoicedGainDB Max audio gain for voiced (vowel) segments (in decibels)
/// \param maxUnvoicedGainDB Max audio gain for unvoiced (consonant) segments (in decibels)
FramePostprocessor::FramePostprocessor(std::vector<Frame> *frames, float maxVoicedGainDB, float maxUnvoicedGainDB) {
    originalFrameTable = std::vector<Frame>(frames->begin(), frames->end());
    frameTable = frames;
    maxUnvoicedGain = maxUnvoicedGainDB;
    maxVoicedGain = maxVoicedGainDB;
}
///////////////////////////////////////////////////////////////////////////////
//                          Getters & Setters
///////////////////////////////////////////////////////////////////////////////

float FramePostprocessor::getMaxUnvoicedGainDB() const {
    return maxUnvoicedGain;
}

void FramePostprocessor::setMaxUnvoicedGainDB(float gainDB) {
    maxUnvoicedGain = gainDB;
}

float FramePostprocessor::getMaxVoicedGainDB() const {
    return maxVoicedGain;
}

void FramePostprocessor::setMaxVoicedGainDB(float gainDB) {
    maxVoicedGain = gainDB;
}

///////////////////////////////////////////////////////////////////////////////
//                          Frame Table Manipulations
///////////////////////////////////////////////////////////////////////////////

/// Identify frames which are similar to their neighbor and mark them as repeated
///
/// \note   Because the human vocal tract changes rather slowly, consecutive encoded Frames may not always vary
///         significantly. In this case, the size of the bitstream can be reduced by marking certain Frames a repeats of
///         preceding ones and allowing the LPC synthesizer to reuse parameters
///
/// \return Number of frames converted to a repeat frame
int FramePostprocessor::detectRepeatFrames() {
    int nRepeatFrames = 0;

    for (int i = 1; i < frameTable->size(); i++) {
        Frame previousFrame = frameTable->at(i - 1);
        Frame &frame = frameTable->at(i);

        if (frame.isSilent() || previousFrame.isSilent()) {
            continue;
        }

        // TODO: Implement variety of repeat detection algorithms
        // The first reflector coefficient is useful in characterizing a Frame, and experimentally is a good indicator
        // of similarity between consecutive Frames
        int prevCoeff = previousFrame.quantizedCoeffs()[0];
        auto coeff = frame.quantizedCoeffs()[0];

        if (abs(coeff - prevCoeff) == 1) {
            frame.setRepeat(true);
            nRepeatFrames++;
        }
    }

    return nRepeatFrames;
}

/// Normalize Frame gain
///
/// \note Gain normalization gain help reduce DC offsets and improve perceived volume
void FramePostprocessor::normalizeGain() {
    normalizeGain(true);
    normalizeGain(false);
}

/// Normalize gain of either all voiced or all unvoiced frames
///
/// \param normalizeVoicedFrames Whether to operate on voiced or unvoiced frames
void FramePostprocessor::normalizeGain(bool normalizeVoicedFrames) {
    // Compute the max gain value for a Frame category
    float maxGain = 0.0f;
    for (const Frame &frame : *frameTable) {
        bool isVoiced = frame.isVoiced();
        float gain = frame.getGain();

        if (isVoiced == normalizeVoicedFrames && gain > maxGain) {
            maxGain = gain;
        }
    }

    // Apply scaling factor to improve naturalness of perceived volume
    float scale = (normalizeVoicedFrames ? maxVoicedGain : maxUnvoicedGain) / maxGain;
    for (Frame &frame : *frameTable) {
        bool isVoiced = frame.isVoiced();
        float gain = frame.getGain();

        if (isVoiced == normalizeVoicedFrames) {
            float scaledGain = gain * scale;
            frame.setGain(scaledGain);
        }
    }
}

/// Shift gain by an integer offset in the coding table
///
/// \note   Following LPC analysis, changing the gain of audio is as simple as selecting a new index of the energy
///         table. A ceiling is applied to the offset to prevent unstable bitstreams
void FramePostprocessor::shiftGain(int offset) {
    // If zero offset, do nothing
    if (!offset) {
        return;
    }

    for (Frame &frame : *frameTable) {
        int quantizedGain = frame.quantizedGain();
        int change = quantizedGain + offset;

        // If the shifted gain would exceed the maximum representable gain of the coding table, let it "hit the
        // ceiling." Overuse of the largest gain parameter may destabilize the synthesized signal
        if (change >= Tms5220CodingTable::rms.size()) {
            frame.setGain(*Tms5220CodingTable::rms.end());

        } else if (change < 0) {
            frame.setGain(0);

        } else {
            frame.setGain(Tms5220CodingTable::rms.at(offset));
        }
    }
}

/// Shift pitch by an integer offset in the coding table
void FramePostprocessor::shiftPitch(int offset) {
    if (!offset) {
        return;
    }

    for (Frame &frame : *frameTable) {
        int quantizedPitch = frame.quantizedPitch();
        int change = quantizedPitch + offset;

        // If the Frame is silent, do nothing
        if (frame.isSilent()) {
            continue;
        }

        // If the shifted gain would exceed the maximum representable gain of the coding table, let it "hit the
        // ceiling." Overuse of the largest gain parameter may destabilize the synthesized signal
        if (change >= Tms5220CodingTable::pitch.size()) {
            frame.setPitch(int(*Tms5220CodingTable::pitch.end()));

        } else if (change < 0) {
            frame.setPitch(0);

        } else {
            frame.setPitch(int(Tms5220CodingTable::pitch.at(change)));
        }
    }

}

/// Set the pitch of all non-silent frames to an index of the coding table
void FramePostprocessor::overridePitch(int index) {
    for (Frame &frame : *frameTable) {
        if (!frame.isSilent()) {
            if (index >= Tms5220CodingTable::pitch.size()) {
                frame.setPitch(int(*Tms5220CodingTable::pitch.end()));

            } else {
                frame.setPitch(int(Tms5220CodingTable::pitch.at(index)));
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
//                              Utility
///////////////////////////////////////////////////////////////////////////////

/// Restore frame table to its initialization state
///
/// \note This function will NOT reset voiced or unvoiced limits
void FramePostprocessor::reset() {
    for (int i = 0; i < frameTable->size(); i++) {
        auto &frame = frameTable->at(i);
        auto originalFrame = originalFrameTable.at(i);

        frame.setGain(originalFrame.getGain());
        frame.setPitch(originalFrame.getPitch());
    }
}