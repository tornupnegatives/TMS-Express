// Author: Joseph Bellahcen <joeclb@icloud.com>

#ifndef TMS_EXPRESS_MAINWINDOW_H
#define TMS_EXPRESS_MAINWINDOW_H

#include "Audio/AudioBuffer.h"
#include "Audio/AudioFilter.h"
#include "LPC_Analysis/PitchEstimator.h"
#include "User_Interfaces/AudioWaveform.h"

#include <QMainWindow>

#include <vector>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

public slots:
    void importAudioFile();
    // TODO: exportBitstream();

    // Pitch analysis functions
    void performPitchAnalysis();
    void playInputAudio();
    void plotInputAudio();

    // LPC analysis functions
    // TODO: performLpcAnalysis();
    // TODO: playLpcAudio();
    // TODO: plotLpcAudio();

private:
    Ui::MainWindow *ui;
    AudioWaveform *audioWaveform;

    AudioBuffer* audioBuffer;
    AudioFilter audioFilter;

    PitchEstimator pitchEstimator = PitchEstimator(8000);
    std::vector<float> pitchTable;

    // Analysis functions
    void computePitchTable();

    // UI state controls
    void updatePitchControls();

    // Pitch UI getters
    int pitchHpfCutoff();
    int pitchLpfCutoff();
    int pitchMaxFrq();
    int pitchMinFrq();
    float pitchPreemph();

    // LPC UI getters
    float maxVoicedGainDb();
    float maxUnvoicedGainDb();
    float windowWidthMs();
};

#endif //TMS_EXPRESS_MAINWINDOW_H
