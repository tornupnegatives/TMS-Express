// Copyright (C) 2022-2024 Joseph Bellahcen <joeclb@icloud.com>

#ifndef TMS_EXPRESS_SRC_UI_CLI_ENCODERCOMMAND_HPP_
#define TMS_EXPRESS_SRC_UI_CLI_ENCODERCOMMAND_HPP_

#include <string>

#include <CLI/CLI.hpp>

#include "bitstream/BitstreamGeneratorParameters.hpp"

namespace tms_express::ui {

class EncoderCommand {
 public:
    ///////////////////////////////////////////////////////////////////////////
    // Initializers ///////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    EncoderCommand() = delete;

    ///////////////////////////////////////////////////////////////////////////
    // Interface //////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    static void setup(CLI::App* app);

    static int run(const std::string& input_path,
                   const std::string& output_path,
                   const SharedParameters& shared_params,
                   const UpperVocalTractParameters& upper_params,
                   const LowerVocalTractParameters& lower_params,
                   const PostProcessorParameters& post_params,
                   const BitstreamParameters& bitstream_params);
};

};  // namespace tms_express::ui

#endif  // TMS_EXPRESS_SRC_UI_CLI_ENCODERCOMMAND_HPP_
