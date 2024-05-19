// Copyright 2023-2024 Joseph Bellahcen <joeclb@icloud.com>

#ifndef TMS_EXPRESS_SRC_UI_APPLICATION_HPP_
#define TMS_EXPRESS_SRC_UI_APPLICATION_HPP_

#include <string>
#include <tuple>
#include <vector>

#include "bitstream/BitstreamGeneratorParameters.hpp"
#include "encoding/Frame.hpp"

namespace tms_express {

class Application {
 public:
    ///////////////////////////////////////////////////////////////////////////
    // Typedefs ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    using Voicing = bool;

    ///////////////////////////////////////////////////////////////////////////
    // Constants //////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    static const Voicing kVoiced = true;

    static const Voicing kUnvoiced = false;

    ///////////////////////////////////////////////////////////////////////////
    // Initializers ///////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    explicit Application(SharedParameters params);

    ///////////////////////////////////////////////////////////////////////////
    // Bitstream I/O //////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Imports bitstream from disk and populates Frame table
    /// @param path Path to existing bitstream file
    std::vector<Frame> importBitstream(const std::string& path);

    /// @brief Exports Frame table to disk as bitstream file
    /// @param path Path to new bitstream file
    void exportBitstream(const std::vector<Frame>& frame_table,
                         BitstreamParameters params, const std::string& path);

    ///////////////////////////////////////////////////////////////////////////
    // Bitstream Routines /////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    // void encodeAudioFileToFrameTable();

    // void

    ///////////////////////////////////////////////////////////////////////////
    // LPC Analysis ///////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Estimates pitch periods for segments in sample
    /// @param params Lower vocal tract analysis parameters
    /// @return Single-row pitch table, with one pitch period estimate per
    ///         segment (in units of samples) of audio in samples
    std::vector<int> analyzeLowerVocalTract(LowerVocalTractParameters params,
                                            std::vector<float> samples) const;

    /// @brief Estimates LPC coefficients and gain of segments in sample
    /// @param params Upper vocal tract analysis parameters
    /// @return Tuple of coefficients table and gain table. The coefficients
    ///         table contains one row for each segment in the sample. Each
    ///         column corresponds to a different LPC reflector coefficient.
    ///         The gain table is a single row, with one column for each
    ///         segment in the sample.Each row corresponds to the predicted
    ///         gain for the segment, in Decibels.
    std::tuple<std::vector<std::vector<float>>, std::vector<float>>
    analyzeUpperVocalTract(UpperVocalTractParameters params,
                           std::vector<float> samples) const;

    /// @brief Categorizes each segment as voiced or unvoiced
    /// @param coeff_table LPC reflector coefficients
    /// @return Voicing table, with one voicing estimate per sample. A voicing
    ///         estimate of `true` corresponds to a voiced sample (vowel sound),
    ///         while an estimate of `false` corresponds to an unvoiced sample
    ///         (consonant sound)
    std::vector<Voicing> estimateVoicing(
        const std::vector<std::vector<float>>& coeff_table);

    ///////////////////////////////////////////////////////////////////////////
    // Frame Table ////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Combines analyses of various speech signal features into table
    ///
    /// @param pitch_table Pitch period estimates, one per segement
    /// @param coeff_table LPC reflector coefficients, model-order-M per segment
    /// @param gain_table Gain estimates, one per segment
    /// @param voicing_table Voicing estimates, one per segment
    /// @return Frame table, with one Frame object per segment
    std::vector<Frame> buildFrameTable(
        std::vector<int> pitch_table,
        std::vector<std::vector<float>> coeff_table,
        std::vector<float> gain_table, std::vector<bool> voicing_table) const;

    /// @brief Post-processes frame table to apply analysis-independent edits
    /// @param frame_table Vector of Frame objects representing input audio
    void postProcessFrameTable(std::vector<Frame>* frame_table,
                               PostProcessorParameters params) const;

    std::string serializeFrameTable(const std::vector<Frame>& frame_table,
                                    BitstreamParameters params);

    ///////////////////////////////////////////////////////////////////////////
    // Synthesis //////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    std::vector<float> synthesizeFrameTable(
        const std::vector<Frame>& frame_table) const;

 protected:
    SharedParameters shared_params_;
};

};  // namespace tms_express

#endif  // TMS_EXPRESS_SRC_UI_APPLICATION_HPP_
