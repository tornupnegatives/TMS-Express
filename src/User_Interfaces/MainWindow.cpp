//
// Created by Joseph Bellahcen on 5/1/23.
//
#include "User_Interfaces/MainWindow.h"
#include "../../gui/ui_MainWindow.h"
#include <QAction>
#include <QDir>
#include <QFileDialog>
#include <string.h>

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    // Menu bar signals
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(importAudioFile()));

}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::importAudioFile() {
    auto filePath = QFileDialog::getOpenFileName(this,
                                                 tr("Import audio file"),
                                                 QDir::homePath(),
                                                 tr("Audio Files (*.wav *.aif *.mp3)")
                                                 );

    qDebug() << "Importing " << filePath;

}
