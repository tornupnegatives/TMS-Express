// Copyright (C) 2022-2024 Joseph Bellahcen <joeclb@icloud.com>

#include "ui/cli/GuiCommand.hpp"

#include <QApplication>

#include "ui/gui/MainWindow.hpp"

namespace tms_express::ui {

void GuiCommand::setup(CLI::App* app) {
    auto* sub = app->add_subcommand("gui", "Launches graphical frontend");
    sub->callback([]() { run(); });
}

int GuiCommand::run() {
    int argc = 0;
    const QApplication app(argc, nullptr);

    tms_express::ui::MainWindow window;
    window.show();

    return QApplication::exec();
}

};  // namespace tms_express::ui
