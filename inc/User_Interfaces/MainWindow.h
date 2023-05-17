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
    void applyAudioPreprocessing();
    // TODO: applyBitstreamPostProcessing();
    void updatePlot();
    void playAudio();


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
    void toggleGroupBoxes(bool enabled);

    // UI getters
    float windowWidthMs();
    int highpassFilterCutoffHz();
    int lowpassFilterCutoffHz();
    float preEmphasisAlpha();
    float maxVoicedGainDb();
    float maxUnvoicedGainDb();
    int maxPitchHz();
    int minPitchHz();
};

#endif //TMS_EXPRESS_MAINWINDOW_H
