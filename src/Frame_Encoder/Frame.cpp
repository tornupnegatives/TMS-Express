//
// Created by Joseph Bellahcen on 4/18/22.
//

#include "Frame_Encoder/Frame.h"

#include <cstdlib>
#include <cstring>
#include <cstdio>

#include "Frame_Encoder/Tms5220CodingTable.h"

Frame::Frame(int order) {
    Frame::order = order;
    Frame::pitch = 0;
    Frame::voicing = 0;
    Frame::reflectorCoefficients = (float *) malloc(sizeof(float) * order);
    Frame::energy = 0.0f;
}

Frame::Frame(int order, int pitch, int voicing, float *coefficients, float energy) {
    Frame::order = order;
    Frame::pitch = pitch;
    Frame::voicing = voicing;
    Frame::energy = energy;

    // Make a copy of the reflector coefficients so that LPC analysis structures may be deallocated
    //
    // Because the first LPC coefficient is always zero, offset the pointer by one and ignore
    // the unused value
    Frame::reflectorCoefficients = (float *) malloc(sizeof(float) * order);
    memcpy(reflectorCoefficients, coefficients + 1, sizeof(float) * order);
}

Frame::~Frame() {
    if (reflectorCoefficients != nullptr) {
        free(reflectorCoefficients);
    }
}

void Frame::setPitch(int pitch) {
    Frame::pitch = pitch;
}

void Frame::setVoicing(int voicing) {
    Frame::voicing = voicing;
}

void Frame::setCoefficients(float *coefficients) {
    memcpy(reflectorCoefficients, coefficients + 1, sizeof(float) * order);
}

void Frame::setEnergy(float energy) {
    Frame::energy = energy;
}