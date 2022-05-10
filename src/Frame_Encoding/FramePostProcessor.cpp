//
// Created by Joseph Bellahcen on 5/8/22.
//

#include "Frame_Encoding/FramePostProcessor.h"
#include "LPC_Analysis/LowerVocalTractAnalyzer.h"

FramePostProcessor::FramePostProcessor(Frame **frames, int count, float maxVoicedGainDB, float maxUnvoicedGainDB) {
    FramePostProcessor::frames = frames;
    FramePostProcessor::count = count;
    FramePostProcessor::maxVoicedGain = maxVoicedGainDB;
    FramePostProcessor::maxUnvoicedGain = maxUnvoicedGainDB;
}

// Normalize gain of all frames
//
// Voiced and unvoiced frames are normalized independently
void FramePostProcessor::normalizeGain() {
    normalizeGain(VOICED);
    normalizeGain(UNVOICED);
}

// Normalize the gain across all frames of given voicing
//
// Gain normalization can produce more realistic synthesized speech
// by constraining audio levels to the usual range of human speech
void FramePostProcessor::normalizeGain(Voicing voicing) {
    // Compute max gain for frame category
    float maxGain = 0.0f;
    for (int i = 0; i < count; i++) {
        Frame *frame = frames[i];
        Voicing frameVoicing = frame->getQuantizedVoicing();
        float frameGain = frame->getGain();

        if (frameVoicing == voicing && frameGain > maxGain) {
            maxGain = frameGain;
        }
    }

    // Apply scaling factor based on frame voicing
    float scale = ((voicing) ? maxVoicedGain : maxUnvoicedGain) / maxGain;
    for (int i = 0; i < count; i++) {
        Frame *frame = frames[i];
        Voicing frameVoicing = frame->getQuantizedVoicing();
        float frameGain = frame->getGain();

        if (frameVoicing == voicing) {
            float scaledGain = frameGain * scale;
            frame->setGain(scaledGain);
        }
    }
}

void FramePostProcessor::shiftGain(int offset) {
    for (int i = 0; i < count; i++) {
        Frame *frame = frames[i];
        int frameQuantizedGain = frame->getQuantizedGain();

        if (frameQuantizedGain > 0) {
            frame->setQuantizedGain(frameQuantizedGain + offset);
        }
    }
}
