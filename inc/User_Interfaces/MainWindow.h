//
// Created by Joseph Bellahcen on 5/1/23.
//

#ifndef TMS_EXPRESS_MAINWINDOW_H
#define TMS_EXPRESS_MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
};

#endif //TMS_EXPRESS_MAINWINDOW_H
