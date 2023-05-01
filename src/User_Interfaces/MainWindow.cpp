//
// Created by Joseph Bellahcen on 5/1/23.
//
#include "User_Interfaces/MainWindow.h"
#include "../../gui/ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
}

MainWindow::~MainWindow() {
    delete ui;
}
