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

#include "json.hpp"

#include <algorithm>
#include <cmath>
#include <vector>

namespace tms_express {

using namespace Tms5220CodingTable;

/// Create a new frame
///
/// \param pitchPeriod Pitch period (in samples)
/// \param isVoiced Whether frame corresponds to a consonant (unvoiced) or a vowel (voiced)
/// \param gainDB Frame gain (in decibels)
/// \param coeffs LPC reflector coefficients
Frame::Frame(int pitchPeriod, bool isVoiced, float gainDB, std::vector<float> coeffs) {
    gain = gainDB;
    pitch = pitchPeriod;
    reflectorCoeffs = std::move(coeffs);
    isRepeatFrame = false;
    isVoicedFrame = isVoiced;

    // The gain may be NaN if the autocorrelation is zero, which has been observed in the following scenarios:
    // 1. The Frame is completely silent (source audio is noise-isolated)
    // 2. The highpass filter cutoff is too low
    if (std::isnan(gainDB)) {
        gain = 0.0f;
        reflectorCoeffs.assign(reflectorCoeffs.size(), 0.0f);
    }
}

///////////////////////////////////////////////////////////////////////////////
//                          Getters & Setters
///////////////////////////////////////////////////////////////////////////////

/// Return LPC reflector coefficients
std::vector<float> Frame::getCoeffs() {
    return reflectorCoeffs;
}

/// Replace LPC reflector coefficients
///
/// \param coeffs New reflector coefficients
void Frame::setCoeffs(std::vector<float> coeffs) {
    reflectorCoeffs = std::move(coeffs);
}

/// Return gain (in decibels)
float Frame::getGain() const {
    return gain;
}

/// Set gain to exact value (in decibels)
///
/// \param gainDb New gain (in decibels)
void Frame::setGain(float gainDb) {
    gain = gainDb;
}

/// Set decibel gain based on coding table index
///
/// \note setGain(i) adjusts the gain to codingTable[i] dB
///
/// \param codingTableIdx
void Frame::setGain(int codingTableIdx) {
    gain = Tms5220CodingTable::rms.at(codingTableIdx);
}

/// Return pitch period (in samples)
int Frame::getPitch() const {
    return pitch;
}

/// Set pitch period (in samples)
///
/// \param pitchPeriod Pitch period (in samples)
void Frame::setPitch(int pitchPeriod) {
    pitch = pitchPeriod;
}

/// Return whether or not frame is identical to its neighbor
bool Frame::getRepeat() const {
    return isRepeatFrame;
}

/// Mark frame as identical to its neighbor
///
/// \note Marking a frame as repeat reduces the storage cost of the frame
///
/// \param isRepeat Whether or not frame is identical to its neighbor
void Frame::setRepeat(bool isRepeat) {
    isRepeatFrame = isRepeat;
}

/// Return whether frame corresponds to a consonant (unvoiced) or a vowel (voiced)
bool Frame::getVoicing() const {
    return isVoicedFrame;
}

/// Mark frame as voiced or unvoiced
///
/// \param isVoiced Whether frame corresponds to a consonant (unvoiced) or a vowel (voiced)
void Frame::setVoicing(bool isVoiced) {
    isVoicedFrame = isVoiced;
}

///////////////////////////////////////////////////////////////////////////////
//                             Const Getters
///////////////////////////////////////////////////////////////////////////////

/// Return LPC reflector coefficient indices, corresponding to coding table entries
std::vector<int> Frame::quantizedCoeffs() {
    auto size = nKCoeffs;
    auto coeffsIndices = std::vector<int>(size);

    // Only parse as many coefficients as the coding table supports
    for (int i = 0; i < size; i++) {
        auto kCoeffTable = kCoeffsSlice(i);
        float coeff = reflectorCoeffs[i];

        int idx = closestCodingTableIndexForValue(coeff, kCoeffTable);
        coeffsIndices[i] = idx;
    }

    return coeffsIndices;
}

/// Return frame gain index, corresponding to coding table entry
int Frame::quantizedGain() const {
    auto gainTable = Tms5220CodingTable::rms;
    auto gainVec = std::vector<float>(gainTable.begin(), gainTable.end());

    int idx = closestCodingTableIndexForValue(gain, gainVec);
    return idx;
}

/// Return pitch period indices, corresponding to coding table entries
int Frame::quantizedPitch() const {
    auto pitchTable = Tms5220CodingTable::pitch;
    auto pitchVec = std::vector<float>(pitchTable.begin(), pitchTable.end());

    int idx = closestCodingTableIndexForValue(float(pitch), pitchVec);
    return idx;
}

/// Return whether frame corresponds to a consonant (unvoiced) or a vowel (voiced)
///
/// \note Alias of \code Frame::getVoicing() \endcode
int Frame::quantizedVoicing() const {
    return int(isVoicedFrame);
}

///////////////////////////////////////////////////////////////////////////////
//                           Boolean Properties
///////////////////////////////////////////////////////////////////////////////

/// Return whether or not frame is identical to its neighbor
bool Frame::isRepeat() const {
    return isRepeatFrame;
}

/// Return whether or not frame is silent
bool Frame::isSilent() {
    return !quantizedGain();
}

/// whether frame corresponds to a consonant (unvoiced) or a vowel (voiced)
///
/// \note Alias of \code Frame::getQuantizedVoicing() \endcode
bool Frame::isVoiced() const {
    return isVoicedFrame;
}

///////////////////////////////////////////////////////////////////////////////
//                              Frame Serialization
///////////////////////////////////////////////////////////////////////////////

/// Return binary string representation of frame
std::string Frame::toBinary() {
    std::string bin;

    // At minimum, a frame will contain an energy parameter
    int energyIdx = quantizedGain();
    bin += valueToBinary(energyIdx, Tms5220CodingTable::gainWidth);

    // A silent frame will contain no further parameters
    if (isSilent()) {
        return bin;
    }

    // A repeat frame contains energy, voicing, and pitch parameters
    bin += isRepeat() ? "1" : "0";

    // A voiced frame will have a non-zero pitch
    int pitchIdx = isVoiced() ? quantizedPitch() : 0;
    bin += valueToBinary(pitchIdx, Tms5220CodingTable::pitchWidth);

    if (isRepeat()) {
        return bin;
    }

    // Both voiced and unvoiced frames contain reflector coefficients, but vary in quantity
    auto coeffs = quantizedCoeffs();
    int nCoeffs = isVoiced() ? 10 : 4;

    for (int i = 0; i < nCoeffs; i++) {
        int coeff = coeffs.at(i);
        int coeffWidth = Tms5220CodingTable::coeffWidths.at(i);

        bin += valueToBinary(coeff, coeffWidth);
    }

    return bin;
}

/// Return JSON representation of frame
nlohmann::json Frame::toJSON() {
    nlohmann::json jFrame;

    // Raw values
    jFrame["pitch"] = pitch;
    jFrame["isVoiced"] = isVoicedFrame;
    jFrame["isRepeat"] = isRepeatFrame;
    jFrame["gain"] = gain;
    jFrame["coeffs"] = nlohmann::json(reflectorCoeffs);

    // Quantized values
    jFrame["tms_pitch"] = quantizedPitch();
    jFrame["tms_gain"] = quantizedGain();
    jFrame["tms_coeffs"] = nlohmann::json(quantizedCoeffs());

    return jFrame;
}

///////////////////////////////////////////////////////////////////////////////
//                              Utility Functions
///////////////////////////////////////////////////////////////////////////////

/// Find index of coding table entry which most closely matches given value
///
/// \param value Value to quantize via coding table
/// \param codingTableRow Row (parameter vector) of coding table
/// \return Index of coding table row which is closest to given value
int Frame::closestCodingTableIndexForValue(float value, std::vector<float> codingTableRow) {
    auto size = int(codingTableRow.size());

    // First, check if the value is within the lower bound of the array values
    if (value <= codingTableRow.at(0)) {
        return 0;
    }

    // Check the elements to the left and right to find where the value best fits
    for (int i = 1; i < size; i++) {
        float rightEntry = codingTableRow.at(i);
        float leftEntry = codingTableRow.at(i - 1);

        if (value < rightEntry) {
            float rightDistance = rightEntry - value;
            float leftDistance = value - leftEntry;

            return (rightDistance < leftDistance) ? i : i - 1;
        }
    }

    return size - 1;
}

/// Convert integer to binary string of given width
///
/// \param value Value to convert to binary
/// \param bitWidth Width (number of bits) of binary string
/// \return Binary string with width-number of bits
std::string Frame::valueToBinary(int value, int bitWidth) {
    std::string bin;

    for (int i = 0; i < bitWidth; i++) {
        bin += (value % 2 == 0) ? "0" : "1";
        value /= 2;
    }

    std::reverse(bin.begin(), bin.end());

    return bin;
}

};  // namespace tms_express
