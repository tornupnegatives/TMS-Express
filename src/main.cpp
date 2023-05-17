// Author: Joseph Bellahcen <joeclb@icloud.com>

#include "User_Interfaces/CommandLineApp.h"
#include "User_Interfaces/MainWindow.h"
#include <QApplication>

int main(int argc, char **argv) {
    // If no arguments are passed, launch GUI
    if (argc == 1) {
        QApplication app(argc, argv);

        MainWindow w;
        w.show();

        return app.exec();
    }

    // Otherwise, use command-line interface
    if (argc > 1) {
        CommandLineApp cli = CommandLineApp();
        int status = cli.run(argc, argv);
        exit(status);
    }
}
