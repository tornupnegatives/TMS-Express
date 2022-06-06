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
#include <vector>

FramePostprocessor::FramePostprocessor(std::vector<Frame> *frames, float maxVoicedGainDB, float maxUnvoicedGainDB) {
    frameData = frames;
    maxVoicedGain = maxVoicedGainDB;
    maxUnvoicedGain = maxUnvoicedGainDB;
}

void FramePostprocessor::normalizeGain() {
    normalizeGain(true);
    normalizeGain(false);
}

void FramePostprocessor::normalizeGain(bool voiced) {
    // Compute the max gain value for a Frame category
    float maxGain = 0.0f;
    for (const Frame &frame : *frameData) {
        bool isVoiced = frame.isVoiced();
        float gain = frame.getGain();

        if (isVoiced == voiced && gain > maxGain) {
            maxGain = gain;
        }
    }

    // Apply scaling factor
    float scale = (voiced ? maxVoicedGain : maxUnvoicedGain) / maxGain;
    for (Frame &frame : *frameData) {
        bool isVoiced = frame.isVoiced();
        float gain = frame.getGain();

        if (isVoiced == voiced) {
            float scaledGain = gain * scale;
            frame.setGain(scaledGain);
        }
    }
}

void FramePostprocessor::shiftGain(int offset) {
    if (!offset) {
        return;
    }

    for (Frame &frame : *frameData) {
        int quantizedGain = frame.getQuantizedGainIdx();

        // Only modify non-silent frames
        if (quantizedGain > 0) {
            frame.setQuantizedGain(quantizedGain + offset);
        }
    }
}