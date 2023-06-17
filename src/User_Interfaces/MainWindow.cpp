//
// Created by Joseph Bellahcen on 5/1/23.
//

#include "Audio/AudioBuffer.h"
#include "Frame_Encoding/Frame.h"
#include "Frame_Encoding/FramePostprocessor.h"
#include "Frame_Encoding/Tms5220CodingTable.h"
#include "LPC_Analysis/Autocorrelator.h"
#include "User_Interfaces/Audio_Waveform/AudioWaveformView.h"
#include "User_Interfaces/MainWindow.h"
#include "User_Interfaces/Control_Panels/ControlPanelPitchView.h"
#include "User_Interfaces/Control_Panels/ControlPanelLpcView.h"
#include "User_Interfaces/Control_Panels/ControlPanelPostView.h"

#include "CRC.h"

#include <QFileDialog>
#include <QMediaPlayer>
#include <QtMultimedia/QAudioOutput>

#include <fstream>
#include <iostream>

/// Setup the main window of the application
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setMinimumSize(1024, 700);
    centralWidget = new QWidget(this);

    // Menu Bar
    actionExport = new QAction(this);
    actionExport->setText("Export");
    actionExport->setShortcut(QKeySequence("Ctrl+E"));

    actionOpen = new QAction(this);
    actionOpen->setText("Open");
    actionOpen->setShortcut(QKeySequence("Ctrl+O"));

    actionSave = new QAction(this);
    actionSave->setText("Save");
    actionSave->setShortcut(QKeySequence("Ctrl+S"));

    auto menubar = new QMenuBar(this);
    auto menuFile = new QMenu(menubar);
    menuFile->setTitle("File");
    setMenuBar(menubar);

    menubar->addAction(menuFile->menuAction());
    menuFile->addAction(actionOpen);
    menuFile->addAction(actionSave);
    menuFile->addAction(actionExport);

    // Control panel
    auto mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setObjectName("MainWindow::MainLayout");

    auto controlPanelGroup = new QGroupBox("Control Panel");
    controlPanelGroup->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    auto controlPanelLayout = new QHBoxLayout(controlPanelGroup);

    pitchControl = new ControlPanelPitchView();
    lpcControl = new ControlPanelLpcView();
    postControl = new ControlPanelPostView();

    controlPanelLayout->addWidget(pitchControl);
    controlPanelLayout->addWidget(lpcControl);
    controlPanelLayout->addWidget(postControl);

    mainLayout->addWidget(controlPanelGroup);

    // Waveforms
    inputWaveform = new AudioWaveformView("Input Signal", 750, 150);
    mainLayout->addWidget(inputWaveform);

    lpcWaveform = new AudioWaveformView("Synthesized Signal", 750, 150);
    mainLayout->addWidget(lpcWaveform);

    setCentralWidget(centralWidget);

    player = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);

    inputBuffer = nullptr;
    lpcBuffer = nullptr;

    frameTable = {};
    pitchPeriodTable = {};
    pitchFrqTable = {};

    configureUiSlots();
    configureUiState();
}

/// Free pointers associated with UI
MainWindow::~MainWindow() {
    delete player;
    delete audioOutput;

    delete inputWaveform;
    delete lpcWaveform;

    delete inputBuffer;
    delete lpcBuffer;
}

///////////////////////////////////////////////////////////////////////////////
//                              UI Helpers
///////////////////////////////////////////////////////////////////////////////

/// Connect UI elements to member functions
void MainWindow::configureUiSlots() {
    // Menu bar
    connect(actionOpen, &QAction::triggered, this, &MainWindow::onOpenFile);
    connect(actionSave, &QAction::triggered, this, &MainWindow::onSaveBitstream);
    connect(actionExport, &QAction::triggered, this, &MainWindow::onExportAudio);

    // Control panels
    connect(pitchControl, &ControlPanelPitchView::signalStateChanged, this, &MainWindow::onPitchParamEdit);
    connect(lpcControl, &ControlPanelLpcView::signalStateChanged, this, &MainWindow::onLpcParamEdit);
    connect(postControl, &ControlPanelPostView::signalStateChanged, this, &MainWindow::onPostProcEdit);

    // Play buttons
    connect(inputWaveform, &AudioWaveformView::signalPlayButtonPressed, this, &MainWindow::onInputAudioPlay);
    connect(lpcWaveform, &AudioWaveformView::signalPlayButtonPressed, this, &MainWindow::onLpcAudioPlay);



    /*
    // Set slider ranges
    ui->postPitchShiftSlider->setMinimum(-64);
    ui->postPitchShiftSlider->setMaximum(64);
    ui->postPitchShiftSlider->setTickInterval(8);

    ui->postFixedPitchSlider->setMinimum(0);
    ui->postFixedPitchSlider->setMaximum(64);
    ui->postFixedPitchSlider->setTickInterval(16);

    ui->postGainShiftSlider->setMinimum(-16);
    ui->postGainShiftSlider->setMaximum(16);
    ui->postFixedPitchSlider->setTickInterval(1);
     */
}

/// Toggle UI elements based on the state of the application
void MainWindow::configureUiState() {
    // Get UI state
    auto disableAudioDependentObject = (inputBuffer == nullptr);
    auto disableBitstreamDependentObject = frameTable.empty();

    // Control panels
    pitchControl->setDisabled(disableAudioDependentObject);
    lpcControl->setDisabled(disableAudioDependentObject);
    postControl->setDisabled(disableBitstreamDependentObject);

    // Menu bar
    /*
    ui->actionSave->setDisabled(disableAudioDependentObject);
    ui->actionExport->setDisabled(disableAudioDependentObject);

    // Play buttons
    ui->inputAudioPlay->setDisabled(disableAudioDependentObject);
    ui->lpcAudioPlay->setDisabled(disableBitstreamDependentObject);

    // Control panel
    ui->pitchHpfLine->setDisabled(disablePitchControl || !ui->pitchHpfEnable->isChecked());
    ui->pitchLpfLine->setDisabled(disablePitchControl || !ui->pitchLpfEnable->isChecked());
    ui->pitchPreemphLine->setDisabled(disablePitchControl || !ui->pitchPreemphEnable->isChecked());
    ui->pitchMaxFrqLine->setDisabled(disablePitchControl);
    ui->pitchMinFrqLine->setDisabled(disablePitchControl);

    ui->lpcWindowWidthLine->setDisabled(disableLpcControl);
    ui->lpcHpfLine->setDisabled(disableLpcControl || !ui->lpcHpfEnable->isChecked());
    ui->lpcLpfLine->setDisabled(disableLpcControl || !ui->lpcLpfEnable->isChecked());
    ui->lpcPreemphLine->setDisabled(disableLpcControl || !ui->lpcPreemphEnable->isChecked());
    ui->lpcMaxUnvoicedGainLine->setDisabled(disableLpcControl);
    ui->lpcMaxVoicedGainLine->setDisabled(disableLpcControl);

    ui->postPitchShiftEnable->setDisabled(disableBitstreamDependentObject);
    ui->postPitchShiftSlider->setDisabled(disableBitstreamDependentObject || !ui->postPitchShiftEnable->isChecked());
    ui->postPitchOverrideEnable->setDisabled(disableBitstreamDependentObject);
    ui->postFixedPitchSlider->setDisabled(disableBitstreamDependentObject || !ui->postPitchOverrideEnable->isChecked());
    ui->postRepeatFramesEnable->setDisabled(disableBitstreamDependentObject);
    ui->postRepeatFramesEnable->setDisabled(disableBitstreamDependentObject);
    ui->postGainShiftEnable->setDisabled(disableBitstreamDependentObject);
    ui->postGainShiftSlider->setDisabled(disableBitstreamDependentObject || !ui->postGainShiftEnable->isChecked());

    // Ensure post-pitch manipulation checkboxes are exclusive
    ui->postPitchShiftEnable->setCheckable(!ui->postPitchOverrideEnable->isChecked());
    ui->postPitchOverrideEnable->setCheckable(!ui->postPitchShiftEnable->isChecked());
*/

}

/// Draw the input and output signal waveforms, along with an abstract representation of their associated pitch data
void MainWindow::drawPlots() {
    if (inputBuffer != nullptr) {
        inputWaveform->plotSamples(inputBuffer->getSamples());
    }

    if (!frameTable.empty()) {
        auto samples = synthesizer.synthesize(frameTable);
        lpcWaveform->plotSamples(samples);

        auto framePitchTable = std::vector<float>(frameTable.size());
        for (int i = 0; i < frameTable.size(); i++)
            // TODO: Parameterize
            framePitchTable[i] = (8000.0f / float(frameTable[i].quantizedPitch())) / float(pitchEstimator.getMaxFrq());

            lpcWaveform->plotPitch(framePitchTable);
    }
}

///////////////////////////////////////////////////////////////////////////////
//                              UI Slots
///////////////////////////////////////////////////////////////////////////////

void MainWindow::onOpenFile() {
    auto filePath = QFileDialog::getOpenFileName(this, "Open file",
                                                 QDir::homePath(),
                                                 "Audio Files (*.wav *.aif *.aiff *.raw *.wav *.caf *.flac);;" \
                                                 "ASCII Bitstream (*.lpc);;" \
                                                 "Binary Bitstream (*.bin)"
    );

    // Do nothing if user cancels request
    if (filePath.isNull()) {
        qDebug() << "Open file canceled";
        return;
    }

    if (inputBuffer != nullptr) {
        delete inputBuffer;
        inputBuffer = nullptr;
    }

    if (lpcBuffer != nullptr) {
        delete lpcBuffer;
        lpcBuffer = nullptr;
    }

    frameTable.clear();
    pitchPeriodTable.clear();
    pitchFrqTable.clear();

    // Import audio file
    if (filePath.endsWith(".wav", Qt::CaseInsensitive)) {
        // Enable gain normalization by default
        //ui->postGainNormalizeEnable->setChecked(true);

        inputBuffer = new AudioBuffer(filePath.toStdString(), 8000, lpcControl->analysisWindowWidth());
        lpcBuffer = new AudioBuffer(filePath.toStdString(), 8000, lpcControl->analysisWindowWidth());

        performPitchAnalysis();
        performLpcAnalysis();
        framePostprocessor = FramePostprocessor(&frameTable);
        performPostProc();

        configureUiState();
        drawPlots();
        return;
    }

    if (filePath.endsWith(".lpc", Qt::CaseInsensitive) || filePath.endsWith(".bin", Qt::CaseInsensitive)) {
        // Disable gain normalization to preserve original bitstream gain
        //ui->postGainNormalizeEnable->setChecked(false);

        performBitstreamParsing(filePath.toStdString());
        framePostprocessor = FramePostprocessor(&frameTable);
        performPostProc();

        configureUiState();
        drawPlots();
        return;
    }
}

/// Save bitstream to disk
void MainWindow::onSaveBitstream() {
    auto filePath = QFileDialog::getSaveFileName(this, "Save bitstream",
                                                 QDir::homePath(),
                                                 "ASCII Bitstream (*.lpc);;" \
                                                 "Binary Bitstream (*.bin)"
    );

    if (filePath.isNull()) {
        qDebug() << "Save bitstream canceled";
        return;
    }

    exportBitstream(filePath.toStdString());
}

/// Export synthesized bitstream to disk
void MainWindow::onExportAudio() {
    // Display file picker
    auto filePath = QFileDialog::getSaveFileName(this, "Export audio file",
                                                 QDir::homePath(),
                                                 "Audio Files (*.wav *.aif *.aiff *.raw *.wav *.caf *.flac)"
    );

    if (filePath.isNull()) {
        qDebug() << "Export audio canceled";
        return;
    }

    synthesizer.render(filePath.toStdString());
}

/// Play contents of input buffer
void MainWindow::onInputAudioPlay() {
    if (inputBuffer == nullptr) {
        qDebug() << "Requested play, but input buffer is null";
        return;
    }

    // Generate checksum of buffer to produce unique temporary filename
    //
    // The pre-emphasis alpha coefficient will be included in this computation, as its impact on the buffer may not
    // be significant enough to modify the buffer checksum alone
    char filename[31];
    auto checksum = samplesChecksum(inputBuffer->getSamples());
    snprintf(filename, 31, "tmsexpress_render_%x.wav", checksum);

    // Only render audio if this particular buffer does not exist
    auto tempDir = std::filesystem::temp_directory_path();
    tempDir.append(filename);
    qDebug() << "Playing " << tempDir.c_str();
    inputBuffer->render(tempDir);

    // Setup player and play
    player->setAudioOutput(audioOutput);
    player->setSource(QUrl::fromLocalFile(tempDir.c_str()));
    audioOutput->setVolume(100);
    player->play();
}

/// Play synthesized bitstream audio
void MainWindow::onLpcAudioPlay() {
    if (frameTable.empty()) {
        return;
    }

    synthesizer.synthesize(frameTable);

    // Generate checksum of buffer to produce unique temporary filename
    //
    // The pre-emphasis alpha coefficient will be included in this computation, as its impact on the buffer may not
    // be significant enough to modify the buffer checksum alone
    char filename[35];

    uint checksum = (lpcBuffer != nullptr) ? samplesChecksum(lpcBuffer->getSamples()) : samplesChecksum(synthesizer.samples());
    snprintf(filename, 35, "tmsexpress_lpc_render_%x.wav", checksum);

    // Only render audio if this particular buffer does not exist
    auto tempDir = std::filesystem::temp_directory_path();
    tempDir.append(filename);
    qDebug() << "Playing " << tempDir.c_str();
    synthesizer.render(tempDir);

    // Setup player and play
    player->setAudioOutput(audioOutput);
    player->setSource(QUrl::fromLocalFile(tempDir.c_str()));
    audioOutput->setVolume(100);
    player->play();
}

/// Re-perform pitch and LPC analysis when pitch controls are changed
void MainWindow::onPitchParamEdit() {
    configureUiState();

    if (inputBuffer != nullptr) {
        performPitchAnalysis();
        performLpcAnalysis();

        drawPlots();
    }
}

/// Re-perform LPC analysis when LPC controls are changed
void MainWindow::onLpcParamEdit() {
    configureUiState();

    if (lpcBuffer != nullptr) {
        performLpcAnalysis();
        performPostProc();

        drawPlots();
    }
}

/// Re-perform LPC analysis when bitstream controls are changed
void MainWindow::onPostProcEdit() {
    configureUiState();

    if (!frameTable.empty()) {
        performPostProc();

        drawPlots();
    }
}

///////////////////////////////////////////////////////////////////////////////
//                              Metadata
///////////////////////////////////////////////////////////////////////////////

/// Compute checksum of a vector of floating-point samples to uniquely identify it
///
/// \note   The checksum is used to generate a unique filename, which is stored temporarily to facilitate playback of
///         rendered audio. In addition to the samples from the vector, this checksum also incorporates the pre-emphasis
///         filter coefficients of both the input audio and synthesized bitstream buffers, as applying the small
///         pre-emphasis filter to floating point samples may not impact the checksum.
///
/// \param samples Samples of which to compute checksum
/// \return Checksum
unsigned int MainWindow::samplesChecksum(std::vector<float> samples) {
    auto bufferSize = int(samples.size());
    auto checksumBuffer = (float *) malloc(sizeof(float) * (bufferSize + 1));
    memccpy(checksumBuffer, samples.data(), bufferSize, sizeof(float));
    checksumBuffer[bufferSize] = char(lpcControl->preemphAlpha() + pitchControl->preemphAlpha());

    auto checksum = CRC::Calculate(checksumBuffer, sizeof(float), CRC::CRC_32());

    free(checksumBuffer);
    return checksum;
}

///////////////////////////////////////////////////////////////////////////////
//                              Data Manipulation
///////////////////////////////////////////////////////////////////////////////

/// Apply filters to input buffer and perform pitch analysis to populate the pitch tables
void MainWindow::performPitchAnalysis() {
    // Clear tables
    inputBuffer->reset();
    pitchPeriodTable.clear();
    pitchFrqTable.clear();

    // Pre-process
    if (pitchControl->hpfEnabled()) {
        filter.highpass(*inputBuffer, pitchControl->hpfCutoff());
    }

    if (pitchControl->lpfEnabled()) {
        filter.lowpass(*inputBuffer, pitchControl->lpfCutoff());
    }

    if (pitchControl->preemphEnabled()) {
        filter.preEmphasis(*inputBuffer, pitchControl->preemphAlpha());
    }

    pitchEstimator.setMaxPeriod(pitchControl->minPitchFrq());
    pitchEstimator.setMinPeriod(pitchControl->maxPitchFrq());

    for (const auto &segment : inputBuffer->segments()) {
        auto acf = Autocorrelator::process(segment);
        auto pitchPeriod = pitchEstimator.estimatePeriod(acf);
        // TODO: Parameterize
        auto pitchFrq = pitchEstimator.estimateFrequency(acf) / float(pitchEstimator.getMaxFrq());

        pitchPeriodTable.push_back(pitchPeriod);
        pitchFrqTable.push_back(pitchFrq);
    }
}

/// Apply filters to input buffer and perform LPC analysis to populate the frame table
///
/// \note   This function may re-trigger pitch analysis if the window width has been modified, as both the pitch and
///         frame tables must share segment boundaries
void MainWindow::performLpcAnalysis() {
    // Clear tables
    lpcBuffer->reset();
    frameTable.clear();

    // Re-trigger pitch analysis if window width has changed
    if (lpcControl->analysisWindowWidth() != inputBuffer->getWindowWidth()) {
        inputBuffer->setWindowWidth(lpcControl->analysisWindowWidth());
        lpcBuffer->setWindowWidth(lpcControl->analysisWindowWidth());

        qDebug() << "Adjusting window width for pitch and LPC buffers";
        performPitchAnalysis();
    }

    // Pre-process
    if (lpcControl->hpfEnabled()) {
        qDebug() << "HPF";
        filter.highpass(*lpcBuffer, lpcControl->hpfCutoff());
    }

    if (lpcControl->lpfEnabled()) {
        qDebug() << "LPF";
        filter.lowpass(*lpcBuffer, lpcControl->lpfCutoff());
    }

    if (lpcControl->preemphEnabled()) {
        qDebug() << "PEF";
        filter.preEmphasis(*lpcBuffer, lpcControl->preemphAlpha());
    }

    for (int i = 0; i < lpcBuffer->size(); i++) {
        auto segment = lpcBuffer->segment(i);
        auto acf = Autocorrelator::process(segment);

        auto coeffs = linearPredictor.reflectorCoefficients(acf);
        auto gain = linearPredictor.gain();

        auto pitchPeriod = pitchPeriodTable[i];
        auto isVoiced = coeffs[0] < 0;

        frameTable.emplace_back(pitchPeriod, isVoiced, gain, coeffs);
    }

    framePostprocessor = FramePostprocessor(&frameTable);
}

/// Perform post-processing on and synthesize bitstream from frame table
void MainWindow::performPostProc() {
    // Clear tables
    framePostprocessor.reset();

    // Re-configure post-processor
    framePostprocessor.setMaxUnvoicedGainDB(postControl->maxUnvoicedGain());
    framePostprocessor.setMaxVoicedGainDB(postControl->maxVoicedGain());

    if (postControl->gainNormalizationEnabled()) {
        framePostprocessor.normalizeGain();
    }

    // Perform either a pitch shift or a fixed-pitch offset
    if (postControl->pitchShiftEnabled()) {
        framePostprocessor.shiftPitch(postControl->pitchShift());

    } else if (postControl->pitchOverrideEnabled()) {
        framePostprocessor.overridePitch(postControl->pitchOverride());
    }

    if (postControl->repeatFramesEnabled()) {
        auto nRepeatFrames = framePostprocessor.detectRepeatFrames();
        qDebug() << "Detected " << nRepeatFrames << " repeat frames";
    }

    if (postControl->gainShiftEnabled()) {
        framePostprocessor.shiftGain(postControl->gainShift());
    }

    synthesizer.synthesize(frameTable);
}

/// Import bitstream file from the disk and populate the frame table
void MainWindow::performBitstreamParsing(const std::string &path) {
    // Determine file extension
    auto filePath = QString::fromStdString(path);
    auto frameEncoder = FrameEncoder();

    if (filePath.endsWith(".lpc")) {
        auto frame_count = frameEncoder.importFromAscii(path);

    } else {
        // TODO: Binary parsing
        return;
    }

    frameTable = frameEncoder.frameTable();
}

void MainWindow::exportBitstream(const std::string& path) {
    auto filePath = QString::fromStdString(path);
    auto frameEncoder = FrameEncoder(frameTable, true);

    if (filePath.endsWith(".lpc")) {
        auto hex = frameEncoder.toHex();

        std::ofstream lpcOut;
        lpcOut.open(path);
        lpcOut << hex;
        lpcOut.close();

    } else if (filePath.endsWith(".bin")) {
        auto bin = frameEncoder.toBin();

        std::ofstream binOut(path, std::ios::out | std::ios::binary);
        binOut.write((char *)(bin.data()), long(bin.size()));
    }
}
