// Copyright (C) 2022-2024 Joseph Bellahcen <joeclb@icloud.com>

#ifndef TMS_EXPRESS_SRC_UI_CLI_GUICOMMAND_HPP_
#define TMS_EXPRESS_SRC_UI_CLI_GUICOMMAND_HPP_

#include <CLI/CLI.hpp>

#include "ui/Application.hpp"

namespace tms_express::ui {

class GuiCommand {
 public:
    static void setup(CLI::App* app);
    static int run();
};

};  // namespace tms_express::ui

#endif  // TMS_EXPRESS_SRC_UI_CLI_GUICOMMAND_HPP_
