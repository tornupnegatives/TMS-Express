// Author: Joseph Bellahcen <joeclb@icloud.com>

#ifndef TMS_EXPRESS_MAINWINDOW_H
#define TMS_EXPRESS_MAINWINDOW_H

#include "Audio/AudioBuffer.h"
#include "Audio/AudioFilter.h"
#include "Bitstream_Generation/BitstreamGenerator.h"
#include "Frame_Encoding/Frame.h"
#include "Frame_Encoding/FrameEncoder.h"
#include "Frame_Encoding/FramePostprocessor.h"
#include "Frame_Encoding/Synthesizer.h"
#include "LPC_Analysis/PitchEstimator.h"
#include "LPC_Analysis/LinearPredictor.h"
#include "User_Interfaces/AudioWaveform.h"

#include <QMainWindow>
#include <QMediaPlayer>
#include <QtMultimedia/QAudioOutput>

#include <vector>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

public slots:
    void onOpenFile();
    void onSaveBitstream();
    void onExportAudio();

    // Play buttons
    void onInputAudioPlay();
    void onLpcAudioPlay();

    // Control panels
    void onPitchParamEdit();
    void onLpcParamEdit();
    void onPostProcEdit();

private:
    // Qt modules
    Ui::MainWindow *ui;
    QMediaPlayer *player;
    QAudioOutput *audioOutput;

    // Custom Qt widgets
    AudioWaveform *inputWaveform;
    AudioWaveform *lpcWaveform;

    // Audio buffers
    AudioBuffer *inputBuffer;
    AudioBuffer *lpcBuffer;

    // Data tables
    std::vector<Frame> frameTable;
    std::vector<int> pitchPeriodTable;
    std::vector<float> pitchFrqTable;

    // Analysis objects
    Synthesizer synthesizer = Synthesizer();
    AudioFilter filter = AudioFilter();
    PitchEstimator pitchEstimator = PitchEstimator(8000);
    LinearPredictor linearPredictor = LinearPredictor();
    FramePostprocessor framePostprocessor = FramePostprocessor(&frameTable);

    // UI helpers
    void configureUiSlots();
    void configureUiState();
    void drawPlots();

    // Data manipulation
    void performBitstreamParsing(const std::string& path);
    void performPitchAnalysis();
    void performLpcAnalysis();
    void performPostProc();
    void exportBitstream(const std::string& path);

    // Metadata
    unsigned int samplesChecksum(std::vector<float> samples);

    // UI getters
    int pitchHpfCutoff();
    int pitchLpfCutoff();
    float pitchPreemph();
    int pitchMinFrq();
    int pitchMaxFrq();

    float lpcWindowWidth();
    int lpcHpfCutoff();
    int lpcLpfCutoff();
    float lpcPreemph();
    float lpcMaxUnvoicedGain();
    float lpcMaxVoicedGain();

    int postPitchShift();
    int postPitchOverride();
    int postGainShift();
};

#endif //TMS_EXPRESS_MAINWINDOW_H
