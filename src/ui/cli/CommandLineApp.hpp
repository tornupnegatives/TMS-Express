// Copyright (C) 2022-2024 Joseph Bellahcen <joeclb@icloud.com>

#ifndef TMS_EXPRESS_SRC_UI_CLI_COMMANDLINEAPP_HPP_
#define TMS_EXPRESS_SRC_UI_CLI_COMMANDLINEAPP_HPP_

#include <CLI/CLI.hpp>

namespace tms_express::ui {

/// @brief Exposes command-line interface (CLI) for application
class CommandLineApp : public CLI::App {
 public:
    /// @brief Creates and configures a new CLI for application
    CommandLineApp();
};

};  // namespace tms_express::ui

#endif  // TMS_EXPRESS_SRC_UI_CLI_COMMANDLINEAPP_HPP_
