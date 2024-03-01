// Copyright 2023 Joseph Bellahcen <joeclb@icloud.com>

#ifdef TMSEXPRESS_GUI
#include <QApplication>
#endif  //  TMSEXPRESS_GUI

#include "ui/cli/CommandLineApp.hpp"

#ifdef TMSEXPRESS_GUI
#include "ui/gui/MainWindow.hpp"
#endif  //  TMSEXPRESS_GUI

int runCommandLineApp(int argc, char **argv) {
    auto cli = tms_express::ui::CommandLineApp();
    int status = cli.run(argc, argv);
    return status;
}

#ifdef TMSEXPRESS_GUI
int runGraphicalApp(int argc, char **argv) {
    QApplication app(argc, argv);

    tms_express::ui::MainWindow w;
    w.show();

    return app.exec();
}
#endif  //  TMSEXPRESS_GUI

int main(int argc, char **argv) {
    // If no arguments are passed, launch GUI
    if (argc == 1) {
#ifdef TMSEXPRESS_GUI
        runGraphicalApp(argc, argv);
#else
        runCommandLineApp(argc, argv);
#endif
    }

    // Otherwise, use command-line interface
    if (argc > 1) {
        runCommandLineApp(argc, argv);
    }
}
