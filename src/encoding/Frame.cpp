// Copyright (C) 2023 Joseph Bellahcen <joeclb@icloud.com>

#include "encoding/Frame.hpp"

#include <algorithm>
#include <cmath>
#include <vector>

#include "lib/json.hpp"

#include "encoding/CodingTable.hpp"

namespace tms_express {

///////////////////////////////////////////////////////////////////////////////
// Initializers ///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Frame::Frame(int pitch_period, bool is_voiced, float gain_db,
    std::vector<float> coeffs) {
    //
    gain_db_ = gain_db;
    pitch_period_ = pitch_period;
    coeffs_ = coeffs;
    is_repeat_ = false;
    is_voiced_ = is_voiced;

    // The gain may be NaN if the autocorrelation is zero, meaning:
    //  1. The Frame is completely silent (source audio is noise-isolated)
    //  2. The highpass filter cutoff is too low
    if (std::isnan(gain_db)) {
        gain_db_ = 0.0f;
        coeffs_.assign(coeffs_.size(), 0.0f);
    }
}

///////////////////////////////////////////////////////////////////////////////
// Accessors //////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

std::vector<float> Frame::getCoeffs() const {
    return coeffs_;
}

void Frame::setCoeffs(std::vector<float> coeffs) {
    coeffs_ = coeffs;
}

float Frame::getGain() const {
    return gain_db_;
}

void Frame::setGain(float gain_db) {
    gain_db_ = gain_db;
}

void Frame::setGain(int idx) {
    if (idx < 0) {
        gain_db_ = *coding_table::tms5220::rms.begin();

    } else if (idx > static_cast<int>(coding_table::tms5220::rms.size())) {
        gain_db_ = *coding_table::tms5220::rms.end();

    } else {
        gain_db_ = coding_table::tms5220::rms.at(idx);
    }
}

int Frame::getPitch() const {
    return pitch_period_;
}

void Frame::setPitch(int pitch) {
    pitch_period_ = pitch;
}

bool Frame::getRepeat() const {
    return is_repeat_;
}

void Frame::setRepeat(bool is_repeat) {
    is_repeat_ = is_repeat;
}

bool Frame::getVoicing() const {
    return is_voiced_;
}

void Frame::setVoicing(bool isVoiced) {
    is_voiced_ = isVoiced;
}

///////////////////////////////////////////////////////////////////////////////
// Quantized Getters //////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

std::vector<int> Frame::quantizedCoeffs() const {
    auto size = coding_table::tms5220::kNCoeffs;
    auto indices = std::vector<int>(size);

    // Only parse as many coefficients as the coding table supports
    for (int i = 0; i < size; i++) {
        auto table = coding_table::tms5220::getCoeffTable(i);
        float coeff = coeffs_[i];

        int idx = closestIndex(coeff, table);
        indices[i] = idx;
    }

    return indices;
}

int Frame::quantizedGain() const {
    auto table_array = coding_table::tms5220::rms;
    auto table_vector = std::vector<float>(table_array.begin(),
        table_array.end());

    int idx = closestIndex(gain_db_, table_vector);
    return idx;
}

/// Return pitch period indices, corresponding to coding table entries
int Frame::quantizedPitch() const {
    auto table = coding_table::tms5220::pitch;
    auto table_vector = std::vector<float>(table.begin(), table.end());

    int idx = closestIndex(pitch_period_, table_vector);
    return idx;
}

int Frame::quantizedVoicing() const {
    return is_voiced_;
}

///////////////////////////////////////////////////////////////////////////////
// Metadata ///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool Frame::isRepeat() const {
    return is_repeat_;
}

bool Frame::isSilent() const {
    return !quantizedGain();
}

bool Frame::isVoiced() const {
    return is_voiced_;
}

///////////////////////////////////////////////////////////////////////////////
// Serializers ////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

std::string Frame::toBinary() {
    // Reference: TMS 5220 VOICE SYNTHESIS PROCESSOR DATA MANUAL
    // http://sprow.co.uk/bbc/hardware/speech/tms5220.pdf

    std::string bin;

    // At minimum, a frame will contain an energy parameter
    int gain_idx = quantizedGain();
    bin += valueToBinary(gain_idx, coding_table::tms5220::kGainBitWidth);

    // A silent frame will contain no further parameters
    if (isSilent()) {
        return bin;
    }

    // A repeat frame contains energy, voicing, and pitch parameters
    bin += isRepeat() ? "1" : "0";

    // A voiced frame will have a non-zero pitch
    int pitch_idx = isVoiced() ? quantizedPitch() : 0;
    bin += valueToBinary(pitch_idx, coding_table::tms5220::kPitchBitWidth);

    if (isRepeat()) {
        return bin;
    }

    // Both voiced and unvoiced frames contain reflector coefficients, but vary
    // in quantity
    auto coeffs = quantizedCoeffs();
    int n_coeffs = isVoiced() ? 10 : 4;

    for (int i = 0; i < n_coeffs; i++) {
        int coeff = coeffs.at(i);
        auto coeff_width = coding_table::tms5220::kCoeffBitWidths[i];

        bin += valueToBinary(coeff, coeff_width);
    }

    return bin;
}

nlohmann::json Frame::toJSON() {
    nlohmann::json jFrame;

    // Raw values
    jFrame["pitch"] = pitch_period_;
    jFrame["isVoiced"] = is_voiced_;
    jFrame["isRepeat"] = is_repeat_;
    jFrame["gain"] = gain_db_;
    jFrame["coeffs"] = nlohmann::json(coeffs_);

    // Quantized values
    jFrame["tms_pitch"] = quantizedPitch();
    jFrame["tms_gain"] = quantizedGain();
    jFrame["tms_coeffs"] = nlohmann::json(quantizedCoeffs());

    return jFrame;
}

///////////////////////////////////////////////////////////////////////////////
// Static Helpers /////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

int Frame::closestIndex(float value, std::vector<float> table) {
    auto size = static_cast<int>(table.size());

    // First, check if the value is within the lower bound of the array values
    if (value <= table.at(0)) {
        return 0;
    }

    // Check elements to left and right to find where value best fits
    for (int i = 1; i < size; i++) {
        float right = table.at(i);
        float left = table.at(i - 1);

        if (value < right) {
            float right_distance = right - value;
            float left_distance = value - left;

            return (right_distance < left_distance) ? i : i - 1;
        }
    }

    return size - 1;
}

std::string Frame::valueToBinary(int value, int width) {
    std::string bin;

    for (int i = 0; i < width; i++) {
        bin += (value % 2 == 0) ? "0" : "1";
        value /= 2;
    }

    std::reverse(bin.begin(), bin.end());
    return bin;
}

};  // namespace tms_express
