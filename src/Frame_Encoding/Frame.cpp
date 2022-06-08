///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class: Frame
//
// Description: A Frame is the smallest unit of speech that can be represented with linear predictive coding. It
//              typically corresponds to a 22.5-30 ms window of audio, and consists of different parameters. Different
//              kinds of frames contain different parameters. The parameters in a Frame do not actually represent
//              to numerical values, but are instead indices to a coding table that the synthesizer possesses
//
// Author: Joseph Bellahcen <joeclb@icloud.com>
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Frame_Encoding/Frame.h"
#include "Frame_Encoding/Tms5220CodingTable.h"
#include <algorithm>
#include <iostream>
#include <vector>

using namespace Tms5220CodingTable;
using std::cout;
using std::endl;

Frame::Frame(int pitchPeriod, bool isVoiced, float gainDB, std::vector<float> coeffs) {
    pitch = pitchPeriod;
    voicedFrame = isVoiced;
    gain = gainDB;
    reflectorCoeffs = std::move(coeffs);
}

///////////////////////////////////////////////////////////////////////////////
//                          Getters & Setters
///////////////////////////////////////////////////////////////////////////////

int Frame::getPitch() const {
    return pitch;
}

void Frame::setPitch(int pitchPeriod) {
    pitch = pitchPeriod;
}

bool Frame::getVoicing() const {
    return voicedFrame;
}

void Frame::setVoicing(bool isVoiced) {
    voicedFrame = isVoiced;
}

float Frame::getGain() const {
    return gain;
}

void Frame::setGain(float gainDb) {
    gain = gainDb;
}

std::vector<float> Frame::getCoeffs() {
    return reflectorCoeffs;
}

void Frame::setCoeffs(std::vector<float> coeffs) {
    reflectorCoeffs = std::move(coeffs);
}

///////////////////////////////////////////////////////////////////////////////
//                          Quantized Getters
//
//  These functions look for the given Frame parameter in a coding table
//  and return the closet index to describe the value
///////////////////////////////////////////////////////////////////////////////

int Frame::getQuantizedPitchIdx() const {
    auto pitchTable = Tms5220CodingTable::pitch;
    auto pitchVec = std::vector<float>(pitchTable.begin(), pitchTable.end());

    int idx = closestIndexFinder(float(pitch), pitchVec);
    return idx;
}

int Frame::getQuantizedVoicingIdx() const {
    return int(voicedFrame);
}

int Frame::getQuantizedGainIdx() const {
    auto gainTable = Tms5220CodingTable::rms;
    auto gainVec = std::vector<float>(gainTable.begin(), gainTable.end());

    int idx = closestIndexFinder(gain, gainVec);
    return idx;
}

std::vector<int> Frame::getQuantizedCoeffsIdx() {
    auto size = nKCoeffs;
    auto coeffsIndices = std::vector<int>(size);

    // Only parse as many coefficients as the coding table supports
    for (int i = 0; i < size; i++) {
        auto kCoeffTable = kCoeffsSlice(i);
        float coeff = reflectorCoeffs[i];

        int idx = closestIndexFinder(coeff, kCoeffTable);
        coeffsIndices[i] = idx;
    }

    return coeffsIndices;
}

///////////////////////////////////////////////////////////////////////////////
//                          Quantized Setters
///////////////////////////////////////////////////////////////////////////////

void Frame::setQuantizedGain(int gainIdx) {
    gain = Tms5220CodingTable::rms.at(gainIdx);
}

///////////////////////////////////////////////////////////////////////////////
//                              Frame Metadata
///////////////////////////////////////////////////////////////////////////////

bool Frame::isVoiced() const {
    return voicedFrame;
}

bool Frame::isSilent() const {
    return getQuantizedGainIdx() == 0;
}

// TODO: Implement repeat frames
bool Frame::isRepeat() {
    return false;
}

///////////////////////////////////////////////////////////////////////////////
//                              Frame Serialization
///////////////////////////////////////////////////////////////////////////////

void Frame::print(int index) {
    // Frame header
    cout << "Frame " << index << ":" << endl;

    // Frame metadata
    cout << "Pitch period (samples): " << pitch << endl;
    cout << "Voicing: " << (voicedFrame ? "voiced" : "unvoiced") << endl;
    cout << "Gain (dB): " << gain << endl;

    cout << "Coeffs: [";
    for (float coeff : reflectorCoeffs) {
        cout << coeff << " ";
    }

    // Remove trailing space and "close" array
    cout << "\b\b]" << endl << endl;
}

std::string Frame::toBinary() {
    std::string bin;

    // At minimum, a frame will contain an energy parameter
    int energyIdx = getQuantizedGainIdx();
    bin += valueToBinary(energyIdx, Tms5220CodingTable::gainWidth);

    // A silent frame will contain no further parameters
    if (isSilent()) {
        return bin;
    }

    // A repeat frame contains energy, voicing, and pitch parameters
    bin += isRepeat() ? "1" : "0";

    // A voiced frame will have a non-zero pitch
    int pitchIdx = isVoiced() ? getQuantizedPitchIdx() : 0;
    bin += valueToBinary(pitchIdx, Tms5220CodingTable::pitchWidth);

    if (isRepeat()) {
        return bin;
    }

    // Both voiced and unvoiced frames contain reflector coefficients, but vary in quantity
    auto coeffs = getQuantizedCoeffsIdx();
    int nCoeffs = isVoiced() ? 10 : 4;

    for (int i = 0; i < nCoeffs; i++) {
        int coeff = coeffs.at(i);
        int coeffWidth = Tms5220CodingTable::coeffWidths.at(i);

        bin += valueToBinary(coeff, coeffWidth);
    }

    return bin;
}

///////////////////////////////////////////////////////////////////////////////
//                              Utility Functions
///////////////////////////////////////////////////////////////////////////////

int Frame::closestIndexFinder(float value, std::vector<float> codingTableEntry) {
    auto size = int(codingTableEntry.size());

    // First, check if the value is within the lower bound of the array values
    if (value <= codingTableEntry.at(0)) {
        return 0;
    }

    // Check the elements to the left and right to find where the value best fits
    for (int i = 1; i < size; i++) {
        float rightEntry = codingTableEntry.at(i);
        float leftEntry = codingTableEntry.at(i - 1);

        if (value < rightEntry) {
            float rightDistance = rightEntry - value;
            float leftDistance = value - leftEntry;

            return (rightDistance < leftDistance) ? i : i - 1;
        }
    }

    return size - 1;
}

std::string Frame::valueToBinary(int value, int bitWidth) {
    std::string bin;

    for (int i = 0; i < bitWidth; i++) {
        bin += (value % 2 == 0) ? "0" : "1";
        value /= 2;
    }

    std::reverse(bin.begin(), bin.end());

    return bin;
}
