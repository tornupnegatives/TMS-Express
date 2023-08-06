// Author: Joseph Bellahcen <joeclb@icloud.com>

#ifndef TMS_EXPRESS_MAINWINDOW_H
#define TMS_EXPRESS_MAINWINDOW_H

#include "Audio/AudioBuffer.hpp"
#include "Audio/AudioFilter.hpp"
#include "Bitstream_Generation/BitstreamGenerator.hpp"
#include "Frame_Encoding/Frame.hpp"
#include "Frame_Encoding/FrameEncoder.hpp"
#include "Frame_Encoding/FramePostprocessor.hpp"
#include "Frame_Encoding/Synthesizer.hpp"
#include "LPC_Analysis/PitchEstimator.hpp"
#include "LPC_Analysis/LinearPredictor.hpp"
#include "User_Interfaces/Audio_Waveform/AudioWaveformView.hpp"
#include "User_Interfaces/Control_Panels/ControlPanelPitchView.hpp"
#include "User_Interfaces/Control_Panels/ControlPanelLpcView.hpp"
#include "User_Interfaces/Control_Panels/ControlPanelPostView.hpp"

#include <QMainWindow>
#include <QMediaPlayer>
#include <QtMultimedia/QAudioOutput>
#include <QtWidgets>

#include <vector>

#define TMS_EXPRESS_WINDOW_MIN_WIDTH    1000
#define TMS_EXPRESS_WINDOW_MIN_HEIGHT   800
#define TMS_EXPRESS_WINDOW_MARGINS      5
#define TMS_EXPRESS_AUDIO_SAMPLE_RATE   8000

namespace tms_express::ui {

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
    // Layouts
    QWidget *mainWidget;
    QVBoxLayout *mainLayout;
    QGroupBox *controlPanelGroup;
    QHBoxLayout *controlPanelLayout;

    // Control panels
    ControlPanelPitchView *pitchControl;
    ControlPanelLpcView *lpcControl;
    ControlPanelPostView *postControl;

    // Menu bar
    QMenuBar *menuBar;
    QAction *actionExport;
    QAction *actionOpen;
    QAction *actionSave;

    // Multimedia
    QMediaPlayer *player;
    QAudioOutput *audioOutput;

    // Custom Qt widgets
    AudioWaveformView *inputWaveform;
    AudioWaveformView *lpcWaveform;

    // Audio buffers
    AudioBuffer inputBuffer;
    AudioBuffer lpcBuffer;

    // Data tables
    std::vector<Frame> frameTable;
    std::vector<int> pitchPeriodTable;
    std::vector<float> pitchFrqTable;

    // Analysis objects
    Synthesizer synthesizer = Synthesizer();
    AudioFilter filter = AudioFilter();
    PitchEstimator pitchEstimator = PitchEstimator(TMS_EXPRESS_AUDIO_SAMPLE_RATE);
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
};

};  // namespace tms_express

#endif //TMS_EXPRESS_MAINWINDOW_H