// Copyright (C) 2022-2024 Joseph Bellahcen <joeclb@icloud.com>

#include <memory>

#include <CLI/App.hpp>

#include "ui/Application.hpp"
#include "ui/cli/EncoderCommand.hpp"
#include "ui/cli/SynthesizerCommand.hpp"

#ifdef TMSEXPRESS_GUI
#include "ui/cli/GuiCommand.hpp"
#endif

int main(int argc, char** argv) {
    // TMS Express is a command-line app through-and-through. In other words,
    // the CLI11 library not only handles the CLI but also lauching the GUI
    auto cli = CLI::App(
        "Bitstream encoder and synthesizer for TMS5220 Voice Synthesis "
        "Processor",
        "tmsexpress");

    // Because of the way CLI11 works, an object-oriented approach toward
    // sub-commands is not possible. This, each subcommand is configured using
    // static member functions. To encourage each subcommand to make use of the
    // Application class rather than redefine common functionality from scratch,
    // a shared Application pointer is required to set up each sub-command
    auto app = std::make_shared<tms_express::Application>(
        tms_express::Application({tms_express::kDefaultSampleRateHz,
                                  tms_express::kDefaultWindowWidthMs}));

    tms_express::ui::EncoderCommand::setup(&cli, app);
    tms_express::ui::SynthesizerCommand::setup(&cli, app);

#ifdef TMSEXPRESS_GUI
    // tms_express::ui::GuiCommand::setup(&cli, app);
#endif

    cli.require_subcommand(1);

    // The CLI11_PARSE macro returns the exit status of the application
    CLI11_PARSE(cli, argc, argv);
}
