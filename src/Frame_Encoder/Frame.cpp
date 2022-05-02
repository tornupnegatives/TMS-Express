//
// Created by Joseph Bellahcen on 4/18/22.
//

#include "Frame_Encoder/Frame.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>

#include "Frame_Encoder/Tms5220CodingTable.h"

using namespace Tms5220CodingTable;

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

void Frame::setEnergy(float energy) {
    Frame::energy = energy;
}

// The below getQuantized*() functions query the TMS5220 coding table to find
// the closest value to the given frame parameter, and return its index

int Frame::getQuantizedPitch() {
    const int *pitchTable = Tms5220CodingTable::pitch;
    const int pitchTableSize = Tms5220CodingTable::pitchSize;

    int pitchIdx = closestValueIndexFinderInt(pitch, pitchTable, pitchTableSize);
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

        int kIdx = closestValueIndexFinderFloat(k, kTableEntry, kTableSize);
        quantizedCoeff[i] = kIdx;
    }

    return quantizedCoeff;
}

int Frame::getQuantizedEnergy() {
    //int wholeEnergy = (int) (energy * 5000.0f);

    const int *rms = Tms5220CodingTable::rms;
    const int rmsSize = Tms5220CodingTable::rmsSize;

    int energyIdx = closestValueIndexFinderInt(energy, rms, rmsSize);
    return  energyIdx;
}

int Frame::closestValueIndexFinderInt(int value, const int *codingTableEntry, int size) {
    // Get first element in coding table which is NOT less than the given value
    const int *offset = std::lower_bound(codingTableEntry, codingTableEntry + size, value);
    int distance = offset - codingTableEntry;
    return distance;
}

int Frame::closestValueIndexFinderFloat(float value, const float *codingTableEntry, int size) {
    // Get first element in coding table which is NOT less than the given value
    const float *offset = std::lower_bound(codingTableEntry, codingTableEntry + size, value);
    int distance = offset - codingTableEntry;
    return distance;
}
