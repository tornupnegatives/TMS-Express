// Copyright (C) 2022-2024 Joseph Bellahcen <joeclb@icloud.com>

#include "ui/cli/CommandLineApp.hpp"

#include <memory>

#include "bitstream/BitstreamGeneratorParameters.hpp"
#include "ui/Application.hpp"
#include "ui/cli/EncoderCommand.hpp"
#include "ui/cli/GuiCommand.hpp"
#include "ui/cli/SynthesizerCommand.hpp"

namespace tms_express::ui {

///////////////////////////////////////////////////////////////////////////////
// Initializers ///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

CommandLineApp::CommandLineApp() {
    // Instantiate a shared Application class for use by all the Commands. This
    // lets us implement CLI11 subcommands using static member functions while
    // forcing the Commands to make use of the Application class
    auto app = std::make_shared<Application>(
        Application({kDefaultSampleRateHz, kDefaultWindowWidthMs}));

    EncoderCommand::setup(this, app);
    SynthesizerCommand::setup(this, app);

    require_subcommand(1);
}

};  // namespace tms_express::ui
