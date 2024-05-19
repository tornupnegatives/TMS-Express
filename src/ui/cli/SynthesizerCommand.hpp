// Copyright (C) 2022-2024 Joseph Bellahcen <joeclb@icloud.com>

#ifndef TMS_EXPRESS_SRC_UI_CLI_SYNTHESIZERCOMMAND_HPP_
#define TMS_EXPRESS_SRC_UI_CLI_SYNTHESIZERCOMMAND_HPP_

#include <string>

#include <CLI/CLI.hpp>

#include "bitstream/BitstreamGeneratorParameters.hpp"

namespace tms_express::ui {

class SynthesizerCommand {
 public:
    ///////////////////////////////////////////////////////////////////////////
    // Initializers ///////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    SynthesizerCommand() = delete;

    ///////////////////////////////////////////////////////////////////////////
    // Interface //////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    static void setup(CLI::App* app);

    static int run(const std::string& input_path,
                   const std::string& output_path,
                   const PostProcessorParameters& post_params);
};

};  // namespace tms_express::ui

#endif  // TMS_EXPRESS_SRC_UI_CLI_SYNTHESIZERCOMMAND_HPP_
