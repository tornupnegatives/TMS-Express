//
// Created by Joseph Bellahcen on 5/1/23.
//

#ifndef TMS_EXPRESS_MAINWINDOW_H
#define TMS_EXPRESS_MAINWINDOW_H

#include "Audio/AudioBuffer.h"
#include "Audio/AudioFilter.h"
#include "User_Interfaces/AudioWaveform.h"
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

public slots:
    void importAudioFile();
    //void exportBitstream();

    void applyAudioPreprocessing();
    //void applyBitstreamPostProcessing();

    void playAudio();

private:
    Ui::MainWindow *ui;
    AudioWaveform *audioWaveform;

    AudioBuffer* audioBuffer;
    AudioFilter audioFilter;
};

#endif //TMS_EXPRESS_MAINWINDOW_H
