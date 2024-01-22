// Copyright 2023 Joseph Bellahcen <joeclb@icloud.com>

#include <QApplication>

#include "ui/CommandLineApp.hpp"
#include "ui/MainWindow.hpp"

int main(int argc, char **argv) {
    // If no arguments are passed, launch GUI
    if (argc == 1) {
        QApplication app(argc, argv);

        tms_express::ui::MainWindow w;
        w.show();

        return app.exec();
    }

    // Otherwise, use command-line interface
    if (argc > 1) {
        auto cli = tms_express::ui::CommandLineApp();
        int status = cli.run(argc, argv);
        exit(status);
    }
}
