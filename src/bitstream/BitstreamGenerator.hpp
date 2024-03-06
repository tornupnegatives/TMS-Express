// Copyright (C) 2023 Joseph Bellahcen <joeclb@icloud.com>

#ifndef TMS_EXPRESS_SRC_BITSTREAM_BITSTREAMGENERATOR_HPP_
#define TMS_EXPRESS_SRC_BITSTREAM_BITSTREAMGENERATOR_HPP_

#include <string>
#include <tuple>
#include <vector>

#include "bitstream/BitstreamGeneratorParameters.hpp"
#include "encoding/Frame.hpp"

namespace tms_express {

/// @brief Facilitates IO, preprocessing, analysis, and encoding
class BitstreamGenerator {
 public:
    ///////////////////////////////////////////////////////////////////////////
    // Initializers ///////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    explicit BitstreamGenerator(std::string input, SharedParameters params);

    ///////////////////////////////////////////////////////////////////////////
    // Analysis ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Estimates pitch period of segments in sample
    /// @param params Lower vocal tract analysis parameters
    /// @return Pitch table, with one pitch period estimate (in samples) per
    ///         segment in input audio buffer
    std::vector<int> analyzeLowerTract(LowerVocalTractParameters params);

    /// @brief Estimates LPC coefficients and gain of segments in sample
    /// @param params Upper vocal tract analysis parameters
    /// @return Tuple of coefficients table and gain table. The coefficients
    ///         table contains one row for each segment in the sample. Each
    ///         column corresponds to a different LPC reflector coefficient.
    ///         The gain table is a single row, with one column for each
    ///         segment in the sample.Each row corresponds to the predicted
    ///         gain for the segment, in Decibels.
    std::tuple<std::vector<std::vector<float>>, std::vector<float>>
    analyzeUpperTract(UpperVocalTractParameters params);

    /// @brief Categorizes each segment as voiced or unvoiced
    /// @param coeffs LPC reflector coefficients
    /// @return Voicing table, with one voicing estimate per sample. A voicing
    ///         estimate of `true` corresponds to a voiced sample (vowel sound),
    ///         while an estimate of `false` corresponds to an unvoiced sample
    ///         (consonant sound)
    std::vector<bool> estimateVoicing(
        const std::vector<std::vector<float>>& coeffs);

    ///////////////////////////////////////////////////////////////////////////
    // Encoding ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Post-processes frame table to apply analysis-independent edits
    /// @param frame_table Vector of Frame objects representing input audio
    void applyPostProcessing(const std::vector<Frame>& frame_table);

    /// @brief Converts frame table to bitstream
    /// @param frame_table Vector of Frame objects representing input audio
    /// @param params Bitstream parameters
    /// @return Serialized frame table, as a bitstream string
    std::string serializeFrames(const std::vector<Frame>& frame_table,
                                BitstreamParameters params);
};

};  // namespace tms_express

#endif  // TMS_EXPRESS_SRC_BITSTREAM_BITSTREAMGENERATOR_HPP_
