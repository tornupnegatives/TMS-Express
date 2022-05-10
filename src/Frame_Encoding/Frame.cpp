//
// Created by Joseph Bellahcen on 4/18/22.
//

#include "Frame_Encoding/Frame.h"

#include <cstdlib>
#include <cstring>

#include "Frame_Encoding/Tms5220CodingTable.h"

using namespace Tms5220CodingTable;

Frame::Frame(int order) {
    Frame::order = order;
    Frame::pitch = 0;
    Frame::voicing = 0;
    Frame::reflectorCoefficients = (float *) malloc(sizeof(float) * order);
    Frame::gain = 0.0f;
}

Frame::Frame(int order, int pitch, int voicing, float *coefficients, float gain) {
    Frame::order = order;
    Frame::pitch = pitch;
    Frame::voicing = voicing;
    Frame::gain = gain;

    // Make a copy of the reflector coefficients so that LPC analysis structures may be deallocated
    //
    // Because the first LPC coefficient is always zero, offset the pointer by one and ignore
    // the unused value
    Frame::reflectorCoefficients = (float *) malloc(sizeof(float) * order);
    memcpy(Frame::reflectorCoefficients, coefficients + 1, sizeof(float) * order);
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

void Frame::setGain(float gain) {
    Frame::gain = gain;
}

float Frame::getGain() {
    return gain;
}

// The below getQuantized*() functions query the TMS5220 coding table to find
// the closest value to the given frame parameter, and return its index

int Frame::getQuantizedPitch() {
    const float *pitchTable = Tms5220CodingTable::pitch;
    const int pitchTableSize = Tms5220CodingTable::pitchSize;

    int pitchIdx = closestValueIndex(pitch, pitchTable, pitchTableSize);
    return pitchIdx;
}

unsigned char Frame::getQuantizedVoicing() {
    return (unsigned char) voicing;
}

int *Frame::getQuantizedCoefficients() {
    auto quantizedCoeff = (int *) malloc(sizeof(int) * order);
    const float **ks = Tms5220CodingTable::ks;
    const int *kSizes = Tms5220CodingTable::kSizes;

    for (int i = 0; i < order - 1; i++) {
        const float *kTableEntry = ks[i];
        const int kTableSize = kSizes[i];
        float k = reflectorCoefficients[i];

        int kIdx = closestValueIndex(k, kTableEntry, kTableSize);
        quantizedCoeff[i] = kIdx;
    }

    return quantizedCoeff;
}

int Frame::getQuantizedGain() {
    const float *rms = Tms5220CodingTable::rms;
    const int rmsSize = Tms5220CodingTable::rmsSize;

    int gainIdx = closestValueIndex(gain, rms, rmsSize);
    return gainIdx;
}

void Frame::setQuantizedGain(int gain) {
    Frame::gain = Tms5220CodingTable::rms[gain];
}

int Frame::closestValueIndex(float value, const float *codingTableEntry, int size) {
    // First, check if the value is within the lower bound of the array values
    if (value <= codingTableEntry[0]) {
        return 0;
    }

    // Check the elements to the left and right to find where the value best fits
    for (int i = 0; i < size; i++) {
        float rightEntry = codingTableEntry[i];
        float leftEntry = codingTableEntry[i - 1];

        if (value < rightEntry) {
            float rightDistance = rightEntry - value;
            float leftDistance = value - leftEntry;

            return (rightDistance < leftDistance) ? i : i - 1;
        }
    }

    return size - 1;
}
